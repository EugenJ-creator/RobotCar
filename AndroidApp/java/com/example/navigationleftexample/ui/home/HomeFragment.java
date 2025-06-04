
/*
 * Copyright 2025 Evgheni Jaruc
 */


package com.example.navigationleftexample.ui.home;

import android.Manifest;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.media.MediaPlayer;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Vibrator;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;
import androidx.core.app.ActivityCompat;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModel;
import androidx.lifecycle.ViewModelProvider;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;
import com.example.navigationleftexample.ui.bluetooth.Characteristic;

import com.example.navigationleftexample.R;
import com.example.navigationleftexample.ui.ViewModels.BluetoothGameDataViewModel;
import com.example.navigationleftexample.ui.bluetooth.BluetoothViewModel;
import com.example.navigationleftexample.ui.circularseekbar.CircularSeekBar;
import com.example.navigationleftexample.databinding.FragmentHomeBinding;
import com.example.navigationleftexample.ui.bluetooth.BluetoothLeService;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

public class HomeFragment extends Fragment {

//    Intent gattServiceIntent;
//    BluetoothGatt bluetoothGatt;
    private SeekBar speedcarSeekBar;
    private SeekBar speedcarBackSeekBar;

    private CircularSeekBar circularSeekBarStearing;

    private ImageButton leftButton;
    private ImageButton rightButton;
    private ImageButton thrrottleButton;
    private ImageButton warningButton;


    private ImageButton onButton;
    private ImageButton cruiseButton;
    private ImageButton signalButton;
    private ImageButton autoLight;
    private ImageButton highBeam;
    private ImageButton parkingLight;

    private ImageView compassNorthDirection;

    private ImageView leftBlinker;
    private ImageView rightBlinker;
    private static final String TAG = "HomeFragment ";

    private Handler handler = new Handler();
    public HomeViewModel homeViewModel;
//    private TextView tempView;
//    private TextView humidityView;

    private final double[] bias = {-544.48,239.4,55.94};
    private final double[][] euler = {{1.225434,-0.028676,0.069206},{-0.028676,1.087208,0.045837},{0.069206,0.045837,1.057089}};
    private static double offsetAngle = 74.745;  // Offset degrees for kompass
    private Double X_Magnetometer_Data;
    private Double Y_Magnetometer_Data;
    private Double Z_Magnetometer_Data;
    // Stearing iteration
    private static final long ITERATION_PERIOD_MOTOR_POWER = 1;
    private static final long ITERATION_PERIOD_STEAR_ANGLE = 5;
    private static final long ITERATION_PERIOD_LEFT_RIGHT_BLINKING = 950;     //    blinking period of led /2

    private static final long DELAY_PRESS_POWER= 300;

    private final static int STERAANGLE_MAX = 140;
    private final static int STERAANGLE_MIN = 0;
    private final static int STERAANGLE_MIDDLE = 49;
    private final static int BUZZER_MIDLE = 20;  //200
    private final static int BUZZER_OFF = 0;

    public static int optionsToggle = 0;
    public static int warningBlinkingActive = 0;
    public static int buzzerVolume = BUZZER_MIDLE;
    public static int stearEngle =  STERAANGLE_MIDDLE;
    public static int throttleProgress;
    public static int throttleBackProgress;
    public static int direction = 0;

//    public static int readDone = 0;  //    If the last read was done

    public static byte optionsLastState = 0; //     For comparing with the new one revceived from OnWrite Function

    public int StearingDirection = 0;    //   Indicating the direction of stearing to blink leds
    private int LastSpeedState = 0;   //   Last Speed state. o or some value

    private final static int MOTOR_POWER_MAX = 250;
    private final static int MOTOR_POWER_MIN = 0;
    public static int motorPower = MOTOR_POWER_MIN;

    private int stearingPressingAvailable = 1;
    private int powerPressingAvailable = 1;

    private FragmentHomeBinding binding;
    private  BluetoothLeService bluetoothService;

    MediaPlayer mediaPlayerOnBlinking;
    MediaPlayer mediaPlayerOffBlinking;
    UpdateLeftButtonThread myUpdateLeftButtonThread = null;
    UpdateRightButtonThread myUpdateRightButtonThread = null;
//    UpdateThrottleGasThread myUpdateThrottleGasThread = null;
//    UpdateBrakeThread myUpdateBrakeThread = null;

    AutoBrakeThread  autoBrakeThread = null;
    AutoBrakeBackThread  autoBrakeBackThread = null;
    AutoStearThread  autoStearThread = null;

    BlinkingTimerLeftThread blinkingTimerLeftThread = null;
    BlinkingTimerRightThread blinkingTimerRightThread = null;

    BlinkingTimerWarningThread blinkingTimerWarningThread = null;

    BluetoothViewModel bluetoothViewModel;

    //BluetoothDataReceiver   bluetoothDataReceiver;
    Vibrator vibe = null;
    int  signal_on_off = 0;   // Buzzer button state
    int  cruise_on_off = 0;  //   state
    int  warningLight_on_off = 0;   // state
    int  lightAuto_on_off = 0;   //  state
    int  hightBeam_on_off = 0;   //  state
    int  parkingLight_on_off = 0;   // state
    int  startButton_on_off = 0;   //  state
    public static double[] multiplyMatrix(double[][] matrix, double[] vector) {
        double[] res = new double[vector.length];

        for (int i = 0; i < matrix[0].length; i++) {
            int sum = 0;
            for (int j = 0; j < vector.length; j++) {
                sum += matrix[j][i] * vector[j];
            }
            res[i] = sum; //this should help you assign the values
        }
        return res;
    }


    public static double[] substractVectors(double[] vector1, double[] vector2) {
        double[] res = new double[vector1.length];
        for (int i = 0; i < vector1.length; i++) {
            res[i] = vector1[i] - vector2[i];
        }
        return res;
    }

