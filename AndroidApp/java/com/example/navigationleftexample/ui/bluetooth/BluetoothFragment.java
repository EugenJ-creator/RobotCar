package com.example.navigationleftexample.ui.bluetooth;

//import static com.example.navigationleftexample.ui.bluetooth.BluetoothLeService.DIRECTION_CHARACTERISTIC_UUID;
//import static com.example.navigationleftexample.ui.bluetooth.BluetoothLeService.STEERING_ANGLE_CHARACTERISTIC_UUID;
//import static com.example.navigationleftexample.ui.bluetooth.BluetoothLeService.CAR_SPEED_CHARACTERISTIC_UUID;

import android.Manifest;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.CompoundButton;
import android.widget.ListView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;


import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.databinding.DataBindingUtil;
import androidx.databinding.ViewDataBinding;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;
import androidx.lifecycle.ViewModelProvider;

import android.os.*;
import android.widget.Toolbar;

import com.example.navigationleftexample.R;
import com.example.navigationleftexample.databinding.FragmentBluetoothBinding;
import com.example.navigationleftexample.databinding.FragmentHomeBinding;
import com.example.navigationleftexample.ui.ViewModels.BluetoothGameDataViewModel;

import java.util.List;
import java.util.UUID;


public class BluetoothFragment extends Fragment {
    Switch bluetoothSwitch;
    private FragmentBluetoothBinding binding;
    private BluetoothAdapter mBluetoothAdapter;
    private String deviceAddress;
    private String CHANNEL_ID = "001";

    public boolean bluetoothToggleON = false;
    private Bundle savedState;
    private boolean saved;
    private static final String _FRAGMENT_STATE = "FRAGMENT_STATE";

//    BluetoothDevice [] deviceArray;

    List<BluetoothDevice> deviceArray;

    private List<BluetoothGattService> listGATTServices;

    private LayoutInflater layoutInflater;
    private BluetoothLeScanner bluetoothLeScanner;
//    private boolean mScanning;
//    private Handler mHandler = new Handler();

    private boolean connected = false;

    private boolean scanning = false;
    private Handler handler = new Handler();

    public static final String[] BLUETOOTH_PERMISSIONS_S = {Manifest.permission.BLUETOOTH_SCAN, Manifest.permission.BLUETOOTH_CONNECT};

    // private LeDeviceListAdapter mLeDeviceListAdapter;

    private static final int PERMISSION_REQUEST_COARSE_LOCATION = 456;

    private static final int REQUEST_CODE_BLUETOOTH_SCAN = 1;

    // Stops scanning after 5 seconds.
    private static final long SCAN_PERIOD = 5000;

    private BluetoothViewModel bluetoothViewModel;

    BluetoothGameDataViewModel bluetoothGameViewModel;
//    private static BluetoothLeService bluetoothService;
    private  BluetoothLeService bluetoothService;

    TextView bluetoothTextViewOn;
    Intent gattServiceIntent;

    //LeDeviceListAdapter leDeviceListAdapter = new LeDeviceListAdapter(layoutInflater);
    LeDeviceListAdapter leDeviceListAdapter;

    private static final String TAG = "MessageBluetooth ";




//////////////  Implement !!!!
//    private final BroadcastReceiver gattUpdateReceiver = new BroadcastReceiver() {
//        @Override
//        public void onReceive(Context context, Intent intent) {
//            final String action = intent.getAction();
//            if (BluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
//                connected = true;
//                // Implement !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//                //updateConnectionState(R.string.connected);
//            } else if (BluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
//                connected = false;
//                // Implement !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//                //updateConnectionState(R.string.disconnected);
//            } else if (BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED.equals(action)) {
//                // Show all the supported services and characteristics on the user interface.
////                displayGattServices(bluetoothService.getSupportedGattServices());
//                listGATTServices = bluetoothService.getSupportedGattServices();
//            }
//        }
//    };



    private ServiceConnection serviceConnection = new ServiceConnection() {
    @Override
    public void onServiceConnected(ComponentName name, IBinder service) {
        bluetoothService = ((BluetoothLeService.LocalBinder) service).getService();
        if (bluetoothService != null) {
            if (!bluetoothService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!    Define finish, Notification devise is not reacheble
            //    finish();
            }






            // perform device connection
            if (bluetoothService.connect(deviceAddress))
            {
                Toast.makeText(getContext(), R.string.ble_connected, Toast.LENGTH_SHORT).show();

            }
            else
            {
                Toast.makeText(getContext(), R.string.ble_not_connected, Toast.LENGTH_SHORT).show();
            }

        }
    }

    @Override
    public void onServiceDisconnected(ComponentName name) {
        bluetoothService = null;
    }
};

