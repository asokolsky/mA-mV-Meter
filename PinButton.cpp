#define NODEBUG 1
#include <Arduino.h>
#include "PinButton.h"
#include "Trace.h"

PinButton::PinButton(uint8_t bPin) : m_bPin(bPin)
{
  pinMode(m_bPin, INPUT_PULLUP);
}


bool PinButton::getAndDispatchKey(unsigned long ulNow)
{
  // get out if we are still bouncing!
  if(ulNow < m_ulBounceSubsided)
    return false;
  
  bool bRes = false;
  // read the momentary state from hardware
  bool bPressed = (digitalRead(m_bPin) == 0);
  if(bPressed == m_bOldPressed) 
  {
    if(!bPressed)
    {
      if(!isUserLongInactive(ulNow))
        return false;
      bRes = onUserInActivity(ulNow);
      onUserActivity(ulNow);
      return bRes;      
    }
    // Button is PRESSED!
    // fire auto repeat logic here
    if((m_ulToFireAutoRepeat == 0) || (ulNow < m_ulToFireAutoRepeat))
    {
      ;
    }
    else
    {
      m_ulToFireAutoRepeat = ulNow + s_iAutoRepeatDelay;
      DEBUG_PRINTLN("onKeyAutoRepeat");
      bRes = onKeyAutoRepeat();
    }      
    // fire long key logic here
    if((m_ulToFireLongKey == 0) || (ulNow < m_ulToFireLongKey))
      return bRes;
    m_ulToFireLongKey = 0;
    DEBUG_PRINTLN("onLongKeyDown");
    return onLongKeyDown() || bRes;
  }
  // bPressed != m_bOldPressed
  if(m_ulBounceSubsided == 0) 
  {
    m_ulBounceSubsided = ulNow + s_iDebounceDelay;
    return false;
  }
  if(bPressed)
  {
    // button was just pressed!
    m_ulToFireLongKey = ulNow + s_iLongKeyDelay;
    m_ulToFireAutoRepeat = ulNow + s_iAutoRepeatDelay;
    m_ulBounceSubsided = 0;
    DEBUG_PRINTLN("onKeyDown");
    bRes = onKeyDown();
  }
  else
  {
    // button was released!
    bool bLongKey = (m_ulToFireLongKey == 0);
    m_ulToFireAutoRepeat = m_ulToFireLongKey = m_ulBounceSubsided = 0;
    DEBUG_PRINTLN("onKeyUp");
    bRes = onKeyUp(bLongKey);
  }
  onUserActivity(ulNow);
  m_bOldPressed = bPressed;
  return bRes;
}
