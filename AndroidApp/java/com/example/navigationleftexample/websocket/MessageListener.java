package com.example.navigationleftexample.websocket;

import java.util.Map;

public interface MessageListener {
    void onJsonReceived(Map<String, Object> json);
}
