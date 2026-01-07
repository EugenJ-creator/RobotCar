package com.example.navigationleftexample.websocket;

public interface ConnectionListener {
    void onWebSocketOpen();
    void onWebSocketClose();
    void onWebSocketError(Exception e);
}
