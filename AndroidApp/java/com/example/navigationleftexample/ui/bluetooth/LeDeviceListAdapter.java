
/*
 * Copyright 2025 Evgheni Jaruc
 */


package com.example.navigationleftexample.ui.bluetooth;

import android.Manifest;
import android.bluetooth.BluetoothDevice;
import android.content.pm.PackageManager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import androidx.core.app.ActivityCompat;

import com.example.navigationleftexample.R;

import java.util.ArrayList;

// Adapter for holding devices found through scanning.
public class LeDeviceListAdapter extends BaseAdapter {
    private ArrayList<BluetoothDevice> mLeDevices;
    private LayoutInflater mInflator;



    public LeDeviceListAdapter(LayoutInflater inflater) {
        super();
        mLeDevices = new ArrayList<BluetoothDevice>();
        mInflator = inflater;
    }

    public void addDevice(BluetoothDevice device) {
        if (!mLeDevices.contains(device)) {
            mLeDevices.add(device);
        }
    }



    public void removeDevice(BluetoothDevice device) {
        if (mLeDevices.contains(device)) {
            mLeDevices.remove(device);
        }
    }






    public BluetoothDevice getDevice(int position) {
        return mLeDevices.get(position);
    }

    public void clear() {
        mLeDevices.clear();
    }



    @Override
    public int getCount() {
        return mLeDevices.size();
    }

    @Override
    public Object getItem(int i) {
        return mLeDevices.get(i);
    }

    @Override
    public long getItemId(int i) {
        return i;
    }

    @Override
    public View getView(int i, View view, ViewGroup viewGroup) {
        ViewHolder viewHolder;
        // General ListView optimization code.
        if (view == null) {
            view = mInflator.inflate(R.layout.item_view, null);
            viewHolder = new ViewHolder();

            viewHolder.deviceName = (TextView) view.findViewById(R.id.itemTextView);

            view.setTag(viewHolder);
        } else {
            viewHolder = (ViewHolder) view.getTag();
        }
        BluetoothDevice device = mLeDevices.get(i);
        if (ActivityCompat.checkSelfPermission(mInflator.getContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
            // TODO: Consider calling
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.
//            return ;
        }


        final String deviceName = device.getName();


        if (deviceName != null && deviceName.length() > 0) {
            viewHolder.deviceName.setText(deviceName);
            //viewHolder.deviceAddress.setText(device.getAddress());
        }
        else {
            viewHolder.deviceName.setText(R.string.unknown_device);
            //viewHolder.deviceAddress.setText(device.getAddress());
        }
        return view;
    }
}

 class ViewHolder {
    TextView deviceName;
    //TextView deviceAddress;
}

