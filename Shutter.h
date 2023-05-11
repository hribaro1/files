#ifndef SHUTTER_H
#define SHUTTER_H

#include <Arduino.h>

class Shutter {
  
  private:
    byte _pinUp;
    byte _pinDown;


    
  public:
    // Setup pin Shutter and call init()
    Shutter(byte pinUp, byte pinDown);

    void init();
    
    // Move shutter Up
    void up();
    // Move Shutter down
    void down();
    // Shutter stop
    void stop();


};

#endif
