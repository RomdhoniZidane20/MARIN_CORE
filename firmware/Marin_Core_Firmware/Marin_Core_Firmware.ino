#include <Dynamixel2Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

TaskHandle_t task_dxl, task_input;

void Task_DXL( void *pvParameters );
void Task_INPUT( void *pvParameters );

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

// Pinout for LED
#define LED_Red_enable           12
#define LED_Green_enable         13
#define LED_Blue_enable          14

//State
//#define Return_Delay             2
//volatile int start_button_state = 0;
//volatile int stop_button_state  = 0;

//Dynamixel Arduino porthandler and packethandler
DYNAMIXEL::SerialPortHandler dxl_port(DXL_SERIAL, DXL_DIR_PIN);
const float DXL_PROTOCOL_VER_1_0 = 1.0;
const float DXL_PROTOCOL_VER_2_0 = 2.0;
const uint16_t DXL_MODEL_NUM     = 0x190; // OpenCM 9.04 model number
DYNAMIXEL::Slave dxl(dxl_port, DXL_MODEL_NUM);

// Addresses for read register variable
uint16_t Yaw_addr         = 66;
uint16_t Pitch_addr       = 68;
uint16_t Roll_addr        = 70;
uint16_t SW_Start_addr    = 72;
uint16_t SW_Stop_addr     = 74;
uint16_t AccelX_addr      = 76;
uint16_t AccelY_addr      = 78;
uint16_t AccelZ_addr      = 80;
uint16_t GyroX_addr       = 82;
uint16_t GyroY_addr       = 84;
uint16_t GyroZ_addr       = 86;

// Address for wirte register variable
uint8_t Reset_addr        = 88;
uint8_t dxl_enbale_addr   = 90;
uint8_t LED_R_addr        = 92;
uint8_t LED_G_addr        = 94;
uint8_t LED_B_addr        = 96;


// Stored variables for Interrupt
uint8_t Switch_Button[2];
uint8_t dxl_power_enable;

// Stored variable for imu
uint16_t dxl_data[20];
int16_t accel_raw[3];
int16_t gyro_raw[3];

int32_t temp;


//This namespace is required to use Control table item names
using namespace ControlTableItem;


void setup()
{
  DEBUG_SERIAL.begin(115200);
  disableCore0WDT();
  disableCore1WDT();

  xTaskCreatePinnedToCore(
    Task_DXL
    ,  "Task_DXL"   // task name
    ,  65536        // stack size allocations
    ,  NULL
    ,  3
    ,  &task_dxl
    ,  0);

  xTaskCreatePinnedToCore(
    Task_INPUT
    ,  "Task_INPUT"   // task namep
    ,  65536          // stack size allocations
    ,  NULL
    ,  3
    ,  &task_input
    ,  1);

}

void loop()
{
  // skip this state
}

void Task_DXL( void *pvParameters )
{
  (void) pvParameters;

  dxl_port.begin(1000000);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VER_1_0);
  dxl.setFirmwareVersion(41);
  dxl.setID(DXL_ID);

  dxl.addControlItem(Yaw_addr,        dxl_data[0]);
  dxl.addControlItem(Pitch_addr,      dxl_data[1]);
  dxl.addControlItem(Roll_addr,       dxl_data[2]);
  dxl.addControlItem(SW_Start_addr,   dxl_data[3]);
  dxl.addControlItem(SW_Stop_addr,    dxl_data[4]);
  dxl.addControlItem(AccelX_addr,     dxl_data[5]);
  dxl.addControlItem(AccelY_addr,     dxl_data[6]);
  dxl.addControlItem(AccelZ_addr,     dxl_data[7]);
  dxl.addControlItem(GyroX_addr,      dxl_data[8]);
  dxl.addControlItem(GyroY_addr,      dxl_data[9]);
  dxl.addControlItem(GyroZ_addr,      dxl_data[10]);
  dxl.addControlItem(Reset_addr,      dxl_data[11]);
  dxl.addControlItem(dxl_enbale_addr, dxl_data[12]);
  dxl.addControlItem(LED_R_addr,      dxl_data[13]);
  dxl.addControlItem(LED_G_addr,      dxl_data[14]);
  dxl.addControlItem(LED_B_addr,      dxl_data[15]);

  //  dxl.setReadCallbackFunc(read_callback_func);
  dxl.setWriteCallbackFunc(write_callback_func);

  for (;;)
  {
    if (dxl.processPacket() == false) {

      DEBUG_SERIAL.print("Last lib err code: ");
      DEBUG_SERIAL.print(dxl.getLastLibErrCode());
      DEBUG_SERIAL.print(", ");
      DEBUG_SERIAL.print("Last status packet err code: ");
      DEBUG_SERIAL.println(dxl.getLastStatusPacketError());
      DEBUG_SERIAL.println();

    }
//    delayMicroseconds(20);
//      delay(500);

  }

}