    private ServiceConnection serviceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            bluetoothService = ((BluetoothLeService.LocalBinder) service).getService();
            if (bluetoothService == null) {
                Log.e(TAG, "Unable to initialize Bluetooth");
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            bluetoothService = null;
        }
    };

    public class BlinkingTimerWarningThread extends Thread {

        private boolean running = false;
        private boolean status = false;
        public void setRunning(boolean running) {
            this.running = running;
        }

        public void toggleThread() {
            this.running = !this.running;
        }

        public void run() {
            running = true;
            warningBlinkingActive = 1;
            try {
                while(!Thread.currentThread().isInterrupted() ) {
                    if (status == false){
                        warningButton.setImageResource(R.drawable.dashboard_warning_off);
                        leftBlinker.setImageResource(R.drawable.dashboard_left_off);
                        rightBlinker.setImageResource(R.drawable.dashboard_right_off);
                        mediaPlayerOffBlinking.start();
                        status = true;

                    } else if (status == true){
                        warningButton.setImageResource(R.drawable.dashboard_warning_on);
                        leftBlinker.setImageResource(R.drawable.dashboard_left_on);
                        rightBlinker.setImageResource(R.drawable.dashboard_right_on);
                        mediaPlayerOnBlinking.start();
                        status = false;
                    }
                    Thread.sleep(ITERATION_PERIOD_LEFT_RIGHT_BLINKING);
                }
                return;
            }  catch
            (InterruptedException e) {
                warningBlinkingActive = 0;
                leftBlinker.setImageResource(R.drawable.dashboard_left_off);
                rightBlinker.setImageResource(R.drawable.dashboard_right_off);
                Log.e(TAG, e.toString());
                //throw new RuntimeException(e);

            }
        }

    }


    public class BlinkingTimerLeftThread extends Thread {

        private boolean running = false;
        private boolean status = false;
        public void setRunning(boolean running) {
            this.running = running;
        }

        public void toggleThread() {
            this.running = !this.running;
        }

        public void run() {
            running = true;

            try {
                while(!Thread.currentThread().isInterrupted() ) {
                    if (status == false){
                        if (warningBlinkingActive != 1) {
                            leftBlinker.setImageResource(R.drawable.dashboard_left_on);
                            mediaPlayerOnBlinking.start();
                        }
                        status = true;

                    } else if (status == true){
                        if (warningBlinkingActive != 1) {
                            leftBlinker.setImageResource(R.drawable.dashboard_left_off);
                            mediaPlayerOffBlinking.start();
                        }
                        status = false;
                    }
                Thread.sleep(ITERATION_PERIOD_LEFT_RIGHT_BLINKING);
                }
                return;
            }  catch
            (InterruptedException e) {
                Log.e(TAG, e.toString());
                //throw new RuntimeException(e);

            }
        }
    }


    public class BlinkingTimerRightThread extends Thread {

        private boolean running = false;
        private boolean status = false;
        public void setRunning(boolean running) {
            this.running = running;
        }

        public void toggleThread() {
            this.running = !this.running;
        }

        public void run() {
            running = true;

            try {
                while(!Thread.currentThread().isInterrupted() ) {
                    if (status == false){
                        if (warningBlinkingActive != 1) {
                            rightBlinker.setImageResource(R.drawable.dashboard_right_on);
                            mediaPlayerOnBlinking.start();
                        }
                        status = true;

                    } else if (status == true){
                        if (warningBlinkingActive != 1) {
                            rightBlinker.setImageResource(R.drawable.dashboard_right_off);
                            mediaPlayerOffBlinking.start();
                        }
                        status = false;
                    }
                    Thread.sleep(ITERATION_PERIOD_LEFT_RIGHT_BLINKING);
                }
                return;
            }  catch
            (InterruptedException e) {
                Log.e(TAG, e.toString());
                //throw new RuntimeException(e);

            }
        }
    }


    public class UpdateLeftButtonThread extends Thread {

        private boolean keepRunning = false;
        private boolean releaseButton = false;

        public void toggleThread() {
            this.keepRunning = !this.keepRunning;
        }

        public void run() {

//            while (stearEngle <= STERAANGLE_MAX && !releaseButton) {
//                stearEngle++;
//                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                    byte[] val =  new byte[1];
//                    val[0] = (byte )stearEngle;
//                    sendCharacteristic(val, BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID);
//                }
//
//
//
//            }
//            while (stearEngle >= STERAANGLE_MIDDLE && releaseButton) {
//                stearEngle--;
//                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                    byte[] val =  new byte[1];
//                    val[0] = (byte )stearEngle;
//                    sendCharacteristic(val, BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID);
//                }
//
//            }


            if (!releaseButton) {
                byte[] val = new byte[1];
                val[0] = (byte) STERAANGLE_MAX;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                    BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                    //sendCharacteristic(val, BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID);
                }
            }
        }
    }





    public class UpdateRightButtonThread extends Thread {

        private boolean running = false;


        public void toggleThread() {
            this.running = !this.running;
        }

        public void setRunning(boolean running) {
            this.running = running;
        }

        public void run() {


            while (stearEngle >= STERAANGLE_MIN) {
                stearEngle--;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                    byte[] val =  new byte[1];
                    val[0] = (byte )stearEngle;
                    BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                    //sendCharacteristic(val, BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID);
                }
                try {
                    Thread.sleep(ITERATION_PERIOD_MOTOR_POWER);
                } catch (InterruptedException e) {
                    Log.e(TAG, e.toString());
                    throw new RuntimeException(e);

                }


            }

        }
    }




