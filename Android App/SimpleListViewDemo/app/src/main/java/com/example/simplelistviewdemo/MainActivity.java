package com.example.simplelistviewdemo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.text.DateFormatSymbols;

public class MainActivity extends AppCompatActivity implements AdapterView.OnItemClickListener {

    ListView lvMonth;
    String[] months;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        lvMonth = findViewById(R.id.lvMonth);
        months = new DateFormatSymbols().getMonths();
//        ArrayAdapter<String> monthAdapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, months);
        ArrayAdapter<String> monthAdapter = new ArrayAdapter<>(this, R.layout.list_item, months);
        lvMonth.setAdapter(monthAdapter);
        lvMonth.setOnItemClickListener(this);
    }

    @Override
    public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
//        String month = adapterView.getItemAtPosition(i).toString();
//        String month = months[i];
//        String month = ((TextView) view).getText().toString();
        String month = lvMonth.getItemAtPosition(i).toString();
        Toast.makeText(getApplicationContext(), "Clicked: "+ month, Toast.LENGTH_SHORT).show();
    }
}