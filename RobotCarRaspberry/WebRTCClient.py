# Import required modules for WebRTC, Pi Camera video capture, and HTTP
import os
import asyncio
import aiohttp
import av
import numpy as np
from picamera2 import Picamera2
from CameraPy import PiCameraVideoStreamTrack
import DataModel
import json
from CameraSingleton import CameraSingleton
from aiortc import (
    RTCPeerConnection,
    RTCConfiguration,
    RTCIceServer,
    RTCIceCandidate,
    RTCSessionDescription,
    VideoStreamTrack
)


class WebRTCPublisher:
    def __init__(self):
        self.pc = None
        self.video_track = None
        self.ws = None   # store current websocket
    
    def set_ws(self, ws):
            print("[INFO] WebSocket rebound to existing publisher")
            self.ws = ws

    # === WebRTC Streaming Function ===
    async def startStream(self, ws):
        print("[INFO] Preparing WebRTC connection to Android...")
         # cleanup old session
        if self.pc:
            await self.stop()
        self.ws = ws
        config = RTCConfiguration(
            iceServers=[RTCIceServer(urls=["stun:stun.l.google.com:19302"])]
        )
        self.pc = RTCPeerConnection(configuration=config)
        self.pc.addIceCandidate

        @self.pc.on("connectionstatechange")
        async def on_connectionstatechange():
            state = self.pc.connectionState
            print(f"[INFO] Connection state: {state}")

            if state in ("failed",  "closed"):
                await self.stop()

        # Create camera + track
        #self.video_track = PiCameraVideoStreamTrack()

        # 🔥 REQUIRED
        # self.pc.addTransceiver("video", direction="sendonly")
        # self.pc.addTrack(self.video_track)
        #transceiver = self.pc.addTransceiver("video", direction="sendonly")
        #transceiver.sender.replaceTrack(self.video_track)

        @self.pc.on("icecandidate")
        async def on_icecandidate(candidate):
            if candidate is None:
                return

            print("[INFO] Sending ICE candidate")
            try:
                await self.ws.send(json.dumps({
                    "type": "RTC.IceCandidate",
                    "data": {
                        "candidate": candidate.candidate,
                        "sdpMid": candidate.sdpMid,
                        "sdpMLineIndex": candidate.sdpMLineIndex
                    }
                }))
            except Exception as e:
                print("[WARN] Failed to send WS message:", e)



        # Attach video track from Pi camera   
        self.video_track = PiCameraVideoStreamTrack()
        # This tells Android clearly:“Raspberry Pi will SEND video”
        self.pc.addTransceiver("video", direction="sendonly")
        self.pc.addTrack(self.video_track)

        # Create SDP offer
        offer = await self.pc.createOffer()
        await self.pc.setLocalDescription(offer)
        print("[INFO] SDP offer created successfully")

        # Send offer to WHIP endpoint
        print(f"[INFO] Sending offer to Android")
        
        # def __init__(self, target, sender, data, type):

        try:
            data = DataModel.DataModel(target="Android", sender="Raspberry", 
                                            data=self.pc.localDescription.sdp, type=DataModel.DataModelType.RTC_OFFER) 
            json_str = json.dumps(data.to_dict())

            print(f"Json sent to Android is: {json_str}")
            # Only send if WebSocket is open

            try:
                await self.ws.send(json_str)
            except Exception as e:
                print("[WARN] Failed to send WS message:", e)
                
        # ws.send("Sending from WebRTC")
        except Exception as e:
            print(f"[FATAL] Unhandled exception: {e}")

        





    async def setICECandidate(self, candidate_json):
        try:
            # Parse the JSON string
            candidate_data = json.loads(candidate_json)

            # # Map `sdp` → `candidate` for aiortc
            # candidate_dict = {
            #     "candidate": candidate_data["sdp"],  # this is correct mapping
            #     "sdpMid": candidate_data.get("sdpMid"),
            #     "sdpMLineIndex": candidate_data.get("sdpMLineIndex"),
            # }

            # candidate = RTCIceCandidate(
            #     sdpMid=candidate_data["sdpMid"],
            #     sdpMLineIndex=candidate_data["sdpMLineIndex"],
            #     candidate=candidate_data["sdp"]
            # )
            candidate = RTCIceCandidate(
                sdpMid=candidate_data.get("sdpMid"),
                sdpMLineIndex=candidate_data.get("sdpMLineIndex"),
                candidate=candidate_data.get("candidate")
            )



            await self.pc.addIceCandidate(candidate)
        except Exception as e:
            print("[WS] ERROR adding ICE candidate:", e)
        
        

    #     # Keep stream alive
    #     try:
    #         await asyncio.sleep(3600)
    #     except KeyboardInterrupt:
    #         print("[INFO] Stream interrupted by user.")
    #     finally:
    #         await pc.close()
    #         video_track.picam2.stop()
    #         print("[INFO] Stream closed and Pi Camera released.")



    async def setRemoteOffer(self, remote_offer):
        await self.pc.setRemoteDescription(
            RTCSessionDescription(sdp=remote_offer, type="answer")
        )
        
        print("[SUCCESS] WebRTC connection established with Android!")



    # # === Keep alive (OPTIONAL) ===
    # async def wait(self):
    #     while self.running:
    #         await asyncio.sleep(10000000)

    # === Cleanup ===
    async def stop(self):
        self.running = False

        # 🔥 Stop camera ONLY when call is fully done
        CameraSingleton.shutdown()

        if self.video_track:
            self.video_track.picam2.stop()
            self.video_track = None

        if self.pc:
            await self.pc.close()
            self.pc = None

        

        if self.ws:
            self.ws = None

        print("[INFO] Stream closed and Pi Camera released.")



        # # Keep stream alive
        # try:
        #     await asyncio.sleep(3600)
        # except KeyboardInterrupt:
        #     print("[INFO] Stream interrupted by user.")
        # finally:
        #     await self.pc.close()
        #     self.video_track.picam2.stop()
        #     print("[INFO] Stream closed and Pi Camera released.")            
