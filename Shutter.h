#ifndef SHUTTER_H
#define SHUTTER_H
#include <Arduino.h>

class Shutter {
  private:
    byte _pinUp;
    byte _pinDown;  
  public:
    Shutter(byte pinUp, byte pinDown);
    void init();
    void up();
    void down();
    void stop();
};
#endif
