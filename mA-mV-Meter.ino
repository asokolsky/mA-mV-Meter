#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_INA219.h>
#include "PinButton.h"
#include "Trace.h"

Adafruit_INA219 ina219;

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif


//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

u8g2_uint_t g_uDisplayWidth = 0;
//u8g2_uint_t g_uDisplayHeight = 0;
u8g2_uint_t g_uColUnits = 0;

bool g_bDisplayFreeze = false;

class MyButton : public PinButton
{
public:
  MyButton(const uint8_t bPin) : PinButton(bPin) {}
  
  bool onUserInActivity(unsigned long ulNow)
  {
    DEBUG_PRINT("MyButton::onUserInActivity(");DEBUG_PRINTDEC(ulNow);DEBUG_PRINTLN(")");    
  }
  bool onKeyAutoRepeat()
  {
    DEBUG_PRINTLN("MyButton::onKeyAutoRepeat()");
  }
  bool onKeyDown()
  {
    DEBUG_PRINTLN("MyButton::onKeyDown()");
  }
  bool onLongKeyDown()
  {
    DEBUG_PRINTLN("MyButton::onLongKeyDown()");
  }
  bool onKeyUp(bool bLong)
  {
    DEBUG_PRINT("MyButton::onKeyUp(bLong=");DEBUG_PRINTDEC(bLong);DEBUG_PRINTLN(")");  
    g_bDisplayFreeze = !g_bDisplayFreeze;
  }
  
};

/** Button is connected to digital pin 2 */
MyButton g_button(2);

void setup(void) 
{
  Serial.begin(115200);
  delay(1000);   
  //while(!Serial)  ; // wait for serial port to connect. Needed for Leonardo only
  DEBUG_PRINTLN("Meter test!");

  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  ina219.begin();
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();
  
   
  u8g2.begin();
  g_uDisplayWidth = u8g2.getWidth();
  g_uColUnits = g_uDisplayWidth - 35; 
  //g_uDisplayHeight = u8g2.getHeight();
}

void drawLine(int8_t iRow, int16_t iValue, const char *szUnits)
{
  u8g2.setFont(u8g2_font_helvR24_tf);
  char buf[80];
  sprintf(buf, "%5.3i", iValue);
  u8g2.drawStr(0, iRow, buf);  
  u8g2.setFont(u8g2_font_helvR14_tf);
  u8g2.drawStr(g_uColUnits, iRow, szUnits);
}

void loop(void) 
{
  unsigned long ulNow = millis();
  if(g_button.getAndDispatchKey(ulNow))
  {
    // return;
    ;
  }
  if(g_bDisplayFreeze)
    return;
  
  //
  // do some measurement
  //
  float shuntVoltage = ina219.getShuntVoltage_mV();
  float busVoltage = ina219.getBusVoltage_V();
  float loadVoltage_mV = (busVoltage*1000) + shuntVoltage;
  float current_mA = ina219.getCurrent_mA();

  // 
  // update the display
  //
  u8g2.clearBuffer();                 // clear the internal memory

  int8_t iLineHeight = 30; // u8g2.getAscent() + u8g2.getDescent();
  int8_t iRow = iLineHeight;
  current_mA = (float)ulNow;
  drawLine(iRow, (int16_t)current_mA, "mA");
  iRow += iLineHeight;
  loadVoltage_mV = (float)millis();
  drawLine(iRow, (int16_t)loadVoltage_mV, "mV");
  
  u8g2.sendBuffer();                  // transfer internal memory to the display
}

