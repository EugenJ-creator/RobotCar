/*
 * Copyright 2025 Evgheni Jaruc
 */

package com.example.navigationleftexample.ui;

import android.Manifest;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;

import androidx.activity.EdgeToEdge;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;

import com.example.navigationleftexample.MainActivity;
import com.example.navigationleftexample.R;


import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Intent;
import android.os.Build;

import android.view.MenuItem;

import android.view.Menu;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ListView;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toolbar;

import com.example.navigationleftexample.databinding.FragmentHomeBinding;
import com.example.navigationleftexample.ui.bluetooth.BluetoothLeService;
import com.example.navigationleftexample.ui.bluetooth.BluetoothViewModel;
import com.example.navigationleftexample.ui.settings;
import com.google.android.material.snackbar.Snackbar;
import com.google.android.material.navigation.NavigationView;

import androidx.annotation.NonNull;
import androidx.core.view.WindowCompat;
import androidx.lifecycle.ViewModelProvider;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.fragment.NavHostFragment;
import androidx.navigation.ui.AppBarConfiguration;
import androidx.navigation.ui.NavigationUI;
import androidx.drawerlayout.widget.DrawerLayout;
import androidx.appcompat.app.AppCompatActivity;

import com.example.navigationleftexample.databinding.ActivityMainBinding;

import java.util.UUID;


public class settings extends AppCompatActivity {
    private static final String TAG = "HomeFragment ";
    private SeekBar powerHighBeamSeekBar;
    private View decorView;
    public static int HighBeamPowerProgress;

    private final static int HIGH_BEAM_POWER_MAX = 250;
    private final static int HIGH_BEAM_POWER_MIN = 0;
    //private FragmentHomeBinding binding;
    private int hideSystemBars(){
        return  View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                |View.SYSTEM_UI_FLAG_FULLSCREEN
//                |View.SYSTEM_UI_FLAG_LAYOUT_STABLE
//                |View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                |View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
    }

   //View.SYSTEM_UI_FLAG_HIDE_NAVIGATION


    // Broadcast Receiver
    // Don't forget to add new notification to filter!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    BroadcastReceiver notificationReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent != null && intent.getAction() != null) {
                if (intent.getAction().equals(BluetoothLeService.ACTION_NOTIFICATION_RECEIVED_HIGH_BEAM_POWER_VALUE)) {
                    byte[] dutyCicleData = intent.getByteArrayExtra("highBeamPowerData");
                    powerHighBeamSeekBar.setProgress(dutyCicleData[0]);
                }
            }
        }


    };














    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_settings);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

//        ColorDrawable colorDrawable
//                = new ColorDrawable(Color.parseColor("#0F9D58"));
//
//        // Set BackgroundDrawable
//        actionBar.setBackgroundDrawable(colorDrawable);

        //android:background="#132A59"
        //Toolbar toolbar = findViewById(R.id.toolbar);
        getSupportActionBar().setTitle("Settings");



        LayoutInflater inflater = (LayoutInflater)getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View layout = inflater.inflate(R.layout.activity_settings, null);


        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothLeService.ACTION_NOTIFICATION_RECEIVED_HIGH_BEAM_POWER_VALUE);

        LocalBroadcastManager.getInstance(this).registerReceiver(notificationReceiver, intentFilter);
//        bluetoothViewModel = new ViewModelProvider(requireActivity()).get(BluetoothViewModel.class);
//        binding.setBluetoothViewModelData(bluetoothViewModel);

        decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(hideSystemBars());

        WindowCompat.setDecorFitsSystemWindows(getWindow(),false);

       //getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        powerHighBeamSeekBar = findViewById(R.id.highBeamSeekBar);

        powerHighBeamSeekBar.setMin(HIGH_BEAM_POWER_MIN);
        powerHighBeamSeekBar.setMax(HIGH_BEAM_POWER_MAX);


        readCharacteristic(BluetoothLeService.CAR_HIGH_BEAM_POWER_CHARACTERISTIC_UUID);


        powerHighBeamSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

                HighBeamPowerProgress = progress & 0xFF;

                byte[] val = new byte[1];
                val[0] = (byte) HighBeamPowerProgress;
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                    //BluetoothLeService.charactersiticFifo.add(new Characteristic(BluetoothLeService.CAR_SPEED_FORWARD_CHARACTERISTIC_UUID, val, BluetoothLeService.CharType.WRITE ));
                    sendCharacteristic(val, BluetoothLeService.CAR_HIGH_BEAM_POWER_CHARACTERISTIC_UUID, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
                }
            }


            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

               // speedcarBackSeekBar.setProgress(0);


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


    }





    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.settings, menu);
        return true;
    }

//    @Override
//    public boolean onSupportNavigateUp() {
//        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment_content_main);
//        return NavigationUI.navigateUp(navController, mAppBarConfiguration)
//                || super.onSupportNavigateUp();
//    }


    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {



        if (item.getItemId() == R.id.dashboardMenu) {
            Intent intent = new Intent(this, MainActivity.class);

            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);   // Go to Main Activity without to execute function OnCreate()
            //startActivity(intent);

            intent.setFlags(Intent.FLAG_ACTIVITY_REORDER_TO_FRONT);
            startActivityIfNeeded(intent, 0);

        } else if (item.getItemId() == R.id.exitMenu) {
            finish();
            System.exit(0);
        }


        return super.onOptionsItemSelected(item);
    }


    @RequiresApi(api = Build.VERSION_CODES.TIRAMISU)
    public boolean sendCharacteristic(byte[] value, UUID uuid, int WriteType) {

//        bluetoothGatt = bluetoothService.getBluetoothGatt();
//        BluetoothGattCharacteristic ch = (BluetoothGattCharacteristic) bluetoothService.getmService().getCharacteristic(uuid);


        BluetoothGattCharacteristic ch = (BluetoothGattCharacteristic) BluetoothLeService.getmService().getCharacteristic(uuid);

        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
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

    public boolean readCharacteristic(UUID uuid) {
        BluetoothGatt bluetoothGatt = BluetoothLeService.getBluetoothGatt();
        if ( bluetoothGatt == null) {
            return false;
        }

        synchronized (BluetoothLeService.mDeviceBusy) {
            if (BluetoothLeService.mDeviceBusy) return false;
            BluetoothLeService.mDeviceBusy = true;
        }

        BluetoothGattCharacteristic ch = (BluetoothGattCharacteristic) BluetoothLeService.getmService().getCharacteristic(uuid);
        ch.setWriteType(BluetoothGattCharacteristic.PERMISSION_READ);


        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
            // TODO: Consider calling
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.
            return false;
        }


        if (BluetoothLeService.executeCharacteristicList.isEmpty()) {
            boolean result = bluetoothGatt.readCharacteristic(ch);

            if (result == false) {
                Log.w(TAG, "writeCharacteristic() is failed, returns !=0");
                return false;
            } else {
                BluetoothLeService.executeCharacteristicList.add(ch);
            }

            Log.i("READ", "CHARACTERISTIC WAS RED");

            return true;
        } else {
            return false;
        }


    }








}