    @Override
    public void onStart() {
        super.onStart();
        Log.i(TAG, "onStart");
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG, "onCreate");
    }


    @Override
    public void onDestroy() {
        //super.onDestroy();

        bluetoothViewModel.setLastServiceConnection(serviceConnection);

        savedState = getSavedState();
        saved = true;

        //super.onDestroyView();
        super.onDestroy();
        Log.i(TAG, "onDestroy");
    }

    protected Bundle getSavedState() {
        return savedState;
    }

    @Override
    public void onStop() {
        super.onStop();
        Log.i(TAG, "onStop");
        stopScanLeDevice();
        handler.removeCallbacksAndMessages(null);  // Cancel postDelayed
//        bluetoothViewModel.setPairedDeviceToSavedStateHandle();
    }

    @Override
    public void onResume() {
        super.onResume();

        Log.i(TAG, "onResume");
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        Log.i(TAG, "onViewCreated");
    }


    @Override
    public void onDetach() {
        super.onDetach();
        Log.i(TAG, "onDetach");
    }

    @Override
    public void onPause() {
        super.onPause();
        Log.i(TAG, "onPause");
    }

    @Override
    public void onViewStateRestored(@Nullable Bundle savedInstanceState) {
        super.onViewStateRestored(savedInstanceState);
        Log.i(TAG, "onViewStateRestored");
        TextView pairedDeviseText = binding.textViewBluetoothDeviceChosen;

//        if (!(bluetoothViewModel.getDevice() == null)) {
//
//            bluetoothViewModel.getDevice().observe(getViewLifecycleOwner(), bluetoothDevice -> {
//                if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
//                    // TODO: Consider calling
//                    //    ActivityCompat#requestPermissions
//                    // here to request the missing permissions, and then overriding
//                    //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
//                    //                                          int[] grantResults)
//                    // to handle the case where the user grants the permission. See the documentation
//                    // for ActivityCompat#requestPermissions for more details.
//                    return;
//                }
//                pairedDeviseText.setText(bluetoothDevice.getName());
//            });
//        } else {
//            pairedDeviseText.setText("");
//        }


//        bluetoothViewModel.getBluetoothActive().observe(getViewLifecycleOwner(), bluetoothActive -> {
//            if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
//                // TODO: Consider calling
//                //    ActivityCompat#requestPermissions
//                // here to request the missing permissions, and then overriding
//                //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
//                //                                          int[] grantResults)
//                // to handle the case where the user grants the permission. See the documentation
//                // for ActivityCompat#requestPermissions for more details.
//                return;
//            }
//
//            bluetoothSwitch.setChecked(bluetoothActive);
//
//        });
//        bluetoothSwitch.setOnClickListener(null);

        bluetoothTextViewOn = (TextView) binding.textViewBluetoothOn;

        if (bluetoothViewModel.getBluetoothActive().getValue() == true){
            bluetoothToggleON = true;
            bluetoothTextViewOn.setText("On");
            if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                // TODO: Consider calling
                //    ActivityCompat#requestPermissions
                // here to request the missing permissions, and then overriding
                //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                //                                          int[] grantResults)
                // to handle the case where the user grants the permission. See the documentation
                // for ActivityCompat#requestPermissions for more details.
                return;
            }
            pairedDeviseText.setText(bluetoothViewModel.getDevice().getValue().getName());
            bluetoothSwitch.setChecked(bluetoothViewModel.getBluetoothActive().getValue());
        } else {
            bluetoothTextViewOn.setText("Off");
            pairedDeviseText.setText("");
        }

//        bluetoothSwitch.setChecked(bluetoothViewModel.getBluetoothActive().getValue());

//        bluetoothSwitch.setOnClickListener(mOn);