//    public class UpdateThrottleGasThread extends Thread {
//
//        private boolean running = false;
//
//        public void setRunning(boolean running) {
//            this.running = running;
//        }
//
//        public void toggleThread() {
//            this.running = !this.running;
//        }
//
//        public void run() {
//            running = true;
//
//            try {
//                while(!Thread.currentThread().isInterrupted()) {
//
//                    while ((motorPower <= MOTOR_POWER_MAX) && running) {
//                        motorPower++;
//
//                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                            byte[] val = new byte[1];
//                            val[0] = (byte) motorPower;
//                            sendCharacteristic(val, BluetoothLeService.CAR_SPEED_CHARACTERISTIC_UUID);
//                        }
//
//                        Thread.sleep(ITERATION_PERIOD_MOTOR_POWER);
//                    }
//                    return;
//                }
//                return;
//            }  catch
//              (InterruptedException e) {
//                    Log.e(TAG, e.toString());
//                    //throw new RuntimeException(e);
//
//            }
//
//
//            }
//    }

    public class AutoBrakeThread extends Thread {

        private boolean running = false;

        public void setRunning(boolean running) {
            this.running = running;
        }

        public void toggleThread() {
            this.running = !this.running;
        }

        public void run() {
            running = true;

            try {
                while(!Thread.currentThread().isInterrupted() ) {

                    if ((throttleProgress > 0) && running) {
                        while (throttleProgress != 0) {
                            throttleProgress--;
                            speedcarSeekBar.setProgress(throttleProgress);

//                            byte[] val = new byte[1];
//                            val[0] = (byte) throttleProgress;
//                            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                                sendCharacteristic(val, BluetoothLeService.CAR_SPEED_CHARACTERISTIC_UUID);
//                            }
                            Thread.sleep(ITERATION_PERIOD_MOTOR_POWER);
                        }

                    } else if ((throttleProgress < 0) && running){

                        while (throttleProgress != 0) {
                            throttleProgress++;
                            speedcarSeekBar.setProgress(throttleProgress);
//                            int posThrottleProgress;
//
//                            if (throttleProgress < 0) {
//                                posThrottleProgress = (-1) * throttleProgress;
//                            } else
//                                posThrottleProgress = throttleProgress;
//
//                            byte[] val = new byte[1];
//                            val[0] = (byte) posThrottleProgress;
//                            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                                sendCharacteristic(val, BluetoothLeService.CAR_SPEED_CHARACTERISTIC_UUID);
//                            }
                            Thread.sleep(ITERATION_PERIOD_MOTOR_POWER);
                        }

                    }

                    return;
                }
                return;
            }  catch
            (InterruptedException e) {
                Log.e(TAG, e.toString());
                //throw new RuntimeException(e);

            }


        }
    }


    public class AutoBrakeBackThread extends Thread {

        private boolean running = false;

        public void setRunning(boolean running) {
            this.running = running;
        }

        public void toggleThread() {
            this.running = !this.running;
        }

        public void run() {
            running = true;

            try {
                while(!Thread.currentThread().isInterrupted() ) {

                    if ((throttleBackProgress > 0) && running) {
                        while (throttleBackProgress != 0) {
                            throttleBackProgress--;
                            speedcarBackSeekBar.setProgress(throttleBackProgress);

//                            byte[] val = new byte[1];
//                            val[0] = (byte) throttleProgress;
//                            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                                sendCharacteristic(val, BluetoothLeService.CAR_SPEED_CHARACTERISTIC_UUID);
//                            }
                            Thread.sleep(ITERATION_PERIOD_MOTOR_POWER);
                        }

                    } else if ((throttleBackProgress < 0) && running){

                        while (throttleBackProgress != 0) {
                            throttleBackProgress++;
                            speedcarBackSeekBar.setProgress(throttleBackProgress);
//                            int posThrottleProgress;
//
//                            if (throttleProgress < 0) {
//                                posThrottleProgress = (-1) * throttleProgress;
//                            } else
//                                posThrottleProgress = throttleProgress;
//
//                            byte[] val = new byte[1];
//                            val[0] = (byte) posThrottleProgress;
//                            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                                sendCharacteristic(val, BluetoothLeService.CAR_SPEED_CHARACTERISTIC_UUID);
//                            }
                            Thread.sleep(ITERATION_PERIOD_MOTOR_POWER);
                        }

                    }

                    return;
                }
                return;
            }  catch
            (InterruptedException e) {
                Log.e(TAG, e.toString());
                //throw new RuntimeException(e);

            }


        }
    }









    public class AutoStearThread extends Thread {

        private boolean running = false;

        public void setRunning(boolean running) {
            this.running = running;
        }

        public void toggleThread() {
            this.running = !this.running;
        }

        public void run() {
            running = true;

            try {
                while(!Thread.currentThread().isInterrupted() ) {

                    if ((stearEngle > STERAANGLE_MIDDLE) && running) {
                        while (stearEngle != STERAANGLE_MIDDLE) {
                            stearEngle--;
                            circularSeekBarStearing.setProgress(stearEngle);

//                            byte[] val = new byte[1];
//                            val[0] = (byte) throttleProgress;
//                            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                                sendCharacteristic(val, BluetoothLeService.CAR_SPEED_CHARACTERISTIC_UUID);
//                            }
                            Thread.sleep(ITERATION_PERIOD_STEAR_ANGLE);
                        }

                    } else if ((stearEngle < STERAANGLE_MIDDLE) && running){

                        while (stearEngle != STERAANGLE_MIDDLE) {
                            stearEngle++;
                            circularSeekBarStearing.setProgress(stearEngle);
//                            int posThrottleProgress;
//
//                            if (throttleProgress < 0) {
//                                posThrottleProgress = (-1) * throttleProgress;
//                            } else
//                                posThrottleProgress = throttleProgress;
//
//                            byte[] val = new byte[1];
//                            val[0] = (byte) posThrottleProgress;
//                            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                                sendCharacteristic(val, BluetoothLeService.CAR_SPEED_CHARACTERISTIC_UUID);
//                            }
                            Thread.sleep(ITERATION_PERIOD_STEAR_ANGLE);
                        }

                    }

                    return;
                }
                return;
            }  catch
            (InterruptedException e) {
                Log.e(TAG, e.toString());
                //throw new RuntimeException(e);

            }


        }
    }




//
//    public class UpdateBrakeThread extends Thread {
//
//        private boolean running = false;
//
//        public void setRunning(boolean running) {
//            this.running = running;
//        }
//
//        public void toggleThread() {
//            this.running = !this.running;
//        }
//
//        public void run() {
//            running = true;
//
//            try {
//                while(!Thread.currentThread().isInterrupted()) {
//
//                    while ((motorPower >= MOTOR_POWER_MIN) && running) {
//                        motorPower--;
//                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                            byte[] val = new byte[1];
//                            val[0] = (byte) motorPower;
//                            sendCharacteristic(val, BluetoothLeService.CAR_SPEED_CHARACTERISTIC_UUID);
//                        }
//
//                        Thread.sleep(ITERATION_PERIOD_MOTOR_POWER);
//                    }
//                    return;
//                }
//                return;
//            }  catch
//            (InterruptedException e) {
//                //throw new RuntimeException(e);
//            }
//
//
//        }
//    }



    // Broadcast Receiver
    // Don't forget to add new notification to filter!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    BroadcastReceiver notificationReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent != null && intent.getAction() != null) {
                if (intent.getAction().equals(BluetoothLeService.ACTION_NOTIFICATION_RECEIVED_TRMP_HUMIDITY)) {
                    //str = intent.getFloatExtra("title", 66);
                    //  Show Connected Bluetooth Device
//Data received via Bluetooth is  Little Endian
                    Float tempData;
                    Float humidityData;
                    byte[] tempHumidityNotificationData = intent.getByteArrayExtra("tempHumidityData");

                    ////Calculate humidity, byte1, byte2, 4bits MSB from byte 3
                    long h = tempHumidityNotificationData[4] & 0xFF;
                    h <<= 8;
                    h |= tempHumidityNotificationData[3] & 0xFF;
                    h <<= 4;
                    h |= (tempHumidityNotificationData[2] >> 4) & 0x0F;
                    //humidityString = Long.toHexString(h);
                    humidityData = ((float) h * 100) / 0x100000;


                    //Calculate temp , 4bits LSB from byte 3, byte4, byte5
                    long tdata = (tempHumidityNotificationData[2] & 0x0F);
                    tdata <<= 8;
                    tdata |= tempHumidityNotificationData[1] & 0xFF;
                    tdata <<= 8;
                    tdata |= tempHumidityNotificationData[0] & 0xFF;
                    //tempString = Long.toHexString(tdata);
                    tempData = ((float) tdata * 200 / 0x100000) - 50;

//                    bluetoothViewModel = new ViewModelProvider(requireActivity()).get(BluetoothViewModel.class);
                    binding.setBluetoothViewModelData(bluetoothViewModel);

                    bluetoothViewModel.setTempSensor(tempData);
                    bluetoothViewModel.setHumiditySensor(humidityData);
//        // Show bluetooth device name in view
//        bluetoothViewModel.getTempSensor().observe(getViewLifecycleOwner(), tempSensor -> {
//        tempView.setText(tempSensor.intValue());
//        });
//
//        bluetoothViewModel.getHumiditySensor().observe(getViewLifecycleOwner(), humiditySensor -> {
//        humidityView.setText(humiditySensor.intValue());
//        });



                } else if (intent.getAction().equals(BluetoothLeService.ACTION_NOTIFICATION_RECEIVED_BUZZER_VALUE)) {       //  Buzzer
                    byte[] buzzerNotificationData = intent.getByteArrayExtra("buzzerData");
                    if (buzzerNotificationData[0] == 0) {
                        signalButton.setImageResource(R.drawable.dashboard_signal_off);
                        signal_on_off = 0;
                    } else if (buzzerNotificationData[0] == BUZZER_MIDLE) {
                        signalButton.setImageResource(R.drawable.dashboard_signal_on);
                        signal_on_off = 1;
                    }
                }
