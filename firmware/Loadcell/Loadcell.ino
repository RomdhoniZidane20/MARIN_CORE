#include "ADS126X.h"
#include <array>
#include <Preferences.h>
#include <Dynamixel2Arduino.h>

// Dynamixel Arduino define pin for CM
#define DXL_SERIAL               Serial2
#define DEBUG_SERIAL             Serial
const uint8_t DXL_DIR_PIN =      5;
const uint8_t DXL_ID      =      200; // ID Dynamixel

// addresses for loadcell
uint16_t addr_force_1           = 60;
uint16_t addr_force_2           = 64;
uint16_t addr_force_3           = 68;
uint16_t addr_force_4           = 72;

//Dynamixel Arduino porthandler and packethandler
DYNAMIXEL::SerialPortHandler dxl_port(DXL_SERIAL, DXL_DIR_PIN);
const float DXL_PROTOCOL_VER_1_0 = 1.0;
const float DXL_PROTOCOL_VER_2_0 = 2.0;
const uint16_t DXL_MODEL_NUM     = 0x136; // OpenCM 9.04 model number
DYNAMIXEL::Slave dxl(dxl_port, DXL_MODEL_NUM);

// ADS126 define pin
ADS126X adc;
#define NUM_AVG 5 //number of samples to use for comparing the value to the median of the
#define ADS_CHIP_SELECT 5 // CS pin
#define ADS_RESET_PIN 4

// Stored variables for loadcell
std::array<uint16_t, 4> force;
std::array<uint8_t, 4> pos_pin;
std::array<uint8_t, 4> neg_pin;
std::array<std::array<int32_t, NUM_AVG>, 4> previous_values;

// cop variables
float dx = 1; // panjang titik tengah F1 dan F2 or F3 dan F4
float dy = 1; // lebar  titik tengah F1 dan F3 or F2 dan F4
float x  = 1; // diameter dalam pul ke luar plat bagian samping (X)
float y  = 1; // diametet dalam pul ke luar plat bagian atas/bawah (Y)


void setup() {

  DEBUG_SERIAL.begin(115200);
  init_ads();

  dxl_port.begin(1000000);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VER_1_0);
  dxl.setFirmwareVersion(25);
  dxl.setID(DXL_ID);

  dxl.addControlItem(addr_force_1, force[0]);
  dxl.addControlItem(addr_force_2, force[1]);
  dxl.addControlItem(addr_force_3, force[2]);
  dxl.addControlItem(addr_force_4, force[3]);

  dxl.setReadCallbackFunc(read_callback_func);


  // ads and loadcell
  pos_pin[0] = 7; //ADS126X_AIN7;
  pos_pin[1] = 5; //ADS126X_AIN5;
  pos_pin[2] = 3; //ADS126X_AIN3;
  pos_pin[3] = 1; //ADS126X_AIN1;

  neg_pin[0] = 6; //ADS126X_AIN6;
  neg_pin[1] = 4; //ADS126X_AIN4;
  neg_pin[2] = 2; //ADS126X_AIN2;
  neg_pin[3] = 0; //ADS126X_AIN0;


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

  int current_sensor = 0;
  long count = 0;
  long counter = 0;

  adc.readADC1(pos_pin[current_sensor], neg_pin[current_sensor]);
  delayMicroseconds(250);

  uint16_t reading = adc.readADC1(pos_pin[current_sensor], neg_pin[current_sensor]);

  if (!reading || reading == 1) // detect if faulty reading occured
  {
    init_ads();
  }
  else if (reading == 0x1000000) //detect if reset occured
  {
    if (adc.checkResetBit())
    {
      init_ads();
    }
  }
  else
  {
    // sort to get median of previous values
    std::array<int32_t, NUM_AVG> current_previous = previous_values[current_sensor];
    std::sort(current_previous.begin(), current_previous.end());

    // check if current reading deviates too strongly from median
    if (abs(current_previous[NUM_AVG / 2] - reading) < 1e7)
    {
      force[current_sensor] = reading;
    }
    // update previous values for median calculation
    previous_values[current_sensor][count] = reading;

    // update current sensor
    current_sensor = (current_sensor + 1) % 4;

    // update counter for median calculating
    if (!current_sensor)
    {
      count = (count + 1) % NUM_AVG;
    }
  }


  //  long voltage1 = adc.readADC1(pos_pin[0], neg_pin[0]);
  //  long voltage2 = adc.readADC1(pos_pin[1], neg_pin[1]);
  //  long voltage3 = adc.readADC1(pos_pin[2], neg_pin[2]);
  //  long voltage4 = adc.readADC1(pos_pin[3], neg_pin[3]);
}

void init_ads()
{

  digitalWrite(4, LOW);
  delayMicroseconds(2);
  digitalWrite(4, HIGH);
  delayMicroseconds(4);

  adc.begin(ADS_CHIP_SELECT); // setup with chip select pin
  adc.setGain(ADS126X_GAIN_32);
  adc.setRate(ADS126X_RATE_38400);
  adc.setFilter(ADS126X_SINC4);
  adc.enableInternalReference();
  adc.startADC1(); // start conversion on ADC1

  adc.disableStatus();
  adc.disableCheck();
  adc.setDelay(ADS126X_DELAY_0);
  adc.clearResetBit();

}

void read_callback_func(uint16_t item_addr, uint8_t &dxl_err_code, void* arg)
{
  (void)dxl_err_code, (void)arg;

  if (item_addr == addr_force_1)
  {
    force[0];
  }

  if (item_addr == addr_force_2)
  {
    force[2];
  }

  if (item_addr == addr_force_3)
  {
    force[3];
  }

  if (item_addr == addr_force_4)
  {
    force[4];
  }
}
