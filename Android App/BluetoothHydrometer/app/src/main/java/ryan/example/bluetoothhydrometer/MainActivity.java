package ryan.example.bluetoothhydrometer;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

public class MainActivity extends Activity {

    private static final String TAG = "MainActivity";

    // create BluetoothAdapter obj for Android
    BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

    // check to see if Bluetooth is turned on, if not we will turn on
    // to scan for our peripheral

    // Button obj for connecting to Arduino using Bluetooth
    Button buttonConnect = findViewById(R.id.buttonConnect);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Log.i(TAG, "onCreate");
        //Log.i(TAG, "button \"buttonConnect\" was clicked.");
        //
        //buttonConnect.setOnClickListener(new View.OnClickListener(){@Override public void onClick(View v) { Log.i(TAG, "button \"buttonConnect\" was clicked."); }});
    }

    public void openBluetooth(View v) {
        //Intent i = new Intent(this, Activity.class);
        //startActivity(i);
        Log.i(TAG, "button \"buttonConnect\" was clicked.");

        // it would seem I have to disable button
    }

}