//        Switch switchButton = binding.switchBluetoothOn;
//        bluetoothViewModel.getSwitch().observe(getViewLifecycleOwner(), aSwitch -> {
//            switchButton.setChecked(aSwitch.isChecked());
//        });
    }


    @Override
    public void onSaveInstanceState(@NonNull Bundle outState) {
        super.onSaveInstanceState(outState);
        //Save the fragment's instance
//        getFragmentManager().putFragment(outState, "bluetoothFragment", this);
        if (getView() == null) {
            outState.putBundle(_FRAGMENT_STATE, savedState);
        } else {
            Bundle bundle = saved ? savedState : getSavedState();

            outState.putBundle(_FRAGMENT_STATE, bundle);
        }

        saved = false;

        super.onSaveInstanceState(outState);

        Log.i(TAG,"onSAveInstanceState");

    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        if (savedInstanceState != null) {
            //Restore the fragment's state here
        }
    }

    private boolean checkPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            int permission = ContextCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_FINE_LOCATION);
            return permission == PackageManager.PERMISSION_GRANTED;
        } else {
            return true;
        }
    }

    // Device scan callback.
    private ScanCallback leScanCallback =
            new ScanCallback() {
                @Override
                public void onScanResult(int callbackType, ScanResult result) {
                    super.onScanResult(callbackType, result);
                    leDeviceListAdapter.addDevice(result.getDevice());
                    leDeviceListAdapter.notifyDataSetChanged();


                }
            };


    private void scanLeDevice() {
        if (!scanning) {
            // Stops scanning after a predefined scan period.
            handler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    scanning = false;
                    if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
                        // TODO: Consider calling
                        //    ActivityCompat#requestPermissions
                        // here to request the missing permissions, and then overriding
                        //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                        //                                          int[] grantResults)
                        // to handle the case where the user grants the permission. See the documentation
                        // for ActivityCompat#requestPermissions for more details.
                        return;
                    }
                    bluetoothLeScanner.stopScan(leScanCallback);
                }
            }, SCAN_PERIOD);

            scanning = true;
            bluetoothLeScanner.startScan(leScanCallback);
        } else {
            scanning = false;
            //bluetoothLeScanner.stopScan(leScanCallback);
            scanLeDevice();
        }
    }

//    private void scanLeDevice() {
//        if (!scanning) {
//            // Stops scanning after a predefined scan period.
//            handler.postDelayed(new Runnable() {
//                @Override
//                public void run() {
//                    scanning = false;
//                    if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
//                        // TODO: Consider calling
//                        //    ActivityCompat#requestPermissions
//                        // here to request the missing permissions, and then overriding
//                        //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
//                        //                                          int[] grantResults)
//                        // to handle the case where the user grants the permission. See the documentation
//                        // for ActivityCompat#requestPermissions for more details.
//                        return;
//                    }
//                    bluetoothLeScanner.stopScan(leScanCallback);
//                }
//            }, SCAN_PERIOD);
//
//            scanning = true;
//            bluetoothLeScanner.startScan(leScanCallback);
//        }
//    }


    // Stop Bluetooth scan
    private void stopScanLeDevice() {
        if (scanning) {
            // Stops scanning if is scanning
            if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
                // TODO: Consider calling
                //    ActivityCompat#requestPermissions
                // here to request the missing permissions, and then overriding
                //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                //                                          int[] grantResults)
                // to handle the case where the user grants the permission. See the documentation
                // for ActivityCompat#requestPermissions for more details.
                return;
            }
            bluetoothLeScanner.stopScan(leScanCallback);
            scanning = false;
        }
    }

