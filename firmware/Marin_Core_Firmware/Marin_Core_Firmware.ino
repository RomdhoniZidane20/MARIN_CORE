#include <Dynamixel2Arduino.h>
#include <Wire.h>
#include <MadgwickAHRS.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

//IMU variables
uint16_t BNO055_SAMPLERATE_DELAY_MS = 0;
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
Madgwick filter;

// Dynamixel Arduino define pin for CM
#define DXL_SERIAL               Serial2
#define DEBUG_SERIAL             Serial
const uint8_t DXL_DIR_PIN =      5;
const uint8_t DXL_ID      =      190; // ID Dynamixel

// Pinout for Switch
#define SW_Start                 40
#define SW_Stop                  44
#define Dxl_power                32

// Pinout for LED
#define LED_Red_enable           12
#define LED_Green_enable         13
#define LED_Blue_enable          14

//State
#define Return_Delay             2
volatile int start_button_state = 0;
volatile int stop_button_state  = 0;

//Dynamixel Arduino porthandler and packethandler
DYNAMIXEL::SerialPortHandler dxl_port(DXL_SERIAL, DXL_DIR_PIN);
const float DXL_PROTOCOL_VER_1_0 = 1.0;
const float DXL_PROTOCOL_VER_2_0 = 2.0;
const uint16_t DXL_MODEL_NUM     = 0x190; // OpenCM 9.04 model number
DYNAMIXEL::Slave dxl(dxl_port, DXL_MODEL_NUM);


// Address for wirte register variable
uint8_t Reset_addr       = 55;
uint8_t dxl_enbale_addr  = 56;
uint8_t LED_R_addr       = 57;
uint8_t LED_G_addr       = 58;
uint8_t LED_B_addr       = 59;

// Addresses for read register variable
uint16_t SW_Start_addr    = 60;
uint16_t SW_Stop_addr     = 62;
uint16_t Yaw_addr         = 64;
uint16_t Pitch_addr       = 66;
uint16_t Roll_addr        = 68;
uint16_t AccelX_addr      = 70;
uint16_t AccelY_addr      = 72;
uint16_t AccelZ_addr      = 74;
uint16_t GyroX_addr       = 76;
uint16_t GyroY_addr       = 78;
uint16_t GyroZ_addr       = 80;

// Stored variables for Interrupt
uint8_t Switch_Button[2];
uint8_t dxl_power_enable;

// Stored variable for imu
uint16_t dxl_data[20];
//float dxl_data_f[12];
uint16_t accel[3];
uint16_t gyro[3];

// Store variable for LED
uint8_t led[3];

// Store Variables for random num

//This namespace is required to use Control table item names
using namespace ControlTableItem;


