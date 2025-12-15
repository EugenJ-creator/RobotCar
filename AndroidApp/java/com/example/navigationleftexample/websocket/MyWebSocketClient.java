package com.example.navigationleftexample.websocket;

import android.util.Log;

import com.google.firebase.crashlytics.buildtools.reloc.com.google.common.reflect.TypeToken;
import com.google.gson.Gson;

import java.lang.reflect.Type;
import java.net.URI;
import java.util.Map;

import dev.gustavoavila.websocketclient.WebSocketClient;

public class MyWebSocketClient extends WebSocketClient {


    private static final String TAG = "MyWebSocket";
    private final Gson gson = new Gson();

    private  MessageListener listener;


    public MyWebSocketClient(URI serverUri) {
        super(serverUri);
    }




    public void setMessageListener(MessageListener listener) {
        this.listener = listener;
    }





    @Override
    public void onOpen() {
        Log.i("WebSocket", "Session is starting");

    }

    @Override
    public void onTextReceived(String message) {
        Log.i(TAG, "Message: " + message);

        try {
            Type mapType = new TypeToken<Map<String, Object>>() {}.getType();
            Map<String, Object> json = gson.fromJson(message, mapType);

            if (listener != null) {
                listener.onJsonReceived(json);
            }
        } catch (Exception e) {
            Log.e(TAG, "JSON parse error: " + e.getMessage());
        }
    }






    @Override
    public void onBinaryReceived(byte[] data) {
    }

    @Override
    public void onPingReceived(byte[] data) {
    }

    @Override
    public void onPongReceived(byte[] data) {
    }

    @Override
    public void onException(Exception e) {
        Log.e("WebSocket", e.getMessage());
    }

    @Override
    public void onCloseReceived(int i, String s) {

    }

    public void onCloseReceived() {
        Log.i("WebSocket", "Closed ");
    }
};