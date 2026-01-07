# from picamera2 import Picamera2

# FRAME_WIDTH  = 640
# FRAME_HEIGHT = 360
# FRAME_RATE   = 30

# class CameraSingleton:
#     _instance = None

#     def __new__(cls):
#         if cls._instance is None:
#             cls._instance = super().__new__(cls)
#             cls._instance.picam2 = Picamera2()
#             config = cls._instance.picam2.create_video_configuration(
#                 main={"size": (FRAME_WIDTH, FRAME_HEIGHT)},
#                 controls={"FrameRate": FRAME_RATE}
#             )
#             cls._instance.picam2.configure(config)
#             cls._instance.picam2.start()
#             print(f"[INFO] Pi Camera singleton started at {FRAME_WIDTH}x{FRAME_HEIGHT}@{FRAME_RATE}fps")
#         return cls._instance

from picamera2 import Picamera2
import threading

FRAME_WIDTH  = 640
FRAME_HEIGHT = 360
FRAME_RATE   = 30


class CameraSingleton:
    _instance = None
    _lock = threading.Lock()

    def __new__(cls):
        with cls._lock:
            if cls._instance is None:
                cls._instance = super().__new__(cls)
                cls._instance._init_camera()
            return cls._instance

    def _init_camera(self):
        self.picam2 = Picamera2()
        config = self.picam2.create_video_configuration(
            main={"size": (FRAME_WIDTH, FRAME_HEIGHT)},
            controls={"FrameRate": FRAME_RATE}
        )
        self.picam2.configure(config)
        self.picam2.start()
        print(f"[INFO] Pi Camera singleton started at {FRAME_WIDTH}x{FRAME_HEIGHT}@{FRAME_RATE}fps")

    @classmethod
    def shutdown(cls):
        with cls._lock:
            if cls._instance is not None:
                try:
                    print("[INFO] Shutting down Pi Camera singleton")
                    cls._instance.picam2.stop()
                    cls._instance.picam2.close()
                except Exception as e:
                    print("[WARN] Error stopping camera:", e)
                finally:
                    cls._instance = None