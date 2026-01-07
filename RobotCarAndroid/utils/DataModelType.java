package com.example.navigationleftexample.utils;


import com.fasterxml.jackson.annotation.JsonValue;
import com.google.gson.annotations.SerializedName;

import org.webrtc.IceCandidate;


import com.google.gson.annotations.SerializedName;
public enum DataModelType {

    @SerializedName("RTC.Offer")
    RTC_OFFER,

    @SerializedName("RTC.IceCandidate")
    RTC_ICECANDIDATE,

    @SerializedName("RTC.Answer")
    RTC_ANSWER,

    @SerializedName("RTC.StartCall")
    RTC_STARTCALL,

    @SerializedName("RTC.EndCall")
    RTC_ENDCALL,

    @SerializedName("Control.StearCamera")
    CONTROL_STEARCAMERA,

    @SerializedName("Radar.Coordinates")
    RADAR_COORDINATES







//    RTC_OFFER("RTC.Offer"),
//    RTC_ICECANDIDATE("RTC.IceCandidate"),
//    RTC_ANSWER("RTC.Answer"),
//
//    RTC_STARTCALL("RTC.StartCall"),
//
//    RTC_ENDCALL("RTC.EndCall"),
//
//    CONTROL_STEARCAMERA("Control.StearCamera"),
//
//    RADAR_COORDINATES("Radar.Coordinates");
//
//
//    private String name;
//    DataModelType(String s) {
//        this.name = s;
//    }
//
//
//
//
//    // standard constructors
//
//    @JsonValue
//    public String getName() {
//        return name;
//    }

}
