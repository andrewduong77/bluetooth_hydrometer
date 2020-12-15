package ryan.example.bluetoothhydrometer;

import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.Charset;
import java.util.UUID;

public class BluetoothConnection {

    private static final String TAG = "Bluetooth Connection";
    private static final String appName = "BluetoothHydrometer";
    private static final UUID myUUID = UUID.fromString("ee042baa-fc9b-498b-94d0-96810b8b1c06");

    public final BluetoothAdapter mBluetoothAdapter;
    Context mContext;

    private AcceptThread mAcceptThread;

    private ConnectThread mConnectThread;
    private BluetoothDevice mmPeripheral;
    // 12981eff-fb45-4c47-828e-a321a6309963
    private UUID peripheralUUID = UUID.fromString("12981eff-fb45-4c47-828e-a321a6309963");
    ProgressDialog mProgressDialog;

    // manages the information being transferred between Peripheral and Central (application)
    private ConnectedThread mConnectedThread;

    public BluetoothConnection(Context c) {
        mContext = c;
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    }

    /*
        Thread for listening to possible connections
    */
    private class AcceptThread extends Thread {
        private final BluetoothServerSocket mmServerSocket;

        public AcceptThread() {
            BluetoothServerSocket tmp = null;

            try {
                tmp = mBluetoothAdapter.listenUsingInsecureRfcommWithServiceRecord(appName, myUUID);
                Log.d(TAG, "Setting up server...");
            } catch (IOException i) {
                Log.e(TAG, "IOException: " + i.getMessage());
            }

            mmServerSocket = tmp;
        }

        public void run() {
            Log.d(TAG, "Thread running...");

            BluetoothSocket socket = null;

            try {
                // sits and waits for a connection
                socket = mmServerSocket.accept();
                Log.d(TAG, "Connection Successful.");
            } catch (IOException i) {
                Log.e(TAG, "IOException: " + i.getMessage());
            }
            // talk about later
            if (socket != null) {
                // connected(socket, mmPeripheral);
            }
        }

        public void closeServerSocket() {
            try {
                mmServerSocket.close();
            } catch (IOException i) {
                Log.e(TAG, "Connection failed to close.");
            }
        }


    }

    private class ConnectThread extends Thread {
        private BluetoothSocket mmSocket;

        public ConnectThread(BluetoothDevice d, UUID uuid) {
            mmPeripheral = d;
            peripheralUUID = uuid;
        }

        public void run() {
            BluetoothSocket tmp = null;

            // establish a Bluetooth socket for a connection with
            // the given peripheral through Bluetooth
            try {
                tmp = mmPeripheral.createInsecureRfcommSocketToServiceRecord(peripheralUUID);
            } catch (IOException i) {
                Log.e(TAG, "Failed." + i.getMessage());
            }

            mmSocket = tmp;

            // once connection is made discovery mode must be disabled
            mBluetoothAdapter.cancelDiscovery();

            try {
                // connection to BluetoothSocket needs to be made
                mmSocket.connect();
                Log.d(TAG, "Connection Successful.");
            } catch (IOException i) {

                try {
                    mmSocket.close();
                    Log.d(TAG, "Socket closed.");
                } catch (IOException e) {
                    Log.e(TAG, "Failed." + e.getMessage());
                }

                Log.e(TAG, "Failed." + i.getMessage());
            }

            // call to connect socket and peripheral
            connected(mmSocket, mmPeripheral);
        }

        public void closePeripheralSocket() {
            try {
                Log.d(TAG, "Peripheral connection closed.");
                mmSocket.close();
            } catch (IOException i) {
                Log.e(TAG, "Connection failed to close.");
            }
        }
    }

    // start communication between Central and Peripheral
    //
    // start AcceptThread which is called by onResume
    //
    public synchronized void startCommunication() {

    if (mConnectThread != null) {
        mConnectThread.closePeripheralSocket();
        mConnectThread = null;
    }

    if (mAcceptThread == null) {
        mAcceptThread = new AcceptThread();
        mAcceptThread.start();
    }

    }

    // AcceptThread waits for a connection
    //
    // ConnectThread attempts to make a connection with peripheral's AcceptThread
    //
    public void startPeripheral(BluetoothDevice d, UUID uuid) {
        Log.d(TAG, "Peripheral starting");

        mProgressDialog = ProgressDialog.show(mContext, "Connecting Bluetooth",
                            "...", true);

        mConnectThread = new ConnectThread(d, uuid);
        mConnectThread.start();
    }


    private class ConnectedThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        // constructor for setting up ConnectedThread for Bluetooth Socket
        public ConnectedThread (BluetoothSocket socket) {

            mmSocket = socket;
            InputStream tmpInStream = null;
            OutputStream tmpOutStream = null;

            // once connection has been establish, the dialog box disappears
            mProgressDialog.dismiss();

            try {
                tmpInStream = mmSocket.getInputStream();
                tmpOutStream = mmSocket.getOutputStream();
            } catch (IOException i) {
                Log.e(TAG, "Failed to establish ability to collect data.");
            }

            mmInStream = tmpInStream;
            mmOutStream = tmpOutStream;
        }

        // establish functionality to thread
        public void run() {
            // stores data for transfer between devices
            byte[] buffer = new byte[1024];

            // amount of info returned
            int bytes;

            while (true) {

                try {
                    // reads input stream
                    bytes = mmInStream.read(buffer);
                    String msg = new String(buffer, 0, bytes);
                    Log.d(TAG, "Input Collected: " + msg);
                } catch (IOException i) {
                    Log.e(TAG, "Failed to receive/read input stream.");
                    break;
                }
            }
        }

        // I don't believe we will need to send data to Arduino
        // ...just in case
        //
        // will be called by MainActivity and used to send data to Arduino (if necessary)
        //
        public void write(byte[] b) {
            String text = new String(b, Charset.defaultCharset());
            Log.d(TAG, "Writing to OutputStream to send to Arduino...");

            try {
                mmOutStream.write(b);
            } catch (IOException i) {
                Log.e(TAG, "Failed to write to Arduino.");
            }
        }

        // will be called from MainActivity to disconnect
        //
        public void cancelConnection() {
            try {
                mmSocket.close();
            } catch (IOException i) {
                Log.e(TAG, "Failed to disconnect.");
            }
        }
    }
    //
    // end of ConnectedThread class
    //


    //
    // to manage connection, send output to Arduino and receive input from Arduino
    //
    // which is why we use "mmPeripheral" object as parameter
    //
    private void connected (BluetoothSocket mmSocket, BluetoothDevice mmPeripheral) {

        mConnectedThread = new ConnectedThread(mmSocket);
        mConnectedThread.start();
    }


    //
    // write to ConnectThread
    //
    public void write (byte[] outStream) {

        // temporary place holder
        ConnectedThread r;

        // sync copy of ConnectedThread
        Log.d(TAG, "Call to Write in ConnectedThread...");
        mConnectedThread.write(outStream);
    }
}