void setup() {

  //  pinMode(ledPin, OUTPUT);

  DEBUG_SERIAL.begin(115200);
  bno.begin(bno.OPERATION_MODE_IMUPLUS);

  pinMode(SW_Start, INPUT);
  pinMode(SW_Stop, INPUT);
  pinMode(Dxl_power, OUTPUT);
  pinMode(LED_Red_enable, OUTPUT);
  pinMode(LED_Green_enable, OUTPUT);
  pinMode(LED_Blue_enable, OUTPUT);


  dxl_port.begin(1000000);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VER_1_0);
  dxl.setFirmwareVersion(24);
  dxl.setID(DXL_ID);

  dxl.addControlItem(Reset_addr,      dxl_data[0]);
  dxl.addControlItem(dxl_enbale_addr, dxl_data[1]);
  dxl.addControlItem(LED_R_addr,      dxl_data[2]);
  dxl.addControlItem(LED_G_addr,      dxl_data[3]);
  dxl.addControlItem(LED_B_addr,      dxl_data[4]);
  dxl.addControlItem(SW_Start_addr,   dxl_data[5]);
  dxl.addControlItem(SW_Stop_addr,    dxl_data[6]);
  dxl.addControlItem(Yaw_addr,        dxl_data[7]);
  dxl.addControlItem(Pitch_addr,      dxl_data[8]);
  dxl.addControlItem(Roll_addr,       dxl_data[9]);
  dxl.addControlItem(AccelX_addr,     dxl_data[10]);
  dxl.addControlItem(AccelY_addr,     dxl_data[11]);
  dxl.addControlItem(AccelZ_addr,     dxl_data[12]);
  dxl.addControlItem(GyroX_addr,      dxl_data[13]);
  dxl.addControlItem(GyroY_addr,      dxl_data[14]);
  dxl.addControlItem(GyroZ_addr,      dxl_data[15]);



  dxl.setReadCallbackFunc(read_callback_func);
  dxl.setWriteCallbackFunc(write_callback_func);

  //    delay();

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
  imu::Vector<3> Accel       = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  imu::Vector<3> Gyro        = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);

  //  YPR value
  dxl_data[7]  = Orientation.x();
  dxl_data[8]  = Orientation.y() + 360;
  dxl_data[9]  = Orientation.z() + 360;

  //  dxl_data[5]  = random(200);

  //  Accelerometer Value
  dxl_data[10]    = Accel.x() + 100;
  dxl_data[11]    = Accel.y() + 100;
  dxl_data[12]    = Accel.z() + 100;

  //  Gyro Value
  dxl_data[13]    = Gyro.x() + 100;
  dxl_data[14]    = Gyro.y() + 100;
  dxl_data[15]    = Gyro.z() + 100;

  //  DEBUG_SERIAL.print("Accel X: ");
  //  DEBUG_SERIAL.print(dxl_data_f[0]);
  //    Serial.print("Accel Y: ");
  //    Serial.print("Accel Z: ");

  //  random_num = random(200);

  //  Debug

  //  Serial.println(random(200));


  uint8_t system, gyro, accel = 0;
  bno.Calibration_no_mag(&system, &gyro, &accel);

  //  delayMicroseconds(Return_Delay);
  //  delay(BNO055_SAMPLERATE_DELAY_MS);
}

void read_callback_func(uint16_t item_addr, uint8_t &dxl_err_code, void* arg)
{
  (void)dxl_err_code, (void)arg;

  if (item_addr == SW_Start_addr)
  {
    start_button_state = digitalRead(SW_Start);
    dxl_data[5] = start_button_state;
  }

  if (item_addr == SW_Stop_addr)
  {
    stop_button_state = digitalRead(SW_Stop);
    dxl_data[6] = stop_button_state;
  }

  if (item_addr == Yaw_addr)
  {
    dxl_data[7];
  }

  if (item_addr == Pitch_addr)
  {
    dxl_data[8];
  }

  if (item_addr == Roll_addr)
  {
    dxl_data[9];
  }

  if (item_addr == AccelX_addr)
  {
    dxl_data[10];
  }

  if (item_addr == AccelY_addr)
  {
    dxl_data[11];
  }

  if (item_addr == AccelZ_addr)
  {
    dxl_data[12];
  }

  if (item_addr == GyroX_addr)
  {
    dxl_data[13];
  }

  if (item_addr == GyroY_addr)
  {
    dxl_data[14];
  }

  if (item_addr == GyroZ_addr)
  {
    dxl_data[15];
  }

}


void write_callback_func(uint16_t item_addr, uint8_t &dxl_err_code, void* arg)
{
  (void)dxl_err_code, (void)arg;

  if (item_addr == Reset_addr)
  {
    if (dxl_data[0] == 1)
    {
      ESP.restart();
    }

  }
  if (item_addr == dxl_enbale_addr)
  {

    digitalWrite(Dxl_power, dxl_data[1]);
  }

  if (item_addr == LED_R_addr)
  {
    digitalWrite(LED_Red_enable, dxl_data[2]);
  }

  if (item_addr == LED_G_addr)
  {
    digitalWrite(LED_Green_enable, dxl_data[3]);
  }

  if (item_addr == LED_B_addr)
  {
    digitalWrite(LED_Blue_enable, dxl_data[4]);
  }
}