//                else if (intent.getAction().equals(BluetoothLeService.ACTION_NOTIFICATION_LAST_READ_DONE)) {       //  Buzzer
//                    readDone = 1;
//                }
//                }  else if (intent.getAction().equals(BluetoothLeService.ACTION_NOTIFICATION_RECEIVED_START_VALUE)) {       //  Start  , change to options !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//                    byte[] startNotificationData = intent.getByteArrayExtra("startData");
//                    if (startNotificationData[0] == 0) {
//                        onButton.setImageResource(R.drawable.dashboard_off_on);
//                        startButton_on_off=0;
//                    } else if (buzzerNotificationData[0] == BUZZER_MIDLE) {
//                        onButton.setImageResource(R.drawable.dashboard_on_on);
//                        startButton_on_off=1;
//                    }
//
//                }
                else if (intent.getAction().equals(BluetoothLeService.ACTION_NOTIFICATION_RECEIVED_OPTION_VALUE)) {
                    byte[] optionsNotificationData = intent.getByteArrayExtra("optionsData");
                    if (((optionsNotificationData[0]^optionsLastState)&0x1) == 0x1) {               // cruise
                        if ((optionsNotificationData[0] & 0x1) == 0x1) {
                            cruiseButton.setImageResource(R.drawable.dashboard_cruise_on);
                            cruise_on_off=1;
                            optionsToggle  = optionsToggle | 0x1;
                        } else if ((optionsNotificationData[0] & 0x1) == 0) {
                            cruiseButton.setImageResource(R.drawable.dashboard_cruise_off);
                            cruise_on_off=0;
                            optionsToggle  = optionsToggle & (~0x1);
                        }
                        //optionsLastState = optionsNotificationData[0];
                        optionsLastState = (byte) optionsToggle;
                    }  else if (((optionsNotificationData[0]^optionsLastState)&0x2) == 0x2) {       //    warning
                        if ((optionsNotificationData[0] & 0x2) == 0x2) {
                            blinkingTimerWarningThread = new BlinkingTimerWarningThread();
                            blinkingTimerWarningThread.start();
                            warningLight_on_off=1;
                            optionsToggle  = optionsToggle | 0x2;
                        } else if ((optionsNotificationData[0] & 0x2) == 0) {
                            if (blinkingTimerWarningThread!=null) {
                                blinkingTimerWarningThread.setRunning(false);
                                blinkingTimerWarningThread.interrupt();
                            }
                            warningButton.setImageResource(R.drawable.dashboard_warning_on);
                            warningLight_on_off=0;
                            optionsToggle  = optionsToggle & (~0x2);
                        }
                        //optionsLastState = optionsNotificationData[0];
                        optionsLastState = (byte) optionsToggle;
                    } else if (((optionsNotificationData[0]^optionsLastState)&0x4) == 0x4) {       //   Light Auto
                        if ((optionsNotificationData[0] & 0x4) == 0x4) {
                            autoLight.setImageResource(R.drawable.dashboard_auto_light_on);
                            lightAuto_on_off=1;
                            optionsToggle  = optionsToggle | 0x4;
                        } else if ((optionsNotificationData[0] & 0x4) == 0) {
                            autoLight.setImageResource(R.drawable.dashboard_auto_light);
                            lightAuto_on_off=0;
                            optionsToggle  = optionsToggle & (~0x4);
                        }
                        //optionsLastState = optionsNotificationData[0];
                        optionsLastState = (byte) optionsToggle;
                    } else if (((optionsNotificationData[0]^optionsLastState)&0x8) == 0x8) {      //   High Beam
                        if ((optionsNotificationData[0] & 0x8) == 0x8) {
                            highBeam.setImageResource(R.drawable.dashboard_high_beam_on);
                            hightBeam_on_off=1;
                            optionsToggle  = optionsToggle | 0x8;
                        } else if ((optionsNotificationData[0] & 0x8) == 0) {
                            highBeam.setImageResource(R.drawable.dashboard_high_beam_off);
                            hightBeam_on_off=0;
                            optionsToggle  = optionsToggle & (~0x8);
                        }
                        //optionsLastState = optionsNotificationData[0];
                        optionsLastState = (byte) optionsToggle;
                    } else if (((optionsNotificationData[0]^optionsLastState)&0x10) == 0x10) {       // Parking Light
                        if ((optionsNotificationData[0] & 0x10) == 0x10) {
                            parkingLight.setImageResource(R.drawable.dashboard_parking_light_on);
                            parkingLight_on_off=1;
                            optionsToggle  = optionsToggle | 0x10;
                        } else if ((optionsNotificationData[0] & 0x10) == 0) {
                            parkingLight.setImageResource(R.drawable.dashboard_parking_light_off);
                            parkingLight_on_off=0;
                            optionsToggle  = optionsToggle & (~0x10);
                        }
                        //optionsLastState = optionsNotificationData[0];
                        optionsLastState = (byte) optionsToggle;
                    }  else if (((optionsNotificationData[0]^optionsLastState)&0x20) == 0x20) {       // Parking Light
                        if ((optionsNotificationData[0] & 0x20) == 0x20) {
                            onButton.setImageResource(R.drawable.dashboard_on_on);
                            startButton_on_off=1;
                            optionsToggle  = optionsToggle | 0x20;
                        } else if ((optionsNotificationData[0] & 0x20) == 0) {
                            onButton.setImageResource(R.drawable.dashboard_off_on);
                            startButton_on_off=0;
                            optionsToggle  = optionsToggle & (~0x20);
                        }
                        //optionsLastState = optionsNotificationData[0];
                        optionsLastState = (byte) optionsToggle;
                    }

                }
