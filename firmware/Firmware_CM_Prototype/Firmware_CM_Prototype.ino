#include <Dynamixel2Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

//IMU variables
uint16_t BNO055_SAMPLERATE_DELAY_MS = 0;
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

// Dynamixel Arduino define pin for CM
#define DXL_SERIAL               Serial
#define DEBUG_SERIAL             Serial2
const uint8_t DXL_DIR_PIN =      5;
const uint8_t DXL_ID      =      200; // ID Dynamixel

// Pinout for Switch
#define SW_Start                 40
#define SW_Stop                  44
#define Dxl_power                32

// Pinout for LED
#define LED_Red_enable           12
#define LED_Green_enable         13
#define LED_Blue_enable          14

//State
#define Return_Delay             100
volatile int start_button_state = 0;
volatile int stop_button_state  = 0;

//Dynamixel Arduino porthandler and packethandler
DYNAMIXEL::SerialPortHandler dxl_port(DXL_SERIAL, DXL_DIR_PIN);
const float DXL_PROTOCOL_VER_1_0 = 1.0;
const float DXL_PROTOCOL_VER_2_0 = 2.0;
const uint16_t DXL_MODEL_NUM     = 0x190; // OpenCM 9.04 model number
DYNAMIXEL::Slave dxl(dxl_port, DXL_MODEL_NUM);


//Addresses for register variable
uint16_t SW_Start_addr    = 56;
uint16_t SW_Stop_addr     = 60;
uint16_t dxl_enbale_addr  = 64;
uint16_t Yaw_addr         = 68;
uint16_t Pitch_addr       = 72;
uint16_t Roll_addr        = 76;
uint16_t AccelX_addr      = 80;
uint16_t AccelY_addr      = 84;
uint16_t AccelZ_addr      = 88;
uint16_t GyroX_addr       = 92;
uint16_t GyroY_addr       = 96;
uint16_t GyroZ_addr       = 100;
uint16_t LED_R_addr       = 104;
uint16_t LED_G_addr       = 108;
uint16_t LED_B_addr       = 112;
uint16_t Random_var       = 200;

// Stored variables for Interrupt
uint8_t Switch_Button[2];
uint8_t dxl_power_enable;

// Stored variable for imu
uint16_t ypr[3];
uint16_t accel[3];
uint16_t gyro[3];

// Store variable for LED
uint8_t led[3];

uint8_t Var = 0;
//This namespace is required to use Control table item names
using namespace ControlTableItem;


void setup() {

  //  pinMode(ledPin, OUTPUT);

  DEBUG_SERIAL.begin(115200);
  bno.begin();

  pinMode(SW_Start, INPUT);
  pinMode(SW_Stop, INPUT);
  pinMode(Dxl_power, OUTPUT);

  dxl_port.begin(1000000);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VER_1_0);
  dxl.setFirmwareVersion(1);
  dxl.setID(DXL_ID);

  dxl.addControlItem(SW_Start_addr, Switch_Button[0]);
  dxl.addControlItem(SW_Stop_addr, Switch_Button[1]);
  dxl.addControlItem(dxl_enbale_addr, dxl_power_enable);
  dxl.addControlItem(Yaw_addr, ypr[0]);
  dxl.addControlItem(Pitch_addr, ypr[1]);
  dxl.addControlItem(Roll_addr, ypr[2]);
  dxl.addControlItem(AccelX_addr, accel[0]);
  dxl.addControlItem(AccelY_addr, accel[1]);
  dxl.addControlItem(AccelZ_addr, accel[2]);
  dxl.addControlItem(GyroX_addr, gyro[0]);
  dxl.addControlItem(GyroY_addr, gyro[1]);
  dxl.addControlItem(GyroZ_addr, gyro[2]);
  dxl.addControlItem(LED_R_addr, led[0]);
  dxl.addControlItem(LED_G_addr, led[1]);
  dxl.addControlItem(LED_B_addr, led[2]);

  dxl.setReadCallbackFunc(read_callback_func);
  dxl.setWriteCallbackFunc(write_callback_func);

//  delay();

}

void loop() {

  if (dxl.processPacket() == false) {

    DEBUG_SERIAL.print("Last lib err code: ");
    DEBUG_SERIAL.print(dxl.getLastLibErrCode());
    DEBUG_SERIAL.print(", ");
    DEBUG_SERIAL.print("Last status packet err code: ");
    DEBUG_SERIAL.println(dxl.getLastStatusPacketError());
    DEBUG_SERIAL.println();
    
//    delayMicroseconds(Return_Delay);

  }

  imu::Vector<3> Orientation = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  imu::Vector<3> Accel       = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  imu::Vector<3> Gyro        = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);

  //  YPR value
  ypr[0]  = Orientation.x();
  ypr[1]  = Orientation.y() + 90;
  ypr[2]  = Orientation.z() + 180;

  //  Accelerometer Value
  accel[0]    = Accel.x();
  accel[1]    = Accel.y();
  accel[2]    = Accel.z();

  //  Gyro Value
  gyro[0]    = Gyro.x();
  gyro[1]    = Gyro.y();
  gyro[2]    = Gyro.z();

  uint8_t system, gyro, accel = 0;
  bno.Calibration_no_mag(&system, &gyro, &accel);

//  delayMicroseconds(Return_Delay);
}

void read_callback_func(uint16_t item_addr, uint8_t &dxl_err_code, void* arg)
{
  (void)dxl_err_code, (void)arg;

  if (item_addr == SW_Start_addr)
  {
    start_button_state = digitalRead(SW_Start);
    Switch_Button[0] = start_button_state;
  }

  if (item_addr == SW_Stop_addr)
  {
    stop_button_state = digitalRead(SW_Stop);
    Switch_Button[1] = stop_button_state;
  }

  if (item_addr == Yaw_addr)
  {
    ypr[0];
  }

  if (item_addr == Pitch_addr)
  {
    ypr[1];
  }

  if (item_addr == Roll_addr)
  {
    ypr[2];
  }

  if (item_addr == AccelX_addr)
  {
    accel[0];
  }

  if (item_addr == AccelY_addr)
  {
    accel[1];
  }

  if (item_addr == AccelZ_addr)
  {
    accel[2];
  }

  if (item_addr == GyroX_addr)
  {
    gyro[0];
  }

  if (item_addr == GyroY_addr)
  {
    gyro[1];
  }

  if (item_addr == GyroZ_addr)
  {
    gyro[2];
  }

}


void write_callback_func(uint16_t item_addr, uint8_t &dxl_err_code, void* arg)
{
  (void)dxl_err_code, (void)arg;

  if (item_addr == dxl_enbale_addr)
  {
    digitalWrite(Dxl_power, dxl_power_enable);
    Serial.print(dxl_power_enable);
  }

  if (item_addr == LED_R_addr)
  {
    digitalWrite(led[0], LED_Red_enable);
  }

  if (item_addr == LED_G_addr)
  {
    digitalWrite(led[1], LED_Green_enable);
  }

  if (item_addr == LED_B_addr)
  {
    digitalWrite(led[2], LED_Blue_enable);
  }
}
