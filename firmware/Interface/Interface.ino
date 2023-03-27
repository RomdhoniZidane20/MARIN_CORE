#include <Dynamixel2Arduino.h>

// Dynamixel Arduino define pin for STM32
#define DXL_SERIAL               Serial3
#define DEBUG_SERIAL             Serial1
const uint8_t DXL_DIR_PIN     =  PA1;
const uint8_t DXL_ID          =  17;
  
// Pinout for Switch
#define SW_Start                 PB13
#define SW_Stop                  PB14
#define Dxl_power                PA2

// Pinout for LED
#define LED_Red_enable           PB5  
#define LED_Green_enable         PB6
#define LED_Blue_enable          PC13
#define LED_Yellow_enable        PB8
#define LED_Orange_enable        PB9

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
uint16_t SW_Start_addr    = 80;
uint16_t SW_Stop_addr     = 82;
uint16_t LED_R_addr       = 84;
uint16_t LED_G_addr       = 86;
uint16_t LED_B_addr       = 88;
uint16_t LED_Y_addr       = 90;
uint16_t LED_O_addr       = 92;

// Stored variables for Interrupt
uint8_t Switch_Button[2];
uint8_t dxl_power_enable;

// Store variable for LED
uint8_t led[5];

uint16_t dxl_data[20];

void setup() {

  DEBUG_SERIAL.begin(115200);
  
  pinMode(SW_Start, INPUT);
  pinMode(SW_Stop, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  dxl_port.begin(1000000);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VER_1_0);
  dxl.setFirmwareVersion(1);
  dxl.setID(DXL_ID);

  dxl.addControlItem(SW_Start_addr, dxl_data[0]);
  dxl.addControlItem(SW_Stop_addr, dxl_data[1]);
  dxl.addControlItem(LED_R_addr, dxl_data[2]);
  dxl.addControlItem(LED_G_addr, dxl_data[3]);
  dxl.addControlItem(LED_B_addr, dxl_data[4]);
  dxl.addControlItem(LED_Y_addr, dxl_data[5]);
  dxl.addControlItem(LED_O_addr, dxl_data[6]);

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
//    DEBUG_SERIAL.print
    dxl_data[0] = start_button_state;
  }

  if (item_addr == SW_Stop_addr)
  {
    stop_button_state = digitalRead(SW_Stop);
    dxl_data[1] = stop_button_state;
  }
}
void write_callback_func(uint16_t item_addr, uint8_t &dxl_err_code, void* arg)
{
  (void)dxl_err_code, (void)arg;

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
    digitalWrite(LED_BUILTIN, dxl_data[4]);
  }

  if (item_addr == LED_Y_addr)
  {
    digitalWrite(LED_Yellow_enable, dxl_data[5]);
  }

  if (item_addr == LED_O_addr)
  {
    digitalWrite(LED_Orange_enable, dxl_data[6]);
  }

}
