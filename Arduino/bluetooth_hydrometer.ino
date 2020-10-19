#include <Arduino_LSM9DS1.h>
#include <Arduino_HTS221.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU Sensors!");
    while (1);
  }

   if (!HTS.begin()) {
   Serial.println("Failed to initialize humidity temperature sensor!");
   while (1);
  }
}

void loop() {
  float x, y, z;

  float temperature = HTS.readTemperature();
  float tempF = 0;
  tempF = (temperature*1.8) + 32;

  // print each of the sensor values
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.print(" °C");
  Serial.print(" \t(");
  Serial.print(tempF);
  Serial.println(" °F)");
  
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    Serial.print("X = \t");
    Serial.print(x);
    Serial.print('\t');
    Serial.print("Y = ");
    Serial.print(y);
    Serial.print('\t');
    Serial.print("Z = ");
    Serial.println(z);   
  }
  Serial.println();
  delay(1000);
  
}
