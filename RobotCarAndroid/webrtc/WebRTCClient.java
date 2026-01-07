package com.example.navigationleftexample.webrtc;

import android.content.Context;
import android.util.Log;

import com.example.navigationleftexample.utils.DataModel;
import com.example.navigationleftexample.utils.DataModelType;
import com.google.gson.Gson;

import org.webrtc.AudioSource;
import org.webrtc.AudioTrack;
import org.webrtc.Camera2Enumerator;
import org.webrtc.CameraVideoCapturer;
import org.webrtc.DefaultVideoDecoderFactory;
import org.webrtc.DefaultVideoEncoderFactory;
import org.webrtc.EglBase;
import org.webrtc.IceCandidate;
import org.webrtc.MediaConstraints;
import org.webrtc.MediaStream;
import org.webrtc.MediaStreamTrack;
import org.webrtc.PeerConnection;
import org.webrtc.PeerConnectionFactory;
import org.webrtc.RtpTransceiver;
import org.webrtc.SessionDescription;
import org.webrtc.SurfaceTextureHelper;
import org.webrtc.SurfaceViewRenderer;
import org.webrtc.VideoSource;
import org.webrtc.VideoTrack;

import java.util.ArrayList;
import java.util.List;

public class WebRTCClient {

    private final Gson gson = new Gson();
    private final Context context;
    private final String username;
    private EglBase.Context eglBaseContext= EglBase.create().getEglBaseContext();
    private PeerConnectionFactory peerConnectionFactory;
    private PeerConnection peerConnection;
    private List<PeerConnection.IceServer> iceServer = new ArrayList<>();
    private CameraVideoCapturer videoCapturer;
    private VideoSource localVideoSource;
    private AudioSource localAudioSource;
    private String localTrackId = "local_track";
    private String localStreamId = "local_stream";
    private VideoTrack localVideoTrack;
    private AudioTrack localAudioTrack;
    private MediaStream localStream;

    private SurfaceTextureHelper helper;


    private MediaConstraints mediaConstraints = new MediaConstraints();
    private boolean isRemoteRendererInitialized = false;
    private SurfaceViewRenderer initializedRemoteView = null;
    public Listener listener;

    public WebRTCClient(Context context, PeerConnection.Observer observer, String username) {
        this.context = context;
        this.username = username;

        try{
            initPeerConnectionFactory();
            peerConnectionFactory = createPeerConnectionFactory();
            //        iceServer.add(PeerConnection.IceServer.builder("turn:a.relay.metered.ca:443?transport=tcp")
            //                .setUsername("83eebabf8b4cce9d5dbcb649")
            //                .setPassword("2D7JvfkOQtBdYW3R").createIceServer());
            iceServer.add(PeerConnection.IceServer.builder("stun:stun.l.google.com:19302").createIceServer());

            peerConnection = createPeerConnection(observer);
            if (peerConnection == null) {
                Log.e("WebRTCClient", "PeerConnection creation failed!");
                // CRITICAL: Stop execution here. Do not proceed.
                throw new RuntimeException("Failed to create PeerConnection.");
            }
            peerConnection.addTransceiver(
                    MediaStreamTrack.MediaType.MEDIA_TYPE_VIDEO,
                    new RtpTransceiver.RtpTransceiverInit(
                            RtpTransceiver.RtpTransceiverDirection.RECV_ONLY
                    )
            );
            //localVideoSource = peerConnectionFactory.createVideoSource(false);
            //localAudioSource = peerConnectionFactory.createAudioSource(new MediaConstraints());
            mediaConstraints.mandatory.add(new MediaConstraints.KeyValuePair("OfferToReceiveVideo","true"));
        } catch (Exception e) {
            e.printStackTrace();
            Log.e("WebRTC", "Error initializing PeerConnection", e);
        }
    }

    //initializing peer connection section
    private void initPeerConnectionFactory() {
        PeerConnectionFactory.InitializationOptions options = PeerConnectionFactory.InitializationOptions.builder(context).setFieldTrials("WebRTC-H264HighProfile/Enabled/").setEnableInternalTracer(true).createInitializationOptions();
        PeerConnectionFactory.initialize(options);
    }

    private PeerConnectionFactory createPeerConnectionFactory() {
        PeerConnectionFactory.Options options = new PeerConnectionFactory.Options();
        options.disableEncryption = false;
        options.disableNetworkMonitor = false;
        return PeerConnectionFactory.builder()
                .setVideoEncoderFactory(new DefaultVideoEncoderFactory(eglBaseContext,true,true))
                .setVideoDecoderFactory(new DefaultVideoDecoderFactory(eglBaseContext))
                .setOptions(options).createPeerConnectionFactory();
    }

    private PeerConnection createPeerConnection(PeerConnection.Observer observer){
        return peerConnectionFactory.createPeerConnection(iceServer,observer);
    }

    //initilizing ui like surface view renderers

    public void initSurfaceViewRendere(SurfaceViewRenderer viewRenderer){
//        if (!isRemoteRendererInitialized) {
//            viewRenderer.setEnableHardwareScaler(true);
//            viewRenderer.setMirror(true);
//            viewRenderer.init(eglBaseContext,null);
//            isRemoteRendererInitialized = true;
//        }

        if (initializedRemoteView == viewRenderer) {
            // Already initialized THIS view
            return;
        }

        viewRenderer.setEnableHardwareScaler(true);
        viewRenderer.setMirror(false);
        viewRenderer.init(eglBaseContext, null);

        initializedRemoteView = viewRenderer;
    }

    public void initLocalSurfaceView(SurfaceViewRenderer view){
        initSurfaceViewRendere(view);
        startLocalVideoStreaming(view);
    }

