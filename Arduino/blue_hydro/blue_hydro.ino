#include <Arduino_LSM9DS1.h>
#include <Arduino_HTS221.h>
#include <ArduinoBLE.h>

#define RED 22     //pin mode for red led
#define BLUE 24     //pin mode for blue led
#define GREEN 23    //pin mode for green led
#define LED_PWR 25

#define GYRO_SENS 245.0
#define M_PI 3.14159265359
#define TEMP_OFFSET -8

BLEService hydrometerService("FCFC"); // Bluetooth service object to enable reading of the board's information

// Hydrometer characteristics - temperautre, pitch, roll and yaw 
BLEUnsignedCharCharacteristic tempChar("2A15", BLERead | BLENotify);
BLEUnsignedCharCharacteristic pitchChar("3B26", BLERead | BLENotify);
BLEUnsignedCharCharacteristic rollChar("4C37", BLERead | BLENotify);
BLEUnsignedCharCharacteristic yawChar("5D48", BLERead | BLENotify);

void printTempReadings(float tempC, float tempF);
void printAccelReadings(float x, float y, float z);
void printGyroReadings(float x, float y, float z);
void calculateTilt(float gyro[3], float acc[3]);
void checkLEDOutput(float tempF, float pitch, float yaw);

unsigned long time_old, time_delta;
float pitch = 0, roll = 0, yaw = 0;
float pitch_acc = 0, roll_acc = 0, yaw_acc = 0;

void setup() {
  Serial.begin(9600);
  //while(!Serial);
  //Serial.println("Started");

  if(!IMU.begin()) {
    Serial.println("Failed to initialize IMU Sensors!");
    while(1);
  }

  if(!HTS.begin()) {
    Serial.println("Failed to initialize humidity temperature sensor!");
    while(1);
  }

  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  //Local name and service of hydrometer 
  //Shows up during connection
  BLE.setLocalName("Bluetooth Hyrometer!");

  //add the hydrometer service
  BLE.setAdvertisedService(hydrometerService);
  // add characteristcs to the service
  hydrometerService.addCharacteristic(tempChar);
  hydrometerService.addCharacteristic(pitchChar);
  hydrometerService.addCharacteristic(rollChar);
  hydrometerService.addCharacteristic(yawChar);

  BLE.addService(hydrometerService); // Add the Bluetooth service

  // set the initial value for the characeristic:
  tempChar.writeValue(0);
  pitchChar.writeValue(0);
  rollChar.writeValue(0);
  yawChar.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active\n");
  
  // Turning off all LEDS except POWER
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(LED_PWR, OUTPUT);

  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);
  digitalWrite(BLUE, HIGH);
  digitalWrite(LED_PWR, HIGH);

  time_old = millis();
}

// theta = angular velocity * change in time
// use complementary filter for the low-pass filter for accel data
// integrate filtered accel data with gyro data to get more accurate tilt result
// DO WE EVEN NEED THE FILTER?????

/* Calculates the tilt angle of 3 axes
 *    
 * Parameters:
 *    gyro[3] = x, y, z values of gyroscope
 *    acc[3] = x, y, z values of accelerometer
 */
void calculateTilt(float gyro[3], float acc[3]) {
  // Calculating tilt angle by arc-tangent and accelerometer data
  pitch_acc = -atan2f(acc[0], acc[2]) * 180 / M_PI;
  roll_acc = -atan2f(acc[2], acc[1]) * 180 / M_PI; // Not used, but still noted
  yaw_acc = -atan2f(acc[1], acc[2]) * 180 / M_PI;

  pitchChar.writeValue(pitch_acc);
  rollChar.writeValue(roll_acc);
  yawChar.writeValue(yaw_acc);
  
  Serial.println("THETA ANGLES --------------");
  Serial.print("Pitch = ");
  Serial.print(pitch_acc);
  Serial.print("\tRoll = ");
  Serial.print(roll_acc); // Has no calibration to it
  Serial.print("\tYaw = ");
  Serial.println(yaw_acc);
  
  /* Calculating angle from angular velocity formula
  pitch += (gyro[1]) * time_delta;
  roll += gyro[2] * time_delta;
  yaw += (gyro[0]) * time_delta;
  */
  
  /* For Complementary Filter use
  pitch = (pitch * 0.98) + (pitch_acc * 0.02);
  roll = (roll * 0.98) + (roll_acc * 0.02);
  yaw = (yaw * 0.98) + (yaw_acc * 0.02);
  */

  /*
  Serial.println("WITH FILTERING --------------");
  Serial.print("Pitch = ");
  Serial.print(pitch);
  Serial.print("\tRoll = ");
  Serial.print(roll);
  Serial.print("\tYaw = ");
  Serial.println(yaw);
  */
}

