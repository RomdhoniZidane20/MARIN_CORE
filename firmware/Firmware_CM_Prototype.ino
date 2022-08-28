#include <Dynamixel2Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

//IMU variables
uint16_t BNO055_SAMPLERATE_DELAY_MS = 0;
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

// Dynamixel Arduino define pin for CM
#define DXL_SERIAL               Serial2
#define DEBUG_SERIAL             Serial
const uint8_t DXL_DIR_PIN =      5;
const uint8_t DXL_ID      =      200; // ID Dynamixel

// Pinout for Switch
#define SW_Start                 34
#define SW_Stop                  35
#define Dxl_power                32

//State
//bool dxl_power_is;
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

//Stored variables for Interrupt
uint8_t Switch_Button[2];
uint8_t dxl_power_enable;

//Stored variable for imu
double ypr[3];
double accel[3];
double gyro[3];

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
  //  dxl.addControlItem(LED_addr, Led_Esp)

  dxl.setReadCallbackFunc(read_callback_func);
  dxl.setWriteCallbackFunc(write_callback_func);

  delay(1000);

}

void loop() {

  if (dxl.processPacket() == false) {

    DEBUG_SERIAL.print("Last lib err code: ");
    DEBUG_SERIAL.print(dxl.getLastLibErrCode());
    DEBUG_SERIAL.print(", ");
    DEBUG_SERIAL.print("Last status packet err code: ");
    DEBUG_SERIAL.println(dxl.getLastStatusPacketError());
    DEBUG_SERIAL.println();

  }
  imu::Vector<3> Orientation = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  imu::Vector<3> Accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  imu::Vector<3> Gyro  = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  
  //  YPR value
  ypr[0]  = Orientation.x();
  ypr[1]  = Orientation.y();
  ypr[2]  = Orientation.z();

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

  // if (item_addr == Yaw_addr)
  // {
  //   ypr[0] = Orientation.x();
  // }

  // if (item_addr == Pitch_addr)
  // {
  //   ypr[1] = Orientation.y();
  // }

  // if (item_addr == Roll_addr)
  // {
  //   ypr[2] = Orientation.z();
  // }

  // if (item_addr == AccelX_addr)
  // {
  //   accel[0] = Accel.x();
  // }

  // if (item_addr == AccelY_addr)
  // {
  //   accel[1] = Accel.y();
  // }

  // if (item_addr == AccelZ_addr)
  // {
  //   accel[2] = Accel.z();
  // }

  // if (item_addr == GyroX_addr)
  // {
  //   gyro[0] = Gyro.x();
  // }

  // if (item_addr == GyroY_addr)
  // {
  //   gyro[1] = Gyro.y();
  // }

  // if (item_addr == GyroZ_addr)
  // {
  //   gyro[2] = Gyro.x();
  // }

  // uint8_t system, gyro, accel = 0;
  // bno.Calibration_no_mag(&system, &gyro, &accel);
}


void write_callback_func(uint16_t item_addr, uint8_t &dxl_err_code, void* arg)
{
  (void)dxl_err_code, (void)arg;

  if (item_addr == dxl_enbale_addr)
  {
    digitalWrite(Dxl_power, dxl_power_enable);
    Serial.print(dxl_power_enable);
  }
}
