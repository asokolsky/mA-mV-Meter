#pragma once

/**
 * Simple momentary button connected to a digital pin
 * Generates the following events to be handled by deriving from PinButton
 *
  bool onUserInActivity(unsigned long ulNow);
  bool onKeyAutoRepeat();
  bool onKeyDown();
  bool onLongKeyDown();
  bool onKeyUp(bool bLong);
  bool onKeyDoubleClick();
  
 */


class PinButton
{
public:
  /** delay in ms to debounce */
  const int s_iDebounceDelay = 50;
  /** delay in ms to autorepeat */
  const int s_iAutoRepeatDelay = 500;
  /** delay in ms before the long key is fired */
  const int s_iLongKeyDelay = 3000;
  /** inactivity timeout in milliseconds */
  const unsigned long s_ulInactivityDelay = 10000;

  PinButton(uint8_t bPin);

  bool getAndDispatchKey(unsigned long ulNow);

  /** call backs.  derive a class and overwrite those */
  virtual bool onUserInActivity(unsigned long ulNow) = 0;
  virtual bool onKeyAutoRepeat() = 0;
  virtual bool onKeyDown() = 0;
  virtual bool onLongKeyDown() = 0;
  virtual bool onKeyUp(bool bLong) = 0;

  bool isUserLongInactive(unsigned long ulNow) 
  {
    return (ulNow > m_ulToFireInactivity);
  }
  /** 
   * Delay inactivity notification.
   * User does NOT have to call it. 
   */
  virtual void onUserActivity(unsigned long ulNow) 
  {
    m_ulToFireInactivity = ulNow + s_ulInactivityDelay;
  }

protected:
  /** Digital pin from which we are reading */
  uint8_t m_bPin;
  /** when to fire long key */
  unsigned long m_ulToFireLongKey = 0;
  /** when to fire key auto repeat */
  unsigned long m_ulToFireAutoRepeat = 0;
  /** when bouncing subsides */
  unsigned long m_ulBounceSubsided = 0;
  /** when inactivity timeout will happen */
  unsigned long m_ulToFireInactivity = s_ulInactivityDelay;

  /** the key state when we last called getAndDispatchKey */
  bool m_bOldPressed = false;
};

