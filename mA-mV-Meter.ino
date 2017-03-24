#include <Arduino.h>
#include <U8g2lib.h>

//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

u8g2_uint_t g_uDisplayWidth = 0;
u8g2_uint_t g_uDisplayHeight = 0;
u8g2_uint_t g_uColUnits = 0;

void setup(void) 
{
  u8g2.begin();
  g_uDisplayWidth = u8g2.getWidth();
  g_uColUnits = g_uDisplayWidth - 35; 
  g_uDisplayHeight = u8g2.getHeight();
}

void drawLine(int8_t iRow, long int iValue, const char *szUnits)
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
  u8g2.clearBuffer();                 // clear the internal memory

  int8_t iLineHeight = 30; // u8g2.getAscent() + u8g2.getDescent();
  int8_t iRow = iLineHeight;
  drawLine(iRow, (long int)millis(), "mA");
  iRow += iLineHeight;
  drawLine(iRow, (long int)millis(), "mV");
  
  u8g2.sendBuffer();                  // transfer internal memory to the display
  delay(100);
}

