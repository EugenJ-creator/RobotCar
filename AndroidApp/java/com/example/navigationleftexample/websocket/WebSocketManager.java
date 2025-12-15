package com.example.navigationleftexample.websocket;

import java.net.URI;

public class WebSocketManager {

    private static WebSocketManager instance;
    private MyWebSocketClient webSocket;

    private URI uriRaspWebSocket;



    private WebSocketManager(URI uriRaspWebSocket) {

        webSocket = new MyWebSocketClient(uriRaspWebSocket);
        webSocket.setConnectTimeout(10000);
        webSocket.setReadTimeout(60000);
        webSocket.enableAutomaticReconnection(5000);
        webSocket.connect();
    }

    public static synchronized WebSocketManager getInstance(URI uriRaspWebSocket) {
        if (instance == null) {
            instance = new WebSocketManager(uriRaspWebSocket);
        }
        return instance;
    }

    public MyWebSocketClient getWebSocket() {
        return webSocket;
    }
}