//--------------------------------------------------------------------------------------------------------------------------------
                  else if (intent.getAction().equals(BluetoothLeService.ACTION_NOTIFICATION_RECEIVED_MAGNETOMETER)) {

                    byte[] magnetometerNotificationData = intent.getByteArrayExtra("XYZCompass");

                    double headingDegrees;
                    double headingDegreesNorm;  // Actual degrees from right to left 360 degree
                    double declinationAngleOffset;  // degrees with offset relatively to vertical axe ( direction of car)
                    double[] pointNotCalibrated = new double[3];
                    double[] pointSubstracted = new double[3];
                    double[] pointCalibrated = new double[3];

                    //Data received via Bluetooth is  Little Endian

                    ////Calculate Magnet, Array is already reversed.
                    long x = magnetometerNotificationData[1];
                    x <<= 8;
                    x |= magnetometerNotificationData[0] & 0xFF;
                    pointNotCalibrated[0] = Double.valueOf(x);

                    long y = magnetometerNotificationData[3];
                    y <<= 8;
                    y |= magnetometerNotificationData[2] & 0xFF;
                    pointNotCalibrated[1] = Double.valueOf(y);

                    long z = magnetometerNotificationData[5];
                    z <<= 8;
                    z |= magnetometerNotificationData[4] & 0xFF;
                    pointNotCalibrated[2] = Double.valueOf(z);

                    pointSubstracted = substractVectors(pointNotCalibrated, bias);


                    pointCalibrated = multiplyMatrix(euler, pointSubstracted);
                    //  Axes in the controller are not right. Change them
                    X_Magnetometer_Data = pointCalibrated[0];
                    Y_Magnetometer_Data = pointCalibrated[1];
                    Z_Magnetometer_Data = pointCalibrated[2];

                    double headingRadians = Math.atan2(Y_Magnetometer_Data, X_Magnetometer_Data);
                    headingDegrees = (double) (headingRadians * 180 / Math.PI);
                    if (headingDegrees < 0) {
                        headingDegreesNorm = 360 + headingDegrees;
                    } else {
                        headingDegreesNorm = headingDegrees;
                    }
                    declinationAngleOffset = (offsetAngle + headingDegreesNorm) % 360;
                    compassNorthDirection.setRotation((float) declinationAngleOffset);


//                    compassNorthDirection.setRotation((float)(intent.getDoubleExtra("angleCompass", 0)));


                } else if (intent.getAction().equals(BluetoothLeService.ACTION_NOTIFICATION_RECEIVED_SPEED_SENSOR_VALUE)) {

                    byte[] speedSensorNotificationData = intent.getByteArrayExtra("speedSensorData");

                    float speed;
                    //Data received via Bluetooth is  Little Endian

                    //Calculate period
                    long period = speedSensorNotificationData[0] & 0xFF;
                    period <<= 8;
                    period |= speedSensorNotificationData[1] & 0xFF;
                    period <<= 8;
                    period |= speedSensorNotificationData[2] & 0xFF;

                    if (period == 0){
                        speed = 0;
                    } else {
                        // (( (3,6 grad * pi)/180 grad ) rad /  T ) * 0.032 m   = m/s
                        speed = (float) (((0.0628 * 1000000000) / (Float.valueOf(period) * 12.5)) * 0.032);
                    }
                    binding.setBluetoothViewModelData(bluetoothViewModel);

                    bluetoothViewModel.setSpeedSensor(speed);





                }
            }
        }
    };



    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {


        binding = FragmentHomeBinding.inflate(inflater, container, false);
        View root = binding.getRoot();
        bluetoothViewModel = new ViewModelProvider(requireActivity()).get(BluetoothViewModel.class);
        binding.setBluetoothViewModelData(bluetoothViewModel);


        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothLeService.ACTION_NOTIFICATION_RECEIVED_TRMP_HUMIDITY);
        intentFilter.addAction(BluetoothLeService.ACTION_NOTIFICATION_RECEIVED_MAGNETOMETER);
        intentFilter.addAction(BluetoothLeService.ACTION_NOTIFICATION_RECEIVED_BUZZER_VALUE);
        intentFilter.addAction(BluetoothLeService.ACTION_NOTIFICATION_RECEIVED_SPEED_SENSOR_VALUE);
        intentFilter.addAction(BluetoothLeService.ACTION_NOTIFICATION_RECEIVED_OPTION_VALUE);
        intentFilter.addAction(BluetoothLeService.ACTION_NOTIFICATION_RECEIVED_START_VALUE);
//        intentFilter.addAction(BluetoothLeService.ACTION_NOTIFICATION_LAST_READ_DONE);



        LocalBroadcastManager.getInstance(this.getContext()).registerReceiver(notificationReceiver, intentFilter);

//        Intent gattServiceIntent = new Intent(getContext(), BluetoothLeService.class);
//        getContext().bindService(gattServiceIntent, serviceConnection, Context.BIND_AUTO_CREATE);



        vibe= (Vibrator) getContext().getSystemService(getContext().VIBRATOR_SERVICE) ;

        //myUpdateLeftButtonThread = new UpdateLeftButtonThread();
        //myUpdateRightButtonThread = new UpdateRightButtonThread();

//        final TextView textView = binding.textHome;
//        homeViewModel.getText().observe(getViewLifecycleOwner(), textView::setText);
        circularSeekBarStearing = (CircularSeekBar) binding.circularSeekbar ;

        warningButton = (ImageButton) binding.warningOnButton;
        onButton = (ImageButton) binding.onOffButton;
        cruiseButton = (ImageButton) binding.cruiseButtonOff;
        autoLight = (ImageButton) binding.lightAutoButton;
        highBeam = (ImageButton) binding.highBeamButton;
        parkingLight = (ImageButton) binding.parkingLightButton;


        compassNorthDirection = (ImageView) binding.nordCompass;

        leftBlinker = (ImageView) binding.leftOff;
        rightBlinker = (ImageView) binding.rightOff;

        signalButton = (ImageButton) binding.signalOffButton;

        speedcarSeekBar = (SeekBar)binding.speedSeekBar;
        speedcarBackSeekBar = (SeekBar)binding.speedBackSeekBar;


