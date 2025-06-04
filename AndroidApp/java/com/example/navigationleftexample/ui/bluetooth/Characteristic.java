/*
 * Copyright 2025 Evgheni Jaruc
 */


package com.example.navigationleftexample.ui.bluetooth;

import android.os.Build;
import android.os.Parcel;
import android.os.Parcelable;

import androidx.annotation.NonNull;

import java.util.UUID;

public class Characteristic {

    public UUID uuidChar = null; //  Characteristic UUID
    public byte[] valueChar = null;  //  Value of characteristic
    public BluetoothLeService.CharType typeChar = null;   // false =

    public Characteristic(UUID uuid, byte[] value, BluetoothLeService.CharType type) {
        uuidChar = uuid;
        valueChar = value;
        typeChar = type;

    }

//    protected Characteristic(Parcel in) {
//        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//            valueChar = in.readBlob();
//        }
//    }
//
//    public final Creator<Characteristic> CREATOR = new Creator<Characteristic>() {
//        @Override
//        public Characteristic createFromParcel(Parcel in) {
//            return new Characteristic(in);
//        }
//
//        @Override
//        public Characteristic[] newArray(int size) {
//            return new Characteristic[size];
//        }
//    };
//
//    @Override
//    public int describeContents() {
//        return 0;
//    }
//
//    @Override
//    public void writeToParcel(@NonNull Parcel dest, int flags) {
//        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//            dest.writeBlob(valueChar);
//        }
//    }
}
