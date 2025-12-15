package com.example.navigationleftexample.ui.wifi;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import com.example.navigationleftexample.databinding.FragmentWifiBinding;
import com.example.navigationleftexample.websocket.MyWebSocketClient;
import com.example.navigationleftexample.websocket.WebSocketManager;

import java.net.URI;
import java.net.URISyntaxException;

public class WiFiFragment extends Fragment {

    public MyWebSocketClient myWebSocketClient;
    private FragmentWifiBinding binding;

    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {
        WiFiViewModel wiFiViewModel =
                new ViewModelProvider(this).get(WiFiViewModel.class);

        binding = FragmentWifiBinding.inflate(inflater, container, false);
        View root = binding.getRoot();

        final TextView textView = binding.textSlideshow;
        wiFiViewModel.getText().observe(getViewLifecycleOwner(), textView::setText);



        //  TODO  URI must be loaded from a Data Bank in the future
        //  Create URI of the Web Socket Server (Raspberry)
        URI uriRaspWebSocket;
        try {
            // Connect to local host
            uriRaspWebSocket = new URI("ws://192.168.178.24:5000/ws");
        }
        catch (URISyntaxException e) {
            e.printStackTrace();
            return root;
        }

        myWebSocketClient = WebSocketManager.getInstance(uriRaspWebSocket).getWebSocket();















        return root;
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }
}