//        tempView = (TextView) binding.textViewTempValue;
//        humidityView = (TextView) binding.textViewHumidityValue;

        speedcarSeekBar.setMin(MOTOR_POWER_MIN);
        speedcarSeekBar.setMax(MOTOR_POWER_MAX);

        speedcarBackSeekBar.setMin(MOTOR_POWER_MIN);
        speedcarBackSeekBar.setMax(MOTOR_POWER_MAX);

        homeViewModel = new ViewModelProvider(this).get(HomeViewModel.class);

        binding.setBluetoothViewModelData(bluetoothViewModel);     // Set initially speed to 0 on dashboard
        bluetoothViewModel.setSpeedSensor(0);

        mediaPlayerOnBlinking = MediaPlayer.create(this.getContext(), R.raw.onblinking);
        mediaPlayerOffBlinking = MediaPlayer.create(this.getContext(), R.raw.offblinking);

        // Implenmenation of circular seekbar
        circularSeekBarStearing.setOnSeekBarChangeListener( new CircularSeekBar.OnCircularSeekBarChangeListener () {
            @Override
            public void onProgressChanged(CircularSeekBar circularSeekBar, int progress, boolean fromUser) {
                // TODO Insert your code here

                stearEngle = progress;

                byte[] val = new byte[1];
                val[0] = (byte) (progress & 0xFF);
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {

                    //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE));
                    sendCharacteristic(val, BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                }
                if (stearEngle > 60) {
                    if (StearingDirection!=1) {
                        blinkingTimerLeftThread = new BlinkingTimerLeftThread();
                        blinkingTimerLeftThread.start();

                        if (blinkingTimerRightThread!=null) {
                            blinkingTimerRightThread.setRunning(false);
                            blinkingTimerRightThread.interrupt();
                        }
                        rightBlinker.setImageResource(R.drawable.dashboard_right_off);
                        StearingDirection = 1;
                    }
                } else if (stearEngle < 40){
                    if (StearingDirection!=2){
                        blinkingTimerRightThread = new BlinkingTimerRightThread();
                        blinkingTimerRightThread.start();

                        if (blinkingTimerLeftThread!=null) {
                            blinkingTimerLeftThread.setRunning(false);
                            blinkingTimerLeftThread.interrupt();
                        }
                        leftBlinker.setImageResource(R.drawable.dashboard_left_off);
                        StearingDirection = 2;
                    }
                } else if ((stearEngle >=40)&&(stearEngle <=60)){
                    if (StearingDirection!=0){
                        if (blinkingTimerLeftThread!=null) {
                            blinkingTimerLeftThread.setRunning(false);
                            blinkingTimerLeftThread.interrupt();
                        }
                        if (blinkingTimerRightThread!=null) {
                            blinkingTimerRightThread.setRunning(false);
                            blinkingTimerRightThread.interrupt();
                        }
                        leftBlinker.setImageResource(R.drawable.dashboard_left_off);
                        rightBlinker.setImageResource(R.drawable.dashboard_right_off);
                        StearingDirection = 0;
                    }
                }

            }

            @Override
            public void onStopTrackingTouch(CircularSeekBar seekBar) {
                autoStearThread = new AutoStearThread();
                autoStearThread.start();
            }

            @Override
            public void onStartTrackingTouch(CircularSeekBar seekBar) {
                if (autoStearThread!=null) {
                    autoStearThread.setRunning(false);
                    autoStearThread.interrupt();
                }

            }
        });




        speedcarSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (progress<40) {
                    throttleProgress = 0;
                    binding.setBluetoothViewModelData(bluetoothViewModel);     // Set speed 0 on dashboard
                    bluetoothViewModel.setSpeedSensor(throttleProgress);
                    if (LastSpeedState == 1) {
                        BluetoothLeService.ActiveSpeedReadThread = 0;
                        LastSpeedState = 0;
                    }
                } else {
                    throttleProgress = progress & 0xFF;
                    if (LastSpeedState == 0) {
                        BluetoothLeService.ActiveSpeedReadThread = 1;
                        LastSpeedState = 1;
                    }
                }
                byte[] val = new byte[1];
                val[0] = (byte) throttleProgress;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                    //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_SPEED_FORWARD_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                    sendCharacteristic(val, BluetoothLeService.CAR_SPEED_FORWARD_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                }
            }


            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

                speedcarBackSeekBar.setProgress(0);
//                if (autoBrakeBackThread!=null) {
//                    autoBrakeBackThread.setRunning(false);
//                    autoBrakeBackThread.interrupt();
//                }
//                autoBrakeBackThread = new AutoBrakeBackThread();
//                autoBrakeBackThread.start();


//                if (autoBrakeThread!=null) {
//                    autoBrakeThread.setRunning(false);
//                    autoBrakeThread.interrupt();
//                }


            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {


//                autoBrakeThread = new AutoBrakeThread();
//                autoBrakeThread.start();

            }

        });


        speedcarBackSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

                if (progress<40) {

                    throttleBackProgress = 0;
                    binding.setBluetoothViewModelData(bluetoothViewModel);     // Set speed 0 on dashboard
                    bluetoothViewModel.setSpeedSensor(throttleBackProgress);
                    if (LastSpeedState == 1) {
                        BluetoothLeService.ActiveSpeedReadThread = 0;
                        LastSpeedState = 0;
                    }
                } else {
                    throttleBackProgress = progress & 0xFF;
                    if (LastSpeedState == 0) {
                        BluetoothLeService.ActiveSpeedReadThread = 1;
                        LastSpeedState = 1;
                    }
                }
                byte[] val = new byte[1];
                val[0] = (byte) throttleBackProgress;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                    //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_SPEED_BACKWARD_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                    sendCharacteristic(val, BluetoothLeService.CAR_SPEED_BACKWARD_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                }

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                speedcarSeekBar.setProgress(0);
//                if (autoBrakeThread!=null) {
//                autoBrakeThread.setRunning(false);
//                autoBrakeThread.interrupt();
//                }
//                autoBrakeThread = new AutoBrakeThread();
//                autoBrakeThread.start();

//                if (autoBrakeBackThread!=null) {
//                    autoBrakeBackThread.setRunning(false);
//                    autoBrakeBackThread.interrupt();
//                }


            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

//                autoBrakeBackThread = new AutoBrakeBackThread();
//                autoBrakeBackThread.start();

            }

        });


