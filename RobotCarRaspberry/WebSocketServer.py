import asyncio
import json
import websockets
import DataModel
import logging
from WebRTCClient import WebRTCPublisher

from gpiozero import Servo
from time import sleep

from gpiozero.pins.pigpio import PiGPIOFactory

factory = PiGPIOFactory()

servoX = Servo(13, pin_factory= factory)
servoY = Servo(19, pin_factory= factory)



# -------- WebRTC publisher singleton --------
_publisher: WebRTCPublisher | None = None

def get_publisher() -> WebRTCPublisher:
    global _publisher
    if _publisher is None:
        print("[INFO] Creating WebRTCPublisher singleton")
        _publisher = WebRTCPublisher()
    return _publisher

async def destroy_publisher():
    global _publisher
    if _publisher:
        print("[INFO] Stopping WebRTCPublisher")
        await _publisher.stop()
        _publisher = None


def set_servo_x(value):
    servoX.value = value
    print(f"Setze Servo X auf {value}")
    


def set_servo_y(value):
    servoY.value = value
    print(f"Setze Servo Y auf {value}")
    


# -------- WebSocket handler --------
async def ws_handler(ws):
    print("[WS] Client connected")

    publisher = get_publisher()
    publisher.set_ws(ws)

    try:
        async for message in ws:
            print("[WS] Received:", message)

            try:
                model = DataModel.json_to_datamodel(message)
            except Exception as e:
                print("[ERROR] Invalid JSON:", e)
                continue

            match model.type:

                case DataModel.DataModelType.RTC_START_CALL:
                    if publisher.pc and publisher.pc.connectionState in (
                        "connecting", "connected"
                    ):
                        print("[INFO] Call already active, reusing")
                        continue

                    print("[INFO] Starting WebRTC stream")
                    try:
                        await publisher.startStream(ws)
                    except Exception as e:
                        # ⛑️ Protect WS + WebRTC from bad messages
                        print("[WS] ERROR handling message:", e)

                case DataModel.DataModelType.RTC_ANSWER:
                    if publisher.pc:
                        try:
                            await publisher.setRemoteOffer(model.data)
                        except Exception as e:
                            # ⛑️ Protect WS + WebRTC from bad messages
                            print("[WS] ERROR handling message:", e)
                        

                case DataModel.DataModelType.RTC_ICE_CANDIDATE:
                    if publisher.pc:
                        try:
                            await publisher.setICECandidate(model.data)
                        except Exception as e:
                            # ⛑️ Protect WS + WebRTC from bad messages
                            print("[WS] ERROR handling message:", e)
                        

                case DataModel.DataModelType.RTC_END_CALL:
                    await destroy_publisher()

                case DataModel.DataModelType.CONTROL_STEAR_CAMERA:
                    

                    try: 
                        msg = json.loads(model.data)
                
                        # X/Y auslesen
                        x = msg.get("x")
                        y = msg.get("y")
                        
                        # Werte validieren
                        if x is not None:
                            set_servo_x(x)

                        if y is not None:
                            set_servo_y(y)

                    except Exception as e:
                        print("Fehler:", e)
                        ws.send("Invalid JSON")

                #ws.send("OK")





                case _:
                    print("[WARN] Unknown message type")

    except websockets.exceptions.ConnectionClosed as e:
        print(f"[WS] Client disconnected ({e.code})")

    except Exception as e:
        print("[FATAL] WS error:", e)

    finally:
        print("[WS] Cleaning up")
        await destroy_publisher()


# -------- Server entrypoint --------
async def main():
    server = await websockets.serve(
        ws_handler,
        host="0.0.0.0",
        port=5000,
        ping_interval=10,   # 🔥 keepalive
        ping_timeout=10
    )

    print("[WS] Server started on port 5000")
    await server.wait_closed()


if __name__ == "__main__":
    asyncio.run(main())