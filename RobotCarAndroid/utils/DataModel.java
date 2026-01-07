package com.example.navigationleftexample.utils;

import android.util.Log;

public class DataModel {

    private String target;
    private String sender;

    private String data;

    private DataModelType type;


    public DataModel(String target, String sender, String data, DataModelType type) {
        this.target = target;
        this.sender = sender;
        this.data = data;
        this.type = type;
    }

    public DataModel() {
        this.target = null;
        this.sender = null;
        this.type = null;
        this.data = null;
    }


    /**
     * Resets the model to its default, pre-call state.
     * This is the most important method for your reconnection logic.
     */
    public void clear() {
        this.target = null;
        this.sender = null;
        this.type = null;
        this.data = null;
        Log.d("DataModel", "Model has been cleared and reset to IDLE state.");
    }

    public String getTarget() {
        return target;
    }

    public String getSender() {
        return sender;
    }

    public String getData() {
        return data;
    }

    public DataModelType getType() {
        return type;
    }

    public void setTarget(String target) {
        this.target = target;
    }

    public void setSender(String sender) {
        this.sender = sender;
    }

    public void setData(String data) {
        this.data = data;
    }

    public void setType(DataModelType type) {
        this.type = type;
    }
}