void loop() {

  // creates object for central(Android app)
    BLEDevice central = BLE.central();

    if(!central)
    {
      Serial.println("Not connected");
    }
    
    digitalWrite(GREEN, LOW);
  
    float acc_x, acc_y, acc_z, acc[3];
    float gyro_x, gyro_y, gyro_z, gyro[3];
    float tempC, tempF;
  
    time_delta = (millis() - time_old) / 1000.0; // change in time from last iteration (seconds)

    // Fetch and print temperature values
    tempC = HTS.readTemperature() + TEMP_OFFSET;
    tempF = tempC * 1.8 + 32;
    printTempReadings(tempC, tempF);
    tempChar.writeValue(tempC);
    
    // Fetch and print accelerometer values
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(acc_x, acc_y, acc_z);
      acc[0] = acc_x;
      acc[1] = acc_y;
      acc[2] = acc_z;
      //printAccelReadings(acc_x, acc_y, acc_z);
    }

    // Fetch and print gyroscope values
    if (IMU.gyroscopeAvailable()) {
      IMU.readGyroscope(gyro_x, gyro_y, gyro_z);
      gyro[0] = gyro_x;
      gyro[1] = gyro_y;
      gyro[2] = gyro_z;
    //printGyroReadings(gyro_x, gyro_y, gyro_z);
    }

    // Calculate the tilt 
    calculateTilt(gyro, acc);

    // Makes the current time the new time for the next iteration
    time_old += time_delta;

    checkLEDOutput(tempF, pitch, yaw);
  
    Serial.println();
    delay(500);
  
}

/* Flashes RGB LED to show the status of the brew (for beer)
 *    Red = neither temp or tilt is ready
 *    Blue = temp is okay
 *    Green = tilt is okay
 *    
 * Parameters:
 *    tempF = temperature in Fahrenheit
 *    pitch = front and back tilt
 *    yaw = side to side tilt (most inaccurate)
 */
void checkLEDOutput(float tempF, float pitch, float yaw) {
  // Checks if temp is in the acceptable range
  if(tempF >= 68.0 && tempF <= 72.0) {
    digitalWrite(BLUE, LOW);
    digitalWrite(GREEN, HIGH);
  }
  else {
    digitalWrite(BLUE, HIGH);
  }

  // Checks if pitch and yaw angles are in the acceptable ranges
  // 90 degrees pitch -> pointing up
  // 0 degrees yaw -> pointing up
  if(pitch_acc >= 50.0 && pitch_acc <= 140.0 && yaw_acc >= -55.0 && yaw_acc <= 55.0) {
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
  }
  else {
    digitalWrite(RED, HIGH);
  }

  // Turn red LED back on if neither conditions are satisfied
  if(digitalRead(BLUE) == HIGH && digitalRead(RED) == HIGH) {
    digitalWrite(GREEN, LOW);
  }
}

// Prints temperature in C and F, read accelerometer values, and read gyroscope values
void printTempReadings(float tempC, float tempF) {
  Serial.println("TEMPERATURE ------------------------");
  Serial.print(tempC);
  Serial.print("°C = ");
  Serial.print(tempF);
  Serial.print("°F");
  Serial.println('\n');
}
void printAccelReadings(float x, float y, float z) {
  Serial.println("ACCELEROMETER ----------------------");
  Serial.print("x = ");
  Serial.print(x);
  Serial.print(",\ty = ");
  Serial.print(y);
  Serial.print(",\tz = ");
  Serial.print(z);
  Serial.println('\n');
}
void printGyroReadings(float x, float y, float z) {
  Serial.println("GYROSCOPE --------------------------");
  Serial.print("x = ");
  Serial.print(x);
  Serial.print(",\ty = ");
  Serial.print(y);
  Serial.print(",\tz = ");
  Serial.print(z);
  Serial.println('\n');
}