//speedcarSeekBar.setOnTouchListener(new View.OnTouchListener() {
//    @Override
//    public boolean onTouch(View v, MotionEvent event) {
//        if(event.getAction() == MotionEvent.ACTION_DOWN) {
//            return true;
//
//        } else if (event.getAction() == MotionEvent.ACTION_UP) {
//
//            autoBrakeThread = new AutoBrakeThread();
//            autoBrakeThread.start();
//
//        }
//
//
//
//        return true;
//    }
//});

        warningButton.setOnClickListener(new View.OnClickListener() {


            @Override
            public void onClick(View v) {


                // TODO Auto-generated method stub
                // Turn Car ON
                vibe.vibrate(50);

                if (warningLight_on_off==0){
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        byte[] val =  new byte[1];
                        val[0] = (byte) ((byte )optionsToggle|0x02);
                        //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                        sendCharacteristic(val, BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                    }
//                    blinkingTimerWarningThread = new BlinkingTimerWarningThread();
//                    blinkingTimerWarningThread.start();

//                    warningLight_on_off=1;

                } else
                {
                    // Turn your car OFF
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        byte[] val =  new byte[1];
                        val[0] = (byte) ((byte )optionsToggle&(~0x02));
                        //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                        sendCharacteristic(val, BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                    }
//                    if (blinkingTimerWarningThread!=null) {
//                        blinkingTimerWarningThread.setRunning(false);
//                        blinkingTimerWarningThread.interrupt();
//                    }
//                    warningButton.setImageResource(R.drawable.dashboard_warning_on);
//                    warningLight_on_off=0;
                }
            }
        });


        onButton.setOnClickListener(new View.OnClickListener() {



            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                // Turn Car ON
                vibe.vibrate(50);

                if (startButton_on_off==0){
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        byte[] val =  new byte[1];
                        val[0] = (byte) ((byte )optionsToggle|0x20);
                        //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                        sendCharacteristic(val, BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                    }
//                    parkingLight.setImageResource(R.drawable.dashboard_parking_light_on);
//                    parkingLight_on_off=1;

                } else
                {
                    // Turn your car OFF
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        byte[] val =  new byte[1];
                        val[0] = (byte) ((byte )optionsToggle&(~0x20));
                        //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                        sendCharacteristic(val, BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                    }
//                    parkingLight.setImageResource(R.drawable.dashboard_parking_light_off);
//                    parkingLight_on_off=0;
                }
            }
        });


        cruiseButton.setOnClickListener(new View.OnClickListener() {


            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                // Turn Car ON
                vibe.vibrate(50);

                if (cruise_on_off==0){
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        byte[] val =  new byte[1];
                        val[0] = (byte) ((byte )optionsToggle|0x01);
                        //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                        sendCharacteristic(val, BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                    }
//                    cruiseButton.setImageResource(R.drawable.dashboard_cruise_on);
//                    on_off=1;


                } else
                {
                    // Turn your car OFF
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        byte[] val =  new byte[1];
                        val[0] = (byte) ((byte )optionsToggle&(~0x01));
                        //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                        sendCharacteristic(val, BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                    }
//                    cruiseButton.setImageResource(R.drawable.dashboard_cruise_off);
//                    on_off=0;
                }
            }
        });

       autoLight.setOnClickListener(new View.OnClickListener() {


            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                // Turn Car ON
                vibe.vibrate(50);

                if (lightAuto_on_off==0){
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        byte[] val =  new byte[1];
                        val[0] = (byte) ((byte )optionsToggle|0x04);
                        //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                        sendCharacteristic(val, BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                    }
//                    autoLight.setImageResource(R.drawable.dashboard_auto_light_on);
//                    lightAuto_on_off=1;

                } else
                {
                    // Turn your car OFF
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        byte[] val =  new byte[1];
                        val[0] = (byte) ((byte )optionsToggle&(~0x04));
                        //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                        sendCharacteristic(val, BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                    }
//                    autoLight.setImageResource(R.drawable.dashboard_auto_light);
//                    lightAuto_on_off=0;
                }
            }
        });


        parkingLight.setOnClickListener(new View.OnClickListener() {


            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                // Turn Car ON
                vibe.vibrate(50);

                if (parkingLight_on_off==0){
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        byte[] val =  new byte[1];
                        val[0] = (byte) ((byte )optionsToggle|0x10);
                        //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                        sendCharacteristic(val, BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                    }
//                    parkingLight.setImageResource(R.drawable.dashboard_parking_light_on);
//                    parkingLight_on_off=1;

                } else
                {
                    // Turn your car OFF
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        byte[] val =  new byte[1];
                        val[0] = (byte) ((byte )optionsToggle&(~0x10));
                        //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                        sendCharacteristic(val, BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                    }
//                    parkingLight.setImageResource(R.drawable.dashboard_parking_light_off);
//                    parkingLight_on_off=0;
                }
            }
        });

        highBeam.setOnClickListener(new View.OnClickListener() {


            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub

                //   Send notification to Service to cancel read thread and and ask if read was done
//                Intent intent = new Intent(BluetoothLeService.ACTION_NOTIFICATION_CANCEL_READ_CHARACTERISTICS);
//                int CancelNotification = 0;
//                intent.putExtra("cancel", CancelNotification);
//                LocalBroadcastManager.getInstance(getContext()).sendBroadcast(intent);
//                BluetoothLeService.lastRead = 1;  //  Need Onread to say when last Read was done
//
//                    if (BluetoothLeService.mDeviceBusy) {
//                        while (readDone != 1) {
//                        }
//                    }
//                readDone = 0;


                vibe.vibrate(50);

                if (hightBeam_on_off==0){
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        byte[] val =  new byte[1];
                        val[0] = (byte) ((byte )optionsToggle|0x8);
                        //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                        sendCharacteristic(val, BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                    }
//                    highBeam.setImageResource(R.drawable.dashboard_high_beam_on);
//                    hightBeam_on_off=1;

                } else
                {
                    // Turn your car OFF
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        byte[] val =  new byte[1];
                        val[0] = (byte) ((byte )optionsToggle&(~0x8));
                        //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                        sendCharacteristic(val, BluetoothLeService.CAR_OPTIONS_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                    }
//                    highBeam.setImageResource(R.drawable.dashboard_high_beam_off);
//                    hightBeam_on_off=0;
                }
            }
        });



        signalButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {


                    // TODO Auto-generated method stub
                    // Turn Signal ON
                    vibe.vibrate(50);

                    if (signal_on_off == 0) {
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                            byte[] val = new byte[1];
                            val[0] = (byte) BUZZER_MIDLE;

                            //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_BUZZER_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                            sendCharacteristic(val, BluetoothLeService.CAR_BUZZER_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);

                        }
//                        signalButton.setImageResource(R.drawable.dashboard_signal_on);
//                        signal_on_off = 1;
                    } else if (signal_on_off == 1) {
                        // Turn your signal OFF
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                            byte[] val = new byte[1];
                            val[0] = (byte) BUZZER_OFF;

                            //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_BUZZER_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                            sendCharacteristic(val, BluetoothLeService.CAR_BUZZER_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                        }
//                        signalButton.setImageResource(R.drawable.dashboard_signal_off);
//                        signal_on_off = 0;
                    }
            }
        });



//        leftButton.setOnTouchListener(new View.OnTouchListener() {
//            @Override
//            public boolean onTouch(View v, MotionEvent event) {
//               if (event.getAction() ==  MotionEvent.ACTION_DOWN){
//                   stearEngle[0]++;
//                   if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                       sendCharacteristic(stearEngle, BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID);
//                   }
//                    while((event.getAction() ==  MotionEvent.ACTION_DOWN) && (stearEngle[0] < STERAANGLE_MAX))
//                    {
//                        handler.postDelayed(new Runnable() {
//                           @Override
//                           public void run() {
//
//                                   stearEngle[0]++;
//                                   if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                                       sendCharacteristic(stearEngle, BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID);
//                                   }
//
//
//                           }
//                        }, ITERATION_PERIOD);
//
//                    }
//
//
//                } else if (event.getAction() == MotionEvent.ACTION_UP) {
//                   stearEngle[0]--;
//                   if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                       sendCharacteristic(stearEngle, BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID);
//                   }
//                   while((event.getAction() ==  MotionEvent.ACTION_UP) && (stearEngle[0] > STERAANGLE_MIDDLE))
//                   {
//                       handler.postDelayed(new Runnable() {
//                           @Override
//                           public void run() {
//
//                               stearEngle[0]--;
//                               if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                                   sendCharacteristic(stearEngle, BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID);
//                               }
//
//
//                           }
//                       }, ITERATION_PERIOD);
//
//                   }
//
//
//                }
//                return true;
//            }
//
//        });



//        leftButton.setOnTouchListener(new View.OnTouchListener() {
//                                          @Override
//                                          public boolean onTouch(View v, MotionEvent event) {
//
//                                              switch (event.getAction()) {
//                                                  case MotionEvent.ACTION_DOWN:
//
//                                                      do {
//
//                                                          handler.postDelayed(new Runnable() {
//                                                              @Override
//                                                              public void run() {
//
//                                                                  Log.e(TAG, "LongPress");
//
//                                                              }
//                                                          }, ITERATION_PERIOD);
//                                                          Log.e(TAG, "ACTION_DOWN");
//
//                                                      } while (event.getAction()==MotionEvent.ACTION_UP);
//
//                                                      break;
//
//
//                                                  case MotionEvent.ACTION_UP:
//                                                      Log.e(TAG, "ACTION_UP");
//
//                                                      break;
//
//                                              }
//
//                                            return true;
//                                          }
//                                      });








