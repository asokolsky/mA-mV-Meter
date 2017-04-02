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

//u8g2_uint_t g_uDisplayWidth = 0;
//u8g2_uint_t g_uDisplayHeight = 0;
//u8g2_uint_t g_uColUnits = 0;

/** update the display this often (in ms) */
int16_t g_iDisplayUpdate = 1;

class MyButton : public PinButton
{
public:
  MyButton(const uint8_t bPin) : PinButton(bPin) {}
  
  bool onUserInActivity(unsigned long ulNow)
  {
    DEBUG_PRINT("MyButton::onUserInActivity(");DEBUG_PRINTDEC(ulNow);DEBUG_PRINTLN(")");    
    return false;
  }
  bool onKeyAutoRepeat()
  {
    DEBUG_PRINTLN("MyButton::onKeyAutoRepeat()");
    return false;
  }
  bool onKeyDown()
  {
    DEBUG_PRINTLN("MyButton::onKeyDown()");
    return false;
  }
  bool onLongKeyDown()
  {
    DEBUG_PRINTLN("MyButton::onLongKeyDown()");
    return false;
  }
  bool onKeyUp(bool bLong)
  {
    DEBUG_PRINT("MyButton::onKeyUp(bLong=");DEBUG_PRINTDEC(bLong);DEBUG_PRINTLN(")");  
    return false;
  }
  bool onClick()
  {
    DEBUG_PRINTLN("MyButton::onClick");
      static int16_t iDisplayUpdateOptions[] = {10, 500, 2000};
      static int8_t i = 1;
      g_iDisplayUpdate = iDisplayUpdateOptions[i++];
      DEBUG_PRINT("g_iDisplayUpdate=");DEBUG_PRINTDEC(g_iDisplayUpdate);DEBUG_PRINTLN("");  
      if(i >= (sizeof(iDisplayUpdateOptions)/sizeof(iDisplayUpdateOptions[0])))
        i = 0;
    return false;
  }
  bool onDoubleClick()
  {
    DEBUG_PRINTLN("MyButton::onDoubleClick");
    return false;
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
  //g_uDisplayWidth = u8g2.getWidth();
  //g_uColUnits = u8g2.getWidth() - 35; 
  //g_uDisplayHeight = u8g2.getHeight();
}

/** draw batttery gauge */
void drawBattery(u8g2_uint_t x, u8g2_uint_t y, uint8_t percentfull)
{
  u8g2_uint_t w = 14;
  u8g2_uint_t h = 8;
  u8g2.drawFrame(x, y, w, h);
  u8g2.drawVLine(x+w, y+3, h-6);
  x += 2;
  y += 2;
  h -= 4;
  w -= 4;
  if(percentfull > 10)
    u8g2.drawBox(x, y, (percentfull > 90) ? w : map(percentfull, 0, 100, 0, w), h);
}

/** draw title bar including batttery gauge */
void drawTitleBar(int16_t iDisplayUpdate)
{
  u8g2.setFont(u8g2_font_profont10_mf);
  char buf[40];
  sprintf(buf, "%ims", iDisplayUpdate);
  u8g2.drawStr(0, 8, buf);
  uint8_t percentfull = (millis() / 100) % 100;
  drawBattery(u8g2.getWidth() - 15, 0, percentfull);
}

void drawLine(u8g2_uint_t iRow, int16_t iValue, const char *szUnits)
{
  u8g2.setFont(u8g2_font_helvR24_tf);
  char buf[80];
  sprintf(buf, "%5.3i", iValue);
  u8g2.drawStr(0, iRow, buf);  
  u8g2.setFont(u8g2_font_helvR14_tf);
  u8g2.drawStr(u8g2.getWidth() - 30, iRow, szUnits);
}


void loop(void) 
{
  unsigned long ulNow = millis();
  if(g_button.getAndDispatchKey(ulNow))
  {
    // return;
    ;
  }
  //
  // freeze display while the key is down
  //
  if(g_button.isKeyDown())
    return;
  static unsigned long ulNextUpdate = 0;
  if(ulNow < ulNextUpdate)
    return;
  ulNextUpdate = ulNow + (unsigned long)g_iDisplayUpdate;
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
  
  drawTitleBar(g_iDisplayUpdate);
  
  const int8_t iTitleBarHeight = 8;
  const int8_t iLineHeight = 27;
  u8g2_uint_t iRow = iLineHeight + iTitleBarHeight;
  current_mA = (float)ulNow;
  drawLine(iRow, (int16_t)current_mA, "mA");
  iRow += iLineHeight;
  loadVoltage_mV = (float)millis();
  drawLine(iRow, (int16_t)loadVoltage_mV, "mV");
  
  u8g2.sendBuffer();                  // transfer internal memory to the display
}