void read_callback_func(uint16_t item_addr, uint8_t &dxl_err_code, void* arg)
{
  (void)dxl_err_code, (void)arg;

  if (item_addr == Yaw_addr)
  {
    dxl_data[0];
  }

  else if (item_addr == Pitch_addr)
  {
    dxl_data[1];
  }

  else if (item_addr == Roll_addr)
  {
    dxl_data[2];
  }

  else if (item_addr == SW_Start_addr)
  {
    dxl_data[3];
  }

  else if (item_addr == SW_Stop_addr)
  {
    dxl_data[4];
  }

  else if (item_addr == AccelX_addr)
  {
    dxl_data[5];
  }

  else if (item_addr == AccelY_addr)
  {
    dxl_data[6];
  }

  else if (item_addr == AccelZ_addr)
  {
    dxl_data[7];
  }

  else if (item_addr == GyroX_addr)
  {
    dxl_data[8];
  }

  else if (item_addr == GyroY_addr)
  {
    dxl_data[9];
  }

  else if (item_addr == GyroZ_addr)
  {
    dxl_data[10];
  }

}


void write_callback_func(uint16_t item_addr, uint8_t &dxl_err_code, void* arg)
{
  (void)dxl_err_code, (void)arg;

  if (item_addr == Reset_addr)
  {
    if (dxl_data[11] == 1)
    {
      ESP.restart();
    }

  }
  if (item_addr == dxl_enbale_addr)
  {

    digitalWrite(Dxl_power, dxl_data[12]);
  }

  else if (item_addr == LED_R_addr)
  {
    digitalWrite(LED_Red_enable, dxl_data[13]);
  }

  else if (item_addr == LED_G_addr)
  {
    digitalWrite(LED_Green_enable, dxl_data[14]);
  }

  else if (item_addr == LED_B_addr)
  {
    digitalWrite(LED_Blue_enable, dxl_data[15]);
  }
}


void Task_INPUT( void *pvParameters )
{
  (void) pvParameters;
//  Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

  bno.begin(bno.OPERATION_MODE_IMUPLUS);

  pinMode(SW_Start, INPUT);
  pinMode(SW_Stop, INPUT);
  pinMode(Dxl_power, OUTPUT);
  pinMode(LED_Red_enable, OUTPUT);
  pinMode(LED_Green_enable, OUTPUT);
  pinMode(LED_Blue_enable, OUTPUT);

  for (;;)
  {
    imu::Vector<3> Orientation = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    imu::Vector<3> Accel       = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    imu::Vector<3> Gyro        = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);

    //  YPR value
    dxl_data[0]  = Orientation.x() + 1000;
    dxl_data[1]  = Orientation.z() + 1000;
    dxl_data[2]  = Orientation.y() + 1000;

    // Push Button Start & Stop
    dxl_data[3] = digitalRead(SW_Start);
    dxl_data[4] = digitalRead(SW_Stop);
    

    //  Accelerometer Value
    accel_raw[0] = Accel.y();
    accel_raw[1] = Accel.x();
    accel_raw[2] = Accel.z();

    temp = (accel_raw[0]);
    temp = temp * 5/4;
    temp = 512 + temp;
    if (temp > 1023) temp = 1023;
    if(temp < 0) temp = 0;
    dxl_data[5] = temp;

    temp = (accel_raw[1]);
    temp = temp * 5/4;
    temp = 512 + temp;
    if (temp > 1023) temp = 1023;
    if(temp < 0) temp = 0;
    dxl_data[6] = temp;

    temp = (accel_raw[2]);
    temp = temp * 5/4;
    temp = 512 + temp;
    if (temp > 1023) temp = 1023;
    if(temp < 0) temp = 0;
    dxl_data[7] = temp;


    
    //  Gyro Value
    gyro_raw[0] = Gyro.y();
    gyro_raw[1] = Gyro.x();
    gyro_raw[2] = Gyro.z();

    temp = (gyro_raw[0]);
    temp = 512 + temp;
    if (temp > 1023) temp = 1023;
    if (temp < 0) temp = 0;
    dxl_data[8] = temp;

    temp = (gyro_raw[1]);
    temp = 512 + temp;
    if (temp > 1023) temp = 1023;
    if (temp < 0) temp = 0;
    dxl_data[9] = temp;

    temp = (gyro_raw[2]);
    temp = 512 + temp;
    if (temp > 1023) temp = 1023;
    if (temp < 0) temp = 0;
    dxl_data[10] = temp;
  
    
    uint8_t system, gyro, accel = 0;
    bno.Calibration_no_mag(&system, &gyro, &accel);
  }

}
