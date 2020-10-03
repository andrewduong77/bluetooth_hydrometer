package com.example.bluetoothhydrometer;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

import java.text.DateFormatSymbols;
import java.util.ArrayList;

public class MainActivity extends AppCompatActivity implements AdapterView.OnItemClickListener {

    ListView listViewTemperature;
//    String[] temperatures = {"36 °F at 1.047","42 °F at 1.049","48 °F at 1.051","54 °F at 1.053","60 °F at 1.055"};
    ArrayList<String> temperatures = new ArrayList<String>();

    void getTemperatures() {
        temperatures.add("36 °F at 1.047");
        temperatures.add("42 °F at 1.049");
        temperatures.add("48 °F at 1.051");
        temperatures.add("54 °F at 1.053");
        temperatures.add("60 °F at 1.055");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        listViewTemperature = findViewById(R.id.listViewTemperature);
        getTemperatures();
//        ArrayAdapter<String> temperatureAdapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, temperatures);
        ArrayAdapter<String> temperatureAdapter = new ArrayAdapter<>(this, R.layout.list_item, temperatures);
        listViewTemperature.setAdapter(temperatureAdapter);
        listViewTemperature.setOnItemClickListener(this);
    }

    @Override
    public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
//        String temperature = adapterView.getItemAtPosition(i).toString();
//        String temperature = temperatures[i];
//        String temperature = ((TextView) view).getText().toString();
        String temperature = listViewTemperature.getItemAtPosition(i).toString();
        Toast.makeText(getApplicationContext(), "Clicked: "+ temperature, Toast.LENGTH_SHORT).show();
    }
}