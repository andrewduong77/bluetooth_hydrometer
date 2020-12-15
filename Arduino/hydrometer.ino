#include <Arduino_LSM9DS1.h>
#include <Arduino_HTS221.h>

#define RED 22     
#define BLUE 24     
#define GREEN 23
#define LED_PWR 25

#define GYRO_SENS 245.0
#define M_PI 3.14159265359
#define TEMP_OFFSET -8

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
  while(!Serial);
  Serial.println("Started");

  if(!IMU.begin()) {
    Serial.println("Failed to initialize IMU Sensors!");
    while(1);
  }

  if(!HTS.begin()) {
    Serial.println("Failed to initialize humidity temperature sensor!");
    while(1);
  }

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
  digitalWrite(RED, LOW);
  
  float acc_x, acc_y, acc_z, acc[3];
  float gyro_x, gyro_y, gyro_z, gyro[3];
  float tempC, tempF;
  
  time_delta = (millis() - time_old) / 1000.0; // change in time from last iteration (seconds)

  // Fetch and print temperature values
  tempC = HTS.readTemperature() + TEMP_OFFSET;
  tempF = tempC * 1.8 + 32;
  printTempReadings(tempC, tempF);

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
    digitalWrite(RED, HIGH);
  }
  else {
    digitalWrite(BLUE, HIGH);
  }

  // Checks if pitch and yaw angles are in the acceptable ranges
  // 90 degrees pitch -> pointing up
  // 0 degrees yaw -> pointing up
  if(pitch_acc >= 80.0 && pitch_acc <= 100.0 && yaw_acc >= -20.0 && yaw_acc <= 20.0) {
    digitalWrite(GREEN, LOW);
    digitalWrite(RED, HIGH);
  }
  else {
    digitalWrite(GREEN, HIGH);
  }

  // Turn red LED back on if neither conditions are satisfied
  if(digitalRead(BLUE) == HIGH && digitalRead(GREEN) == HIGH) {
    digitalWrite(RED, LOW);
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