//    private void scanLeDevice(final boolean enable) {
//        if (enable) {
//            // Stops scanning after a pre-defined scan period.
//            mHandler.postDelayed(new Runnable() {
//                @Override
//                public void run() {
//                    mScanning = false;
//                    if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
//                        // TODO: Consider calling
//                        //    ActivityCompat#requestPermissions
//                        // here to request the missing permissions, and then overriding
//                        //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
//                        //                                          int[] grantResults)
//                        // to handle the case where the user grants the permission. See the documentation
//                        // for ActivityCompat#requestPermissions for more details.
//                        return;
//                    }
//                    mBluetoothAdapter.stopLeScan(mLeScanCallback);
//
//                }
//            }, SCAN_PERIOD);
//            mScanning = true;
//            mBluetoothAdapter.startLeScan(mLeScanCallback);
//        } else {
//            mScanning = false;
//            mBluetoothAdapter.stopLeScan(mLeScanCallback);
//        }
//
//    }


    public View onCreateView(@NonNull LayoutInflater inflater,
                             ViewGroup container, Bundle savedInstanceState) {






        if (savedInstanceState != null) {
            savedState = savedInstanceState.getBundle(_FRAGMENT_STATE);
        }


//         = getFragmentManager().getFragment(savedInstanceState, "bluetoothFragment");

//        bluetoothViewModel =
//                new ViewModelProvider(this).get(BluetoothViewModel.class);
        bluetoothViewModel = new ViewModelProvider(requireActivity()).get(BluetoothViewModel.class);




       // bluetoothTextViewOn = (TextView) binding.textViewBluetoothOn;


//        LiveData<Float> da = bluetoothViewModel.getTempSensor();
//        float d = da.getValue();
//        bluetoothViewModel.setTempSensor(27);

        // create Broadcast channel
        createNotificationChannel();
        //getContext().registerReceiver(gattUpdateReceiver, new IntentFilter(BluetoothLeService.ACTION_GATT_SERVICES_DISCOVERED), Context.RECEIVER_NOT_EXPORTED);

//-----------------------------------------------------------------------------------------------------------------------------
//        // Initializes a Bluetooth adapter.  For API level 18 and above, get a reference to
//        // BluetoothAdapter through BluetoothManager.
//        final BluetoothManager bluetoothManager =
//                (BluetoothManager) getContext().getSystemService(Context.BLUETOOTH_SERVICE);
//        mBluetoothAdapter = bluetoothManager.getAdapter();
//
//
//        // Checks if Bluetooth is supported on the device.
//        if (mBluetoothAdapter == null) {
//            Toast.makeText(getContext(), R.string.ble_not_supported, Toast.LENGTH_SHORT).show();
//            getActivity().finish();
////            return;
//        }
//        bluetoothLeScanner = mBluetoothAdapter.getBluetoothLeScanner();

        //--------------------------------------------------------------------------------------------------------------------------------------
        layoutInflater = inflater;

//        WordViewModel = new ViewModelProvider(this, ViewModelProvider.AndroidViewModelFactory.getInstance(getApplication())).get(WordViewModel.class);


        binding = FragmentBluetoothBinding.inflate(inflater, container, false);
        View root = binding.getRoot();


        // Check Bluetooth Switch
        bluetoothSwitch = (Switch) binding.switchBluetoothOn;


//        bluetoothSwitch.setOnClickListener(new View.OnClickListener() {
//
//            public void onClick (CompoundButton buttonView, boolean isChecked) {
////                Toast.makeText(root.getContext(), "Bluetooth was changed", Toast.LENGTH_SHORT).show();
//                if (isChecked == true) {
//// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
////                    // Use this check to determine whether BLE is supported on the device.  Then you can
////                    // selectively disable BLE-related features.
////                    if (!getActivity().getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
////                        Toast.makeText(getContext(), R.string.ble_not_supported, Toast.LENGTH_SHORT).show();
////                        getActivity().finish();
////                    }
//
//
////-------------------------------------------------------------
//
//
////                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
////                        requestPermissions(new String[]{Manifest.permission.ACCESS_COARSE_LOCATION}, PERMISSION_REQUEST_COARSE_LOCATION);
////                    }
//
//// ----------------------------------------------------------------------------------------
//
//                    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//                    //  Check permissions for Bluetooth Connect
//                    if (ContextCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_DENIED) {
//                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
//                            ActivityCompat.requestPermissions(getActivity(), new String[]{Manifest.permission.BLUETOOTH_CONNECT}, 2);
//                            return;
//                        }
//                    }
//                    // Request Scan permission
//                    ActivityCompat.requestPermissions(getActivity(), new String[]{Manifest.permission.BLUETOOTH_SCAN}, REQUEST_CODE_BLUETOOTH_SCAN);
//
//
////                    if (ContextCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_SCAN) == PackageManager.PERMISSION_DENIED)
////                    {
////                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S)
////                        {
////                            ActivityCompat.requestPermissions(getActivity(), new String[]{Manifest.permission.BLUETOOTH_SCAN}, 2);
////                            return;
////                        }
////                    }
//
//
////                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
////                        if (!EasyPermissions.hasPermissions(getContext(), BLUETOOTH_PERMISSIONS_S)) {
////                            EasyPermissions.requestPermissions(this, message, yourRequestCode,BLUETOOTH_PERMISSIONS_S);
////                        }
////                    }
//
//
////                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
////                        int permission = ContextCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_SCAN);
////                    }
//                    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
////                    boolean fineLocation = checkPermission();
//
//                    bluetoothTextViewOn = (TextView) binding.textViewBluetoothOn;
//
////                    // Initializes a Bluetooth adapter.  For API level 18 and above, get a reference to
////                    // BluetoothAdapter through BluetoothManager.
////                    final BluetoothManager bluetoothManager =
////                            (BluetoothManager) getContext().getSystemService(Context.BLUETOOTH_SERVICE);
////                    mBluetoothAdapter = bluetoothManager.getAdapter();
////
////
////                    // Checks if Bluetooth is supported on the device.
////                    if (mBluetoothAdapter == null) {
////                        Toast.makeText(getContext(), R.string.ble_not_supported, Toast.LENGTH_SHORT).show();
////                        getActivity().finish();
//////            return;
////                    }
//
//                    //bluetoothLeScanner = mBluetoothAdapter.getBluetoothLeScanner();
//
//
//
//                    bluetoothTextViewOn.setText("On");
//
//                    scanLeDevice();
//                    bluetoothViewModel.setBluetoothActive(true);
//                } else {
//
////                    TextView connectedDevice = binding.textViewBluetoothDeviceChosen;
//
////                    bluetoothViewModel.setDevice(null);
//
//
////                    bluetoothViewModel.getDevice().observe(getViewLifecycleOwner(), bluetoothDevice -> {
////                        textView.setText(bluetoothDevice.getName());
////                    });
//
//                    bluetoothViewModel.setBluetoothActive(false);
//
//                    BluetoothGatt bluetoothGatt = BluetoothLeService.getBluetoothGatt();
//
//                    if (bluetoothGatt == null) {
//                        bluetoothTextViewOn = (TextView) binding.textViewBluetoothOn;
//
//
//                        bluetoothTextViewOn.setText("Off");
//                        stopScanLeDevice();
//                        //bluetoothViewModel.deleteDevice();
//                        final TextView textView = binding.textViewBluetoothDeviceChosen;
//                        // Show bluetooth device name in view
//                        textView.setText("");
//
//
//                        leDeviceListAdapter.clear();
//                        leDeviceListAdapter.notifyDataSetChanged();
//
//                        return;
//                    } else {
//                        if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
//                            // TODO: Consider calling
//                            //    ActivityCompat#requestPermissions
//                            // here to request the missing permissions, and then overriding
//                            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
//                            //                                          int[] grantResults)
//                            // to handle the case where the user grants the permission. See the documentation
//                            // for ActivityCompat#requestPermissions for more details.
//                            return;
//                        }
//
//
//                        bluetoothTextViewOn = (TextView) binding.textViewBluetoothOn;
//
//
//                        bluetoothTextViewOn.setText("Off");
//                        stopScanLeDevice();
//                        //bluetoothViewModel.deleteDevice();
//                        final TextView textView = binding.textViewBluetoothDeviceChosen;
//                        // Show bluetooth device name in view
//                        textView.setText("");
//
//
//                        leDeviceListAdapter.clear();
//                        leDeviceListAdapter.notifyDataSetChanged();
//                        getContext().unbindService(serviceConnection);
//                    }
//                }
//            }
//        });

//---------------------------------------------------------------------------------------------------




//        bluetoothSwitch.setOnClickListener(new View.OnClickListener(){
//
//            @Override
//            public void onClick(View v) {
//                boolean enabled = v.isEnabled();
//                boolean activated = v.isActivated();
//                boolean clickble = v.isClickable();
//                boolean pressed = v.isPressed();
//                boolean selected = v.isSelected();
//
//
//
//                if (v.isEnabled()== true) {
//// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
////                    // Use this check to determine whether BLE is supported on the device.  Then you can
////                    // selectively disable BLE-related features.
////                    if (!getActivity().getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
////                        Toast.makeText(getContext(), R.string.ble_not_supported, Toast.LENGTH_SHORT).show();
////                        getActivity().finish();
////                    }
//
//
////-------------------------------------------------------------
//
//
////                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
////                        requestPermissions(new String[]{Manifest.permission.ACCESS_COARSE_LOCATION}, PERMISSION_REQUEST_COARSE_LOCATION);
////                    }
//
//// ----------------------------------------------------------------------------------------
//
//                    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//                    //  Check permissions for Bluetooth Connect
//                    if (ContextCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_DENIED) {
//                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
//                            ActivityCompat.requestPermissions(getActivity(), new String[]{Manifest.permission.BLUETOOTH_CONNECT}, 2);
//                            return;
//                        }
//                    }
//                    // Request Scan permission
//                    ActivityCompat.requestPermissions(getActivity(), new String[]{Manifest.permission.BLUETOOTH_SCAN}, REQUEST_CODE_BLUETOOTH_SCAN);
//
//
////                    if (ContextCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_SCAN) == PackageManager.PERMISSION_DENIED)
////                    {
////                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S)
////                        {
////                            ActivityCompat.requestPermissions(getActivity(), new String[]{Manifest.permission.BLUETOOTH_SCAN}, 2);
////                            return;
////                        }
////                    }
//
//
////                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
////                        if (!EasyPermissions.hasPermissions(getContext(), BLUETOOTH_PERMISSIONS_S)) {
////                            EasyPermissions.requestPermissions(this, message, yourRequestCode,BLUETOOTH_PERMISSIONS_S);
////                        }
////                    }
//
//
////                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
////                        int permission = ContextCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_SCAN);
////                    }
//                    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
////                    boolean fineLocation = checkPermission();
//
//                    bluetoothTextViewOn = (TextView) binding.textViewBluetoothOn;
//
////                    // Initializes a Bluetooth adapter.  For API level 18 and above, get a reference to
////                    // BluetoothAdapter through BluetoothManager.
////                    final BluetoothManager bluetoothManager =
////                            (BluetoothManager) getContext().getSystemService(Context.BLUETOOTH_SERVICE);
////                    mBluetoothAdapter = bluetoothManager.getAdapter();
////
////
////                    // Checks if Bluetooth is supported on the device.
////                    if (mBluetoothAdapter == null) {
////                        Toast.makeText(getContext(), R.string.ble_not_supported, Toast.LENGTH_SHORT).show();
////                        getActivity().finish();
//////            return;
////                    }
//
//                    //bluetoothLeScanner = mBluetoothAdapter.getBluetoothLeScanner();
//
//
//                    bluetoothTextViewOn.setText("On");
//
//                    scanLeDevice();
//                    bluetoothViewModel.setBluetoothActive(true);
//                } else {
//
////                    TextView connectedDevice = binding.textViewBluetoothDeviceChosen;
//
////                    bluetoothViewModel.setDevice(null);
//
//
////                    bluetoothViewModel.getDevice().observe(getViewLifecycleOwner(), bluetoothDevice -> {
////                        textView.setText(bluetoothDevice.getName());
////                    });
//
//                    bluetoothViewModel.setBluetoothActive(false);
//
//                    BluetoothGatt bluetoothGatt = BluetoothLeService.getBluetoothGatt();
//
//                    if (bluetoothGatt == null) {
//                        bluetoothTextViewOn = (TextView) binding.textViewBluetoothOn;
//
//
//                        bluetoothTextViewOn.setText("Off");
//                        stopScanLeDevice();
//                        //bluetoothViewModel.deleteDevice();
//                        final TextView textView = binding.textViewBluetoothDeviceChosen;
//                        // Show bluetooth device name in view
//                        textView.setText("");
//
//
//                        leDeviceListAdapter.clear();
//                        leDeviceListAdapter.notifyDataSetChanged();
//
//                        return;
//                    } else {
//                        if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
//                            // TODO: Consider calling
//                            //    ActivityCompat#requestPermissions
//                            // here to request the missing permissions, and then overriding
//                            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
//                            //                                          int[] grantResults)
//                            // to handle the case where the user grants the permission. See the documentation
//                            // for ActivityCompat#requestPermissions for more details.
//                            return;
//                        }
//
//
//                        bluetoothTextViewOn = (TextView) binding.textViewBluetoothOn;
//
//
//                        bluetoothTextViewOn.setText("Off");
//                        stopScanLeDevice();
//                        //bluetoothViewModel.deleteDevice();
//                        final TextView textView = binding.textViewBluetoothDeviceChosen;
//                        // Show bluetooth device name in view
//                        textView.setText("");
//
//
//                        leDeviceListAdapter.clear();
//                        leDeviceListAdapter.notifyDataSetChanged();
//                        getContext().unbindService(serviceConnection);
//                    }
//                }
//            }
//        });





        bluetoothSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {


                boolean  keydown = buttonView.isContextClickable();


//                Toast.makeText(root.getContext(), "Bluetooth was changed", Toast.LENGTH_SHORT).show();
                    if ((isChecked == true)&&(bluetoothToggleON!=true)) {
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//                    // Use this check to determine whether BLE is supported on the device.  Then you can
//                    // selectively disable BLE-related features.
//                    if (!getActivity().getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
//                        Toast.makeText(getContext(), R.string.ble_not_supported, Toast.LENGTH_SHORT).show();
//                        getActivity().finish();
//                    }


//-------------------------------------------------------------


//                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
//                        requestPermissions(new String[]{Manifest.permission.ACCESS_COARSE_LOCATION}, PERMISSION_REQUEST_COARSE_LOCATION);
//                    }

// ----------------------------------------------------------------------------------------

                        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                        //  Check permissions for Bluetooth Connect
                        if (ContextCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_DENIED) {
                            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                                ActivityCompat.requestPermissions(getActivity(), new String[]{Manifest.permission.BLUETOOTH_CONNECT}, 2);
                                return;
                            }
                        }
                        // Request Scan permission
                        ActivityCompat.requestPermissions(getActivity(), new String[]{Manifest.permission.BLUETOOTH_SCAN}, REQUEST_CODE_BLUETOOTH_SCAN);


//                    if (ContextCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_SCAN) == PackageManager.PERMISSION_DENIED)
//                    {
//                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S)
//                        {
//                            ActivityCompat.requestPermissions(getActivity(), new String[]{Manifest.permission.BLUETOOTH_SCAN}, 2);
//                            return;
//                        }
//                    }


//                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
//                        if (!EasyPermissions.hasPermissions(getContext(), BLUETOOTH_PERMISSIONS_S)) {
//                            EasyPermissions.requestPermissions(this, message, yourRequestCode,BLUETOOTH_PERMISSIONS_S);
//                        }
//                    }


//                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
//                        int permission = ContextCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_SCAN);
//                    }
                        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//                    boolean fineLocation = checkPermission();

                        bluetoothTextViewOn = (TextView) binding.textViewBluetoothOn;

                        // Initializes a Bluetooth adapter.  For API level 18 and above, get a reference to
                        // BluetoothAdapter through BluetoothManager.
                        final BluetoothManager bluetoothManager =
                                (BluetoothManager) getContext().getSystemService(Context.BLUETOOTH_SERVICE);
                        mBluetoothAdapter = bluetoothManager.getAdapter();


                        // Checks if Bluetooth is supported on the device.
                        if (mBluetoothAdapter == null) {
                            Toast.makeText(getContext(), R.string.ble_not_supported, Toast.LENGTH_SHORT).show();
                            getActivity().finish();
//            return;
                        }
                        bluetoothLeScanner = mBluetoothAdapter.getBluetoothLeScanner();


                        bluetoothTextViewOn.setText("On");

                        scanLeDevice();
                        bluetoothViewModel.setBluetoothActive(true);
                    } else if (isChecked == false){

//                    TextView connectedDevice = binding.textViewBluetoothDeviceChosen;

//                    bluetoothViewModel.setDevice(null);


//                    bluetoothViewModel.getDevice().observe(getViewLifecycleOwner(), bluetoothDevice -> {
//                        textView.setText(bluetoothDevice.getName());
//                    });

                        bluetoothViewModel.setBluetoothActive(false);

                        BluetoothGatt bluetoothGatt = BluetoothLeService.getBluetoothGatt();

                        if (bluetoothGatt == null) {
                            bluetoothTextViewOn = (TextView) binding.textViewBluetoothOn;


                            bluetoothTextViewOn.setText("Off");
                            stopScanLeDevice();
                            //bluetoothViewModel.deleteDevice();
                            final TextView textView = binding.textViewBluetoothDeviceChosen;
                            // Show bluetooth device name in view
                            textView.setText("");


                            leDeviceListAdapter.clear();
                            leDeviceListAdapter.notifyDataSetChanged();

                            return;
                        } else {
                            if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                                // TODO: Consider calling
                                //    ActivityCompat#requestPermissions
                                // here to request the missing permissions, and then overriding
                                //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                                //                                          int[] grantResults)
                                // to handle the case where the user grants the permission. See the documentation
                                // for ActivityCompat#requestPermissions for more details.
                                return;
                            }


                            bluetoothTextViewOn = (TextView) binding.textViewBluetoothOn;


                            bluetoothTextViewOn.setText("Off");
                            stopScanLeDevice();
                            //bluetoothViewModel.deleteDevice();
                            final TextView textView = binding.textViewBluetoothDeviceChosen;
                            // Show bluetooth device name in view
                            textView.setText("");


                            leDeviceListAdapter.clear();
                            leDeviceListAdapter.notifyDataSetChanged();
                            if (bluetoothService == null) {
                                getContext().unbindService(bluetoothViewModel.getLastServiceConnection().getValue());
                                BluetoothLeService.mDeviceBusy = false;
                            }  else {
                                getContext().unbindService(serviceConnection);
                                BluetoothLeService.mDeviceBusy = false;
                            }
                        }
                    }
                    bluetoothToggleON = false;

            }
        });



        // Show List of Bluetooth scanned devices
        final ListView listView = (ListView) binding.listBluetoothDeviceView;