    private void startLocalVideoStreaming(SurfaceViewRenderer view) {
        helper= org.webrtc.SurfaceTextureHelper.create(
                Thread.currentThread().getName(), eglBaseContext
        );

        videoCapturer = getVideoCapturer();
        videoCapturer.initialize(helper,context,localVideoSource.getCapturerObserver());
        videoCapturer.startCapture(480,360,15);
        localVideoTrack = peerConnectionFactory.createVideoTrack(
                localTrackId+"_video",localVideoSource
        );
        localVideoTrack.addSink(view);

        localAudioTrack = peerConnectionFactory.createAudioTrack(localTrackId+"_audio",localAudioSource);
        localStream = peerConnectionFactory.createLocalMediaStream(localStreamId);
        localStream.addTrack(localVideoTrack);
        localStream.addTrack(localAudioTrack);
        peerConnection.addStream(localStream);
    }

    private CameraVideoCapturer getVideoCapturer() {
        Camera2Enumerator enumerator = new Camera2Enumerator(context);

        String[] deviceNames = enumerator.getDeviceNames();

        for (String device: deviceNames){
            if (enumerator.isFrontFacing(device)){
                return enumerator.createCapturer(device,null);
            }
        }
        throw new IllegalStateException("front facing camera not found");
    }

    public void initRemoteSurfaceView(SurfaceViewRenderer view){
        initSurfaceViewRendere(view);
    }

    //negotiation section like call and answer
    public void call(String target){
        try{
            this.peerConnection.createOffer(new MySdpObserver(){
                @Override
                public void onCreateSuccess(SessionDescription sessionDescription) {
                    super.onCreateSuccess(sessionDescription);
                    peerConnection.setLocalDescription(new MySdpObserver(){
                        @Override
                        public void onSetSuccess() {
                            super.onSetSuccess();
                            //its time to transfer this sdp to other peer
                            if (listener!=null){
                                listener.onTransferDataToOtherPeer(new DataModel(
                                        target,username,sessionDescription.description, DataModelType.RTC_OFFER
                                ));
                            }
                        }
                    },sessionDescription);
                }
            },mediaConstraints);
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    public void answer(String target){
        if (peerConnection == null) {
            Log.e("WebRTCClient", "PeerConnection is null, cannot create answer.");
            return;
        }
        try{
            peerConnection.createAnswer(new MySdpObserver(){
                @Override
                public void onCreateSuccess(SessionDescription sessionDescription) {
                    Log.d("SdpObserver", "Answer created successfully. Setting local description.");
                    super.onCreateSuccess(sessionDescription);
                    peerConnection.setLocalDescription(new MySdpObserver(){
                        @Override
                        public void onSetSuccess() {
                            super.onSetSuccess();
                            Log.d("SdpObserver", "Local description set successfully. Sending answer to peer.");
                            //its time to transfer this sdp to other peer
                            if (listener!=null){
                                listener.onTransferDataToOtherPeer(new DataModel(
                                        target,username,sessionDescription.description, DataModelType.RTC_ANSWER
                                ));
                            }
                        }
                        @Override
                        public void onSetFailure(String s) {
                            Log.e("SdpObserver", "FAILED to set local description: " + s);
                        }
                    },sessionDescription);
                }
                @Override
                public void onCreateFailure(String s) {
                    Log.e("SdpObserver", "FAILED to create answer: " + s);
                }
            },mediaConstraints);
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    public void onRemoteSessionReceived(SessionDescription sessionDescription){
        if (peerConnection == null) {
            Log.e("WebRTCClient", "PeerConnection is null, cannot process remote session.");
            return;
        }
        peerConnection.setRemoteDescription(new MySdpObserver(){
            @Override
            public void onSetSuccess() {
                // THIS IS A CRITICAL LOG
                Log.d("SdpObserver", "Remote description set successfully. Now creating answer.");
                // Chain the next step
                answer("Raspberry");
            }

            @Override
            public void onSetFailure(String s) {
                // THIS IS THE MOST IMPORTANT LOG FOR DEBUGGING
                Log.e("SdpObserver", "FAILED to set remote description: " + s);
            }

            // You don't need these for setRemoteDescription
            @Override public void onCreateSuccess(SessionDescription sessionDescription) {}
            @Override public void onCreateFailure(String s) {}
        },sessionDescription);
        Log.d("WebRTCClient", "Setting remote description.");
    }

    public void addIceCandidate(IceCandidate iceCandidate){
        peerConnection.addIceCandidate(iceCandidate);
    }

    public void sendIceCandidate(IceCandidate iceCandidate, String target){
        addIceCandidate(iceCandidate);
        if (listener!=null){
            listener.onTransferDataToOtherPeer(new DataModel(
                    target,username,gson.toJson(iceCandidate),DataModelType.RTC_ICECANDIDATE
            ));
        }
    }

    public void switchCamera() {
        videoCapturer.switchCamera(null);
    }

    public void toggleVideo(Boolean shouldBeMuted){
        localVideoTrack.setEnabled(shouldBeMuted);

    }

    public void toggleAudio(Boolean shouldBeMuted){
        localAudioTrack.setEnabled(shouldBeMuted);
    }

    public void closeConnection(){
        try{
            if (peerConnection != null) {
                peerConnection.close();
//                peerConnection.dispose(); // Important for native resource cleanup
                //peerConnection = null;

            }
            // 3. Dispose of the SurfaceTextureHelper.
//            if (helper != null) {
//                helper.dispose();
//                helper = null;
//            }

//            if (peerConnectionFactory != null) {
//                peerConnectionFactory.dispose();
//                peerConnectionFactory = null;
//            }


        }catch (Exception e){
            e.printStackTrace();
        }
    }

    public interface Listener {
        void onTransferDataToOtherPeer(DataModel model);
    }
}