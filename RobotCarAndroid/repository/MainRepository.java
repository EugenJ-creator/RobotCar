package com.example.navigationleftexample.repository;

import android.content.Context;
import android.util.Log;


import com.example.navigationleftexample.utils.DataModel;
import com.example.navigationleftexample.utils.DataModelType;
import com.example.navigationleftexample.utils.ErrorCallBack;
import com.example.navigationleftexample.utils.NewEventCallBack;
import com.example.navigationleftexample.utils.SuccessCallBack;
import com.example.navigationleftexample.webrtc.MyPeerConnectionObserver;
import com.example.navigationleftexample.webrtc.WebRTCClient;
import com.example.navigationleftexample.websocket.MyWebSocketClient;
import com.example.navigationleftexample.websocket.WebSocketManager;
import com.google.gson.Gson;
import com.fasterxml.jackson.databind.ObjectMapper;

import org.webrtc.IceCandidate;
import org.webrtc.MediaStream;
import org.webrtc.MediaStreamTrack;
import org.webrtc.PeerConnection;
import org.webrtc.PeerConnectionFactory;
import org.webrtc.RtpTransceiver;
import org.webrtc.SessionDescription;
import org.webrtc.SurfaceViewRenderer;
import org.webrtc.VideoTrack;

import java.net.URI;
import java.net.URISyntaxException;



import java.net.URI;
import java.net.URISyntaxException;

public class MainRepository implements WebRTCClient.Listener {

    //private final PeerConnectionFactory peerConnectionFactory; // <-- LONG-LIVED FACTORY
    public Listener listener;
    private final Gson gson = new Gson();
    private MyWebSocketClient myWebSocketClient;

    private org.webrtc.VideoTrack remoteVideoTrack;
    private WebRTCClient webRTCClient;

    private String currentUsername = "Android";
    private String target = "Raspberry";

    private boolean isRemoteViewInitialized = false;
    public  String getTarget() {
        return target;
    }

    private StreamReadyListener streamReadyListener;

    public void setStreamReadyListener(StreamReadyListener listener) {
        this.streamReadyListener = listener;
    }

    public interface StreamReadyListener {
        void onRemoteStreamReady();
    }
    public String getCurrentUsername() {
        return currentUsername;
    }

    private SurfaceViewRenderer remoteView;

    private static String RASPBERRY_IP = "192.168.178.24";

    public static void setRaspberryIp(String raspberryIp) {
        RASPBERRY_IP = raspberryIp;
    }

    public enum CallState {
        IDLE,
        CALLING,
        CONNECTED
    }

    private CallState callState = CallState.IDLE;

    public boolean isCallActive() {
        return callState == CallState.CALLING || callState == CallState.CONNECTED;
    }




    public static String getRaspberryIp() {
        return RASPBERRY_IP;
    }



    private void updateCurrentUsername(String username){
        this.currentUsername = username;
    }

    private MainRepository(){


        //  TODO  URI must be loaded from a Variable. that should be each time updated if IP is changed
        //  Create URI of the Web Socket Server (Raspberry)
        URI uriRaspWebSocket;
        try {
            // Connect to local host
            uriRaspWebSocket = new URI("ws://"+getRaspberryIp()+":5000/ws");


            myWebSocketClient = WebSocketManager.getInstance(uriRaspWebSocket).getWebSocket();

        }
        catch (URISyntaxException e) {
            Log.e("WebSocketManager", "Error initializing websocket", e);
            myWebSocketClient = null;  // explicitly set to null
            e.printStackTrace();
            return;
        }

    }

    private static MainRepository instance;
    public static MainRepository getInstance(){
        if (instance == null) {
            synchronized (MainRepository.class) {
                if (instance == null) {
                    instance = new MainRepository();
                }
            }
        }
        return instance;
    }

    /**
     * WARNING: This is a destructive operation and is NOT a recommended pattern.
     * It will clean up all resources in this repository and then destroy the
     * singleton instance, forcing it to be recreated on the next getInstance() call.
     */
    public static void destroyInstance() {
        if (instance != null) {
            synchronized (MainRepository.class) {
                if (instance != null) {
                    Log.d("MainRepository", "Starting destruction of MainRepository instance.");
                    // 1. Call the internal cleanup method on the instance
                    instance.cleanup();
                    // 2. Set the static instance to null
                    instance = null;
                    Log.d("MainRepository", "MainRepository instance has been destroyed.");
                }
            }
        } else {
            Log.d("MainRepository", "Instance was already null. No action taken.");
        }
    }

    /**
     * An internal cleanup method to release all resources held by the repository.
     */
    private void cleanup() {
        Log.d("MainRepository", "Running internal cleanup...");
        // This is where you clean up EVERYTHING the repository holds.
        endCall(); // Clean up WebRTC client
        // if you have a Retrofit client, you might clean it here.
        // if you have a database connection, close it here.
        // etc.
    }



    public void initWebRTCClient(String username, Context context, SuccessCallBack callBack){


        this.webRTCClient = new WebRTCClient(context,new MyPeerConnectionObserver(){
            @Override
            public void onAddStream(MediaStream mediaStream) {
                super.onAddStream(mediaStream);

                try {
                    if (!mediaStream.videoTracks.isEmpty()) {
                        remoteVideoTrack = mediaStream.videoTracks.get(0);

                        if (remoteView != null) {
                            remoteVideoTrack.addSink(remoteView);
                        }

                        // 🔔 Notify UI layer
                        if (streamReadyListener != null) {
                            streamReadyListener.onRemoteStreamReady();
                        }
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }

            }


            @Override
            public void onConnectionChange(PeerConnection.PeerConnectionState newState) {
                Log.d("TAG", "onConnectionChange: "+newState);
                super.onConnectionChange(newState);
                if (newState == PeerConnection.PeerConnectionState.CONNECTED && listener!=null){
                    callState = CallState.CONNECTED;
                    listener.webrtcConnected();
                }

                if (newState == PeerConnection.PeerConnectionState.CLOSED ||
                        newState == PeerConnection.PeerConnectionState.DISCONNECTED ){
                    callState = CallState.IDLE;
                    if (listener!=null){
                        listener.webrtcClosed();
                    }
                }
            }
            //  When Ice Server  is setted to WebRTC Client the Ice is transmitted to another Peer
            @Override
            public void onIceCandidate(IceCandidate iceCandidate) {
                super.onIceCandidate(iceCandidate);
                // Not necessary
                //webRTCClient.sendIceCandidate(iceCandidate,target);
            }
        },currentUsername);
        webRTCClient.listener = this;
        callBack.onSuccess();

    }

    public void initLocalView(SurfaceViewRenderer view){
        webRTCClient.initLocalSurfaceView(view);
    }


    public void onDestroyHomeView() {
        if (remoteVideoTrack != null && remoteView != null) {
            remoteVideoTrack.removeSink(remoteView);
        }

//        if (remoteView != null) {
//            remoteView.release();
//            remoteView = null;
//        }
    }






    public void initRemoteView(SurfaceViewRenderer view){
//        if (!isRemoteViewInitialized) {
        if (view!=null) {
            webRTCClient.initRemoteSurfaceView(view);
            this.remoteView = view;
        }
//            isRemoteViewInitialized = true;
//        }


        if (remoteVideoTrack != null) {
            if (remoteView == null) return;
            remoteVideoTrack.addSink(remoteView);
        }
    }

    public void startCall(String target){
        webRTCClient.call(target);
    }

    public void switchCamera() {
        webRTCClient.switchCamera();
    }

    public void toggleAudio(Boolean shouldBeMuted){
        webRTCClient.toggleAudio(shouldBeMuted);
    }


//    public void closeVideo(){
//        myWebSocketClient.sendMessageToOtherUser(
//                new DataModel(target, currentUsername, null, DataModelType.RTC_ENDCALL)
//        );
//        // Close WebRTC Connection
//        endCall();
//        }


    public void sendCallRequest(String target){
        if (callState != CallState.IDLE) {
            Log.w("WebRTC", "Call already active, ignoring STARTCALL");
            return;
        }
        callState = CallState.CALLING;
        myWebSocketClient.sendMessageToOtherUser(
                new DataModel(target, currentUsername, null, DataModelType.RTC_STARTCALL)
        );
    }

    public void endCall(){
        myWebSocketClient.sendMessageToOtherUser(
                new DataModel(target, currentUsername, null, DataModelType.RTC_ENDCALL)
        );


        if (webRTCClient != null) {
            // 2. Call the internal cleanup method.
            webRTCClient.closeConnection();

            // 3. Remove the reference to the object.
            // This makes it eligible for garbage collection.
            //webRTCClient = null;

            //onDestroyHomeView();


//            if (remoteView != null) {
//                remoteView.release();
//            }

            Log.d("MainRepository", "WebRTCClient has been closed and nulled.");
        }




    }


    // If Android get Offer from Raspberry
    public void processOffer(DataModel model){
        try{
            this.target = model.getSender();
            webRTCClient.onRemoteSessionReceived(new SessionDescription(
                    SessionDescription.Type.OFFER, model.getData()
            ));
            //webRTCClient.answer(model.getSender());
        }catch (Exception e){
            e.printStackTrace();
        }
    }


    // If Android get IceCandidate from Raspberry
    public void processIceCandidate(DataModel model) {
        try {
            IceCandidate candidate = gson.fromJson(model.getData(), IceCandidate.class);
            webRTCClient.addIceCandidate(candidate);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onTransferDataToOtherPeer(DataModel model) {
        myWebSocketClient.sendMessageToOtherUser(model);
    }

    public interface Listener{
        void webrtcConnected();
        void webrtcClosed();
    }
}