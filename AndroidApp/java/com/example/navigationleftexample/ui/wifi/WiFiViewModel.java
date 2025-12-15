package com.example.navigationleftexample.ui.wifi;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

public class WiFiViewModel extends ViewModel {

    private final MutableLiveData<String> mText;

    public WiFiViewModel() {
        mText = new MutableLiveData<>();
        mText.setValue("This is wifi fragment");
    }

    public LiveData<String> getText() {
        return mText;
    }
}