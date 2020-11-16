l#include <Arduino_LSM9DS1.h>
#include <Arduino_HTS221.h>
#include <ArduinoBLE.h>

// BLUETOOTH HYDROMETER
//
// we create the overall service being connected to by Central
//

BLEService hydrometerService("12981eff-fb45-4c47-828e-a321a6309963"); // service object to enable reading of the board's information

// we need to give the service some characteristics
//
// we will use 4: floatChar(temperature) and 3 floatChar(gyro values)
//

BLEFloatCharacteristic tempChar("2A15", BLERead | BLENotify | BLEWrite);
BLEFloatCharacteristic xChar("3B26", BLERead | BLENotify | BLEWrite);
BLEFloatCharacteristic yChar("4C37", BLERead | BLENotify | BLEWrite);
BLEFloatCharacteristic zChar("5D48", BLERead | BLENotify | BLEWrite);

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

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  //
  // establish local name and services of hydrometer so Android application
  // knows what to connect to 
  //
  BLE.setLocalName("Bluetooth Hyrometer!");
  //hydrometerService.setAdvertisedServiceUuid(hydrometerService.uuid()); // add the service UUID
  BLE.setAdvertisedServiceUuid(hydrometerService.uuid());
  // add characteristcs to the service
  hydrometerService.addCharacteristic(tempChar);
  hydrometerService.addCharacteristic(xChar);
  hydrometerService.addCharacteristic(yChar);
  hydrometerService.addCharacteristic(zChar);
  
  BLE.addService(hydrometerService); // Add the Bluetooth service

  // set the initial value for the characeristic:
  tempChar.writeValue(0.0);
  xChar.writeValue(0.0);
  yChar.writeValue(0.0);
  zChar.writeValue(0.0);
  
  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
  
  
}

void loop() {

  // creates object for central(Android app)
  BLEDevice central = BLE.central();
  
  float x, y, z;
  char byteArray[4]; // store values to send to Android

  float temperature;
  while (1) {
    temperature = HTS.readTemperature();
    Serial.print("Temp: ");
    Serial.println(temperature);
    delay(10000);
  }
  if (central) {

    Serial.println("Bluetooth Connection Successful!!");
  }
  
  /*
  // print each of the sensor values
  //Serial.print("Temperature = ");
  //Serial.print(temperature);
  //Serial.println(" °C");
  
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
  */
}