//        ArrayAdapter<String> adapter = new ArrayAdapter<String>
//                (this.getActivity(), R.layout.item_view, R.id.itemTextView, deviceArray);


//                ArrayAdapter<BluetoothDevice> adapter = new ArrayAdapter<BluetoothDevice>
//                (this.getActivity(), R.layout.item_view, R.id.itemTextView, deviceArray);


        leDeviceListAdapter = new LeDeviceListAdapter(layoutInflater);
        listView.setAdapter(leDeviceListAdapter);

        // Create Listener for the List
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                BluetoothDevice item = (BluetoothDevice) parent.getItemAtPosition(position);
                if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                    // TODO: Consider calling
                    //    ActivityCompat#requestPermissions
                    // here to request the missing permissions, and then overriding
                    //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                    //                                          int[] grantResults)
                    // to handle the case where the user grants the permission. See the documentation
                    // for ActivityCompat#requestPermissions for more details.
                    return;
                }


                bluetoothViewModel.setDevice(item);

                //  Initialize Bluetooth Sertvice
                gattServiceIntent = new Intent(getContext(), BluetoothLeService.class);
                getContext().bindService(gattServiceIntent, serviceConnection, Context.BIND_AUTO_CREATE);

//                //  Pass Bluetooth View Model to Service to update Bluetooth DATA
//                Bundle b = new Bundle();
//                b.putParcelable("data", bluetoothViewModel);
//                gattServiceIntent.putExtra("bluetoothData",b);

                final String deviceName = item.getName();

                //  Show Connected Bluetooth Device
                final TextView textView = binding.textViewBluetoothDeviceChosen;
                // Show bluetooth device name in view
                bluetoothViewModel.getDevice().observe(getViewLifecycleOwner(), bluetoothDevice -> {
                    textView.setText(bluetoothDevice.getName());
                });
                // Set device adress for bluetooth connection
                bluetoothViewModel.getDevice().observe(getViewLifecycleOwner(), bluetoothDevice -> {
                    deviceAddress = bluetoothDevice.getAddress();
                });

                leDeviceListAdapter.removeDevice(item);
                leDeviceListAdapter.notifyDataSetChanged();

                //hs selected
                stopScanLeDevice();
                handler.removeCallbacksAndMessages(null);  // Cancel postDelay


            }
        });



        return root;
    }


//    @Override
//    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
//        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
//        EasyPermissions.onRequestPermissionsResult(requestCode, permissions, grantResults, this);
//    }

//    @Override
//    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
//        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
//
//        if (requestCode == REQUEST_CODE_BLUETOOTH_SCAN) {
//            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
//                // The user has granted the permission.
//                // Start scanning for Bluetooth devices.
//            } else {
//                // The user has denied the permission.
//                // Display an error message.
//            }
//        }
//    }

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//    @Override
//    public void onAttach(@NonNull Context context) {
//        super.onAttach(context);
//        ActivityResultLauncher<String[]> locationPermissionRequest =
//                registerForActivityResult(new ActivityResultContracts
//                                .RequestMultiplePermissions(), result -> {
//                            Boolean fineLocationGranted = result.getOrDefault(
//                                    Manifest.permission.ACCESS_FINE_LOCATION, false);
//                            Boolean coarseLocationGranted = result.getOrDefault(
//                                    Manifest.permission.ACCESS_COARSE_LOCATION,false);
//                            if (fineLocationGranted != null && fineLocationGranted) {
//                                // Precise location access granted.
//                                Toast.makeText(getContext(), "Precise location granted" , Toast.LENGTH_SHORT).show();
//                            } else if (coarseLocationGranted != null && coarseLocationGranted) {
//                                // Only approximate location access granted.
//                                Toast.makeText(getContext(), "Aproximate location granted" , Toast.LENGTH_SHORT).show();
//                            } else {
//                                // No location access granted.
//                                Toast.makeText(getContext(), "No location granted" , Toast.LENGTH_SHORT).show();
//                            }
//                        }
//                );
//
//        locationPermissionRequest.launch(new String[] {
//                Manifest.permission.ACCESS_FINE_LOCATION,
//                Manifest.permission.ACCESS_COARSE_LOCATION
//        });
//    }


    private void createNotificationChannel() {
        // Create the NotificationChannel, but only on API 26+ because
        // the NotificationChannel class is not in the Support Library.
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            CharSequence name = getString(R.string.channel_name);
            String description = getString(R.string.channel_description);
            int importance = NotificationManager.IMPORTANCE_DEFAULT;
            NotificationChannel channel = new NotificationChannel(CHANNEL_ID, name, importance);
            channel.setDescription(description);
            // Register the channel with the system; you can't change the importance
            // or other notification behaviors after this.
            NotificationManager notificationManager = getContext().getSystemService(NotificationManager.class);
            notificationManager.createNotificationChannel(channel);
        }
    }






    @Override
    public void onDestroyView() {
        super.onDestroyView();
        binding = null;
    }


}