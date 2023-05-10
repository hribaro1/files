#ifndef SHUTTER_H
#define SHUTTER_H

#include <Arduino.h>

class Shutter {
  
  private:
    byte _pinUp;
    byte _pinDown;
    unsigned long _rollTime;
    unsigned long _rollStartTime;

    
  public:
    // Setup pin Shutter and call init()
    Shutter(byte pinUp, byte pinDown);

    void init();
    
    void setPosition(int newPosition, int currentPosition, int calibratedTimeUp, int calibratedTimeDown);
    // Move shutter Up
    void up();
    // Move Shutter down
    void down();
    // Shutter stop
    void stop();


};

#endif