//        leftButton.setOnTouchListener(new View.OnTouchListener() {
//            @Override
//            public boolean onTouch(View v, MotionEvent event) {
//                if(event.getAction() == MotionEvent.ACTION_DOWN) {
////                    myUpdateLeftButtonThread.toggleThread();
////                    myUpdateLeftButtonThread.run();
//
//                    byte[] val = new byte[1];
//                    val[0] = (byte) STERAANGLE_MAX;
//                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                        sendCharacteristic(val, BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID);
//                    }
//
//
//                    Log.e(TAG, "Unable to initialize Bluetooth");
//                } else if (event.getAction() == MotionEvent.ACTION_UP) {
//                    byte[] val = new byte[1];
//                    val[0] = (byte) STERAANGLE_MIDDLE;
//                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                        sendCharacteristic(val, BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID);
//                    }
//
//                    Log.e(TAG, "Unable to initialize Bluetooth");
//                }
//                return true;
//            }
//        });


//        rightButton.setOnTouchListener(new View.OnTouchListener() {
//            @Override
//            public boolean onTouch(View v, MotionEvent event) {
//                if(event.getAction() == MotionEvent.ACTION_DOWN) {
////                    myUpdateLeftButtonThread.toggleThread();
////                    myUpdateLeftButtonThread.run();
//
//                    byte[] val = new byte[1];
//                    val[0] = (byte) STERAANGLE_MIN;
//                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                        sendCharacteristic(val, BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID);
//                    }
//
//
//                    Log.e(TAG, "Unable to initialize Bluetooth");
//                } else if (event.getAction() == MotionEvent.ACTION_UP) {
//                    byte[] val = new byte[1];
//                    val[0] = (byte) STERAANGLE_MIDDLE;
//                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
//                        sendCharacteristic(val, BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID);
//                    }
//
//                    Log.e(TAG, "Unable to initialize Bluetooth");
//                }
//                return true;
//            }
//        });


//        thrrottleButton.setOnTouchListener(new View.OnTouchListener() {
//            @Override
//            public boolean onTouch(View v, MotionEvent event) {
//                if(event.getAction() == MotionEvent.ACTION_DOWN) {
//
//                    myUpdateThrottleGasThread = new UpdateThrottleGasThread();
//                    myUpdateThrottleGasThread.start();
//
//
//                } else if (event.getAction() == MotionEvent.ACTION_UP) {
//
//
//                    myUpdateThrottleGasThread.setRunning(false);
//                    myUpdateThrottleGasThread.interrupt();
//
//
//                }
//                return true;
//            }
//        });


//        brakeButton.setOnTouchListener(new View.OnTouchListener() {
//            @Override
//            public boolean onTouch(View v, MotionEvent event) {
//                if(event.getAction() == MotionEvent.ACTION_DOWN) {
//
//
//                    myUpdateBrakeThread = new UpdateBrakeThread();
//                    myUpdateBrakeThread.start();
//
//
//                } else if (event.getAction() == MotionEvent.ACTION_UP) {
//                    myUpdateBrakeThread.setRunning(false);
//                    myUpdateBrakeThread.interrupt();
//
//                }
//                return true;
//            }
//        });


         return root;
    }


    public ImageButton getLeftButton() {
        return leftButton;
    }

//    private void unSubscribeNoticationCharacteristics(BluetoothGatt bluetoothGatt) {
//
//        if (bluetoothGatt == null) {
//            return;
//        }
//
//        BluetoothGattCharacteristic characteristic = BluetoothLeService.notificationDeactivateChars.get(0);
//        if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
//            // TODO: Consider calling
//            //    ActivityCompat#requestPermissions
//            // here to request the missing permissions, and then overriding
//            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
//            //                                          int[] grantResults)
//            // to handle the case where the user grants the permission. See the documentation
//            // for ActivityCompat#requestPermissions for more details.
//            return;
//        }
//        bluetoothGatt.setCharacteristicNotification(characteristic, false);
//        BluetoothGattDescriptor descriptor = characteristic.getDescriptor(UUID.fromString(BluetoothLeService.BLEUUID.CAR_NOTIFICATION_CCCD_DESCRIPTOR));
////            // get Characteristics
////            gattDescriptors = ch.getDescriptors();
//
//        if(descriptor != null) {
//            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
//                descriptor.setValue(BluetoothGattDescriptor.DISABLE_NOTIFICATION_VALUE);
//            }
//            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
//                bluetoothGatt.writeDescriptor(descriptor);
//            }
//        }
//    }



    // Set Characteristic new Value
    @RequiresApi(api = Build.VERSION_CODES.TIRAMISU)
    public boolean sendCharacteristic(byte[] value, UUID uuid, int WriteType) {

//        bluetoothGatt = bluetoothService.getBluetoothGatt();
//        BluetoothGattCharacteristic ch = (BluetoothGattCharacteristic) bluetoothService.getmService().getCharacteristic(uuid);


        BluetoothGattCharacteristic ch = (BluetoothGattCharacteristic) BluetoothLeService.getmService().getCharacteristic(uuid);

        if (ActivityCompat.checkSelfPermission(getActivity(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
            // TODO: Consider calling
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.
            return false;
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {

            //           BluetoothLeService.getBluetoothGatt().writeCharacteristic(ch, value, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);

            BluetoothGatt gatt = BluetoothLeService.getBluetoothGatt();

            if (WriteType == BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT) {
                synchronized (BluetoothLeService.mDeviceBusy) {
                    if (BluetoothLeService.mDeviceBusy) return false;
                    BluetoothLeService.mDeviceBusy = true;
                }


                if (BluetoothLeService.executeCharacteristicList.isEmpty()) {
                    int result = gatt.writeCharacteristic(ch, value, WriteType);

                    if (result != 0) {
                        Log.w(TAG, "writeCharacteristic() is failed, returns !=0");
                        return false;
                    } else {
                        BluetoothLeService.executeCharacteristicList.add(ch);
                    }
                }
            } else {
                int result = gatt.writeCharacteristic(ch, value, WriteType);
            }
        }
        return true;

    }


//    @Override
//    public void onStart() {
//        Intent gattServiceIntent = new Intent(getContext(), BluetoothLeService.class);
//        getContext().bindService(gattServiceIntent, serviceConnection, Context.BIND_AUTO_CREATE);
//        super.onStart();
//    }


//    @Override
//    public void onStop()
//    {
//        super.onStop();
//        this.getContext().unregisterReceiver(bluetoothDataReceiver);           //<-- Unregister to avoid memoryleak
//    }




    @Override
    public void onDestroyView() {
        super.onDestroyView();
        LocalBroadcastManager.getInstance(this.getContext()).unregisterReceiver(notificationReceiver);
    }
}