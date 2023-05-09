#ifndef SHUTTER_H
#define SHUTTER_H

#include <Arduino.h>

class Shutter {
  
  private:
    byte _pinUp;
    byte _pinDown;
    int _newPosition

    
  public:
    // Setup pin Shutter and call init()
    Shutter(byte pinUp, byte pinDown, int newPosition);

    void init();
    
    void setPosition(newPosition);
    // Move shutter Up
    void up();
    // Move Shutter down
    void down();
    // Shutter stop
    void stop();


};

#endif
