/*
 * Copyright 2025 Evgheni Jaruc
 */

package com.example.navigationleftexample.ui.bluetooth;

import android.bluetooth.BluetoothDevice;
import android.content.ServiceConnection;
import android.os.Parcel;
import android.os.Parcelable;
import android.widget.Switch;

import androidx.annotation.NonNull;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.SavedStateHandle;
import androidx.lifecycle.ViewModel;

public class BluetoothViewModel extends ViewModel implements Parcelable {

   // private SavedStateHandle savedStateHandle;


    private MutableLiveData<ServiceConnection> lastServiceConnection;

    private  MutableLiveData<BluetoothDevice> selectedDevice;

    private MutableLiveData<Switch> switchButton;

    private MutableLiveData<Float> tempSensor;
    private MutableLiveData<Float> humiditySensor;

    private MutableLiveData<Float> speedSensor;

    private MutableLiveData<Boolean> bluetoothActive;

    public BluetoothViewModel() {

        tempSensor = new MutableLiveData<>();
        humiditySensor = new MutableLiveData<>();
        speedSensor = new MutableLiveData<>();
        selectedDevice = new MutableLiveData<BluetoothDevice>();
        switchButton = new MutableLiveData<Switch>();
        bluetoothActive = new MutableLiveData<>();
        lastServiceConnection = new MutableLiveData<ServiceConnection>();
        bluetoothActive.setValue(false);
    }

    public LiveData<Boolean> getBluetoothActive() {
        return bluetoothActive;
    }

    public void setBluetoothActive(Boolean state){
        bluetoothActive.setValue(state);
    }

    public LiveData<ServiceConnection> getLastServiceConnection() {
        return lastServiceConnection;
    }

    public void setLastServiceConnection(ServiceConnection conn){
        lastServiceConnection.setValue(conn);
    }






    public LiveData<Float> getTempSensor() {
        return tempSensor;
    }


    public LiveData<Float> getHumiditySensor() {
        return humiditySensor;
    }
    public LiveData<Float> getSpeedSensor() {
        return speedSensor;
    }


    public void setTempSensor(float temp){
        tempSensor.setValue(temp);
    }

    public void setHumiditySensor(float humidity){
        humiditySensor.setValue(humidity);
    }


    public void setSpeedSensor(float speed){
        speedSensor.setValue(speed);
    }


//    public void setPairedDeviceToSavedStateHandle(){
//
//        savedStateHandle.set("pairedDevice", selectedDevice);
//    }


    protected BluetoothViewModel(Parcel in) {
    }

    public static final Creator<BluetoothViewModel> CREATOR = new Creator<BluetoothViewModel>() {
        @Override
        public BluetoothViewModel createFromParcel(Parcel in) {
            return new BluetoothViewModel(in);
        }

        @Override
        public BluetoothViewModel[] newArray(int size) {
            return new BluetoothViewModel[size];
        }
    };

    public int setSwitch(Switch switchB) {
        switchButton.setValue(switchB);
        return 0;
    }


    public LiveData<Switch> getSwitch() {
        return switchButton;
    }


    public LiveData<BluetoothDevice> getDevice() {
        return selectedDevice;
    }


    public int setDevice(BluetoothDevice dev) {
        selectedDevice.setValue(dev);
        return 0;
    }


    public int deleteDevice() {
        selectedDevice = null;
        return 0;
    }


    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(@NonNull Parcel dest, int flags) {

    }
}