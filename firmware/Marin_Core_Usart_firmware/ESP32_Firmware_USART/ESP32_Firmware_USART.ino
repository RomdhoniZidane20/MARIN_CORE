#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

// Pinout for Switch
#define SW_Start                 2
#define SW_Stop                  3

// Pinout for LED
// #define LED_Red_enable           12
// #define LED_Green_enable         13
// #define LED_Blue_enable          14

// IMU variables
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
// uint16_t BNO055_SAMPLERATE_DELAY_MS = 10;

// Input and Sensors Variables
float Yaw, Pitch, Roll;
float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;
float start_switch, stop_switch;

// Buffer Variables
char yawBuffer[4], pitchBuffer[4], rollBuffer[4]; 
char startBuffer[4], stopBuffer[4];
char accelxBuffer[4], accelyBuffer[4], accelzBuffer[4];
char gyroxBuffer[4],gyroyBuffer[4],gyrozBuffer[4];

void setup() {
    
  Serial.begin(115200);
  bno.begin(bno.OPERATION_MODE_IMUPLUS);

  pinMode(SW_Start, INPUT);
  pinMode(SW_Stop, INPUT);

  // pinMode(LED_Red_enable, OUTPUT);
  // pinMode(LED_Green_enable, OUTPUT);
  // pinMode(LED_Blue_enable, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  imu::Vector<3> Orientation = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  imu::Vector<3> Accel       = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  imu::Vector<3> Gyro        = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);

  

  Yaw   = Orientation.x();
  Roll  = Orientation.y();
  Pitch = Orientation.z();

  accelX = (Accel.y() * 5/2) + 512;
  accelY = (Accel.x() * 5/2) + 512;
  accelZ = (Accel.z() * 5/2) + 512;

  gyroX = (Gyro.y() * (-1)) + 512;
  gyroY = (Gyro.x() * (-1)) + 512;
  gyroZ = Gyro.z() + 512;

  start_switch = digitalRead(SW_Start);
  stop_switch  = digitalRead(SW_Stop);

  uint8_t system, gyro, accel = 0;
  bno.Calibration_no_mag(&system, &gyro, &accel);
  
  // Stored buffer
  yawBuffer[0]    = (*(long int*)&Yaw) & 0xFF;
  yawBuffer[1]    = ((*(long int*)&Yaw) >> 8) & 0xFF;
  yawBuffer[2]    = ((*(long int*)&Yaw) >> 16) & 0xFF;
  yawBuffer[3]    = ((*(long int*)&Yaw) >> 24) & 0xFF;

  rollBuffer[0]   = (*(long int*)&Roll) & 0xFF;
  rollBuffer[1]   = ((*(long int*)&Roll) >> 8) & 0xFF;
  rollBuffer[2]   = ((*(long int*)&Roll) >> 16) & 0xFF;
  rollBuffer[3]   = ((*(long int*)&Roll) >> 24) & 0xFF;

  pitchBuffer[0]  = (*(long int*)&Pitch) & 0xFF;
  pitchBuffer[1]  = ((*(long int*)&Pitch) >> 8) & 0xFF;
  pitchBuffer[2]  = ((*(long int*)&Pitch) >> 16) & 0xFF;
  pitchBuffer[3]  = ((*(long int*)&Pitch) >> 24) & 0xFF;

  startBuffer[0]  = (*(long int*)&start_switch) & 0xFF;
  startBuffer[1]  = ((*(long int*)&start_switch) >> 8) & 0xFF;
  startBuffer[2]  = ((*(long int*)&start_switch) >> 16) & 0xFF;
  startBuffer[3]  = ((*(long int*)&start_switch) >> 24) & 0xFF;
  
  stopBuffer[0]  = (*(long int*)&stop_switch) & 0xFF;
  stopBuffer[1]  = ((*(long int*)&stop_switch) >> 8) & 0xFF;
  stopBuffer[2]  = ((*(long int*)&stop_switch) >> 16) & 0xFF;
  stopBuffer[3]  = ((*(long int*)&stop_switch) >> 24) & 0xFF;
  
  accelxBuffer[0] = (*(long int*)&accelX) & 0xFF;
  accelxBuffer[1] = ((*(long int*)&accelX) >> 8) & 0xFF;
  accelxBuffer[2] = ((*(long int*)&accelX) >> 16) & 0xFF;
  accelxBuffer[3] = ((*(long int*)&accelX) >> 24) & 0xFF;

  accelyBuffer[0] = (*(long int*)&accelY) & 0xFF;
  accelyBuffer[1] = ((*(long int*)&accelY) >> 8) & 0xFF;
  accelyBuffer[2] = ((*(long int*)&accelY) >> 16) & 0xFF;
  accelyBuffer[3] = ((*(long int*)&accelY) >> 24) & 0xFF;

  accelzBuffer[0] = (*(long int*)&accelZ)& 0xFF;
  accelzBuffer[1] = ((*(long int*)&accelZ) >> 8) & 0xFF;
  accelzBuffer[2] = ((*(long int*)&accelZ) >> 16) & 0xFF;
  accelzBuffer[3] = ((*(long int*)&accelZ) >> 24) & 0xFF;
 
  gyroxBuffer[0] = (*(long int*)&gyroX) & 0xFF;
  gyroxBuffer[1] = ((*(long int*)&gyroX) >> 8) & 0xFF;
  gyroxBuffer[2] = ((*(long int*)&accelX) >> 16) & 0xFF;
  gyroxBuffer[3] = ((*(long int*)&accelX) >> 24) & 0xFF;

  gyroyBuffer[0] = (*(long int*)&gyroY) & 0xFF;
  gyroyBuffer[1] = ((*(long int*)&gyroY) >> 8) & 0xFF;
  gyroyBuffer[2] = ((*(long int*)&accelY) >> 16) & 0xFF;
  gyroyBuffer[3] = ((*(long int*)&accelY) >> 24) & 0xFF;

  gyrozBuffer[0] = (*(long int*)&gyroZ) & 0xFF;
  gyrozBuffer[1] = ((*(long int*)&gyroZ) >> 8) & 0xFF;
  gyrozBuffer[2] = ((*(long int*)&accelZ) >> 16) & 0xFF;
  gyrozBuffer[3] = ((*(long int*)&accelZ) >> 24) & 0xFF; 
  
  // Send to Master
  Serial.write('i');
  Serial.write('t');
  Serial.write('s');
  Serial.write(yawBuffer[0]);
  Serial.write(yawBuffer[1]);
  Serial.write(yawBuffer[2]);
  Serial.write(yawBuffer[3]);
  Serial.write(':');
  Serial.write(rollBuffer[0]);
  Serial.write(rollBuffer[1]);
  Serial.write(rollBuffer[2]);
  Serial.write(rollBuffer[3]);
  Serial.write(':');
  Serial.write(pitchBuffer[0]);
  Serial.write(pitchBuffer[1]);
  Serial.write(pitchBuffer[2]);
  Serial.write(pitchBuffer[3]);
  Serial.write(':');
  Serial.write(startBuffer[0]);
  Serial.write(startBuffer[1]);
  Serial.write(startBuffer[2]);
  Serial.write(startBuffer[3]);
  Serial.write(':');
  Serial.write(stopBuffer[0]);
  Serial.write(stopBuffer[1]);
  Serial.write(stopBuffer[2]);
  Serial.write(stopBuffer[3]);
  Serial.write(':');
  Serial.write(accelxBuffer[0]);
  Serial.write(accelxBuffer[1]);
  Serial.write(accelxBuffer[2]);
  Serial.write(accelxBuffer[3]);
  Serial.write(':');
  Serial.write(accelyBuffer[0]);
  Serial.write(accelyBuffer[1]);
  Serial.write(accelyBuffer[2]);
  Serial.write(accelyBuffer[3]);
  Serial.write(':');
  Serial.write(accelzBuffer[0]);
  Serial.write(accelyBuffer[1]);
  Serial.write(accelyBuffer[2]);
  Serial.write(accelyBuffer[3]);
  Serial.write(':');
  Serial.write(gyroxBuffer[0]);
  Serial.write(gyroxBuffer[1]);
  Serial.write(gyroxBuffer[2]);
  Serial.write(gyroxBuffer[3]);
  Serial.write(':');
  Serial.write(gyroyBuffer[0]);
  Serial.write(gyroyBuffer[1]);
  Serial.write(gyroyBuffer[2]);
  Serial.write(gyroyBuffer[3]);
  Serial.write(':');
  Serial.write(gyrozBuffer[0]);
  Serial.write(gyrozBuffer[1]);
  Serial.write(gyrozBuffer[2]);
  Serial.write(gyrozBuffer[3]);

//  // Debug Imu 
//  Serial.print("Imu Test \n");
//  Serial.print("Yaw: ");
//  Serial.print(Yaw);
//  Serial.print("\t Roll: ");
//  Serial.print(Roll);
//  Serial.print("\t Pitch: ");
//  Serial.print(Pitch);
//
//  // Debug Push button
//  Serial.print("\t Push Button Test");
//  Serial.print("Start: ");
//  Serial.print(start_switch);
//  Serial.print("\t Stop: ");
//  Serial.println(stop_switch);

}
