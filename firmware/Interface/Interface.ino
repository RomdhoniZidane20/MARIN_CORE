#include <Dynamixel2Arduino.h>

// Dynamixel Arduino define pin for STM32
#define DXL_SERIAL               Serial3
#define DEBUG_SERIAL             Serial2
const uint8_t DXL_DIR_PIN =      22;
const uint8_t DXL_ID      =      1;

// Pinout for Switch
#define SW_Start                 34
#define SW_Stop                  35
#define Dxl_power                32

// Pinout for LED
#define LED_Red_enable           12
#define LED_Green_enable         13
#define LED_Blue_enable          14
#define LED_Yellow_enable        15
#define LED_Orange_enable        16

//State
//bool dxl_power_is;
volatile int start_button_state = 0;
volatile int stop_button_state  = 0;

// Dynamixel Arduino porthandler and packethandler
DYNAMIXEL::SerialPortHandler dxl_port(DXL_SERIAL, DXL_DIR_PIN);
const float DXL_PROTOCOL_VER_1_0 = 1.0;
const float DXL_PROTOCOL_VER_2_0 = 2.0;
const uint16_t DXL_MODEL_NUM     = 0x190; // OpenCM 9.04 model number
DYNAMIXEL::Slave dxl(dxl_port, DXL_MODEL_NUM);

//Addresses for register variable
uint16_t SW_Start_addr    = 56;
uint16_t SW_Stop_addr     = 60;
uint16_t LED_R_addr       = 104;
uint16_t LED_G_addr       = 108;
uint16_t LED_B_addr       = 112;
uint16_t LED_Y_addr       = 108;
uint16_t LED_O_addr       = 112;

// Stored variables for Interrupt
uint8_t Switch_Button[2];
uint8_t dxl_power_enable;

// Store variable for LED
uint8_t led[5];

void setup() {

  pinMode(SW_Start, INPUT);
  pinMode(SW_Stop, INPUT);

  dxl_port.begin(1000000);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VER_1_0);
  dxl.setFirmwareVersion(25);
  dxl.setID(DXL_ID);

  dxl.addControlItem(SW_Start_addr, Switch_Button[0]);
  dxl.addControlItem(SW_Stop_addr, Switch_Button[1]);
  dxl.addControlItem(LED_R_addr, led[0]);
  dxl.addControlItem(LED_G_addr, led[1]);
  dxl.addControlItem(LED_B_addr, led[2]);
  dxl.addControlItem(LED_Y_addr, led[3]);
  dxl.addControlItem(LED_O_addr, led[4]);

  dxl.setReadCallbackFunc(read_callback_func);
  dxl.setWriteCallbackFunc(write_callback_func);


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
}
void write_callback_func(uint16_t item_addr, uint8_t &dxl_err_code, void* arg)
{
  (void)dxl_err_code, (void)arg;

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

  if (item_addr == LED_Y_addr)
  {
    digitalWrite(led[3], LED_Yellow_enable);
  }

  if (item_addr == LED_O_addr)
  {
    digitalWrite(led[4], LED_Orange_enable);
  }

}
