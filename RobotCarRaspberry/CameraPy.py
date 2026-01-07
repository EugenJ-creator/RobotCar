import asyncio
import av
import numpy as np
import threading
from queue import Queue, Empty
from aiortc import VideoStreamTrack
from CameraSingleton import CameraSingleton


class PiCameraVideoStreamTrack(VideoStreamTrack):
    kind = "video"

    def __init__(self):
        super().__init__()
        self.picam2 = CameraSingleton().picam2
        self.queue = Queue(maxsize=1)
        self.running = True

        self.thread = threading.Thread(
            target=self._capture_loop,
            daemon=True
        )
        self.thread.start()

        print("[INFO] PiCameraVideoStreamTrack initialized using singleton camera")

    # ✅ MUST be a class method
    def _capture_loop(self):
        while self.running:
            try:
                frame = self.picam2.capture_array()
                if not self.queue.full():
                    self.queue.put(frame)
            except Exception as e:
                print("[CAMERA ERROR]", e)
                break

    async def recv(self):
        pts, time_base = await self.next_timestamp()

        try:
            frame = await asyncio.get_running_loop().run_in_executor(
                None,
                lambda: self.queue.get(timeout=1)
            )
        except Empty:
            # Drop frame instead of blocking WebRTC
            await asyncio.sleep(0)
            return await self.recv()

        frame_rgb = np.ascontiguousarray(frame[..., :3])
        video_frame = av.VideoFrame.from_ndarray(frame_rgb, format="rgb24")
        video_frame.pts = pts
        video_frame.time_base = time_base
        return video_frame

    def stop(self):
        self.running = False