#include "Shutter.h"
#include "Arduino.h"

Shutter::Shutter(byte pinUp, byte pinDown) {
  _pinUp = pinUp;
  _pinDown = pinDown;
  init();
}
void Shutter::init() {
  pinMode(_pinUp, OUTPUT);
  pinMode(_pinDown, OUTPUT);
  stop();
}


void Shutter::setPosition(newPosition, currentPosition, calibratedTimeUp, calibratedTimeDown) {
  
  if (newPosition >=  currentPosition) {   
   _rollTime = calibratedTimeUp*(newPosition - currentPosition)/100;
   _rollStartTime = millis();
   if ((millis() - _rollStartTime) <= _rollTime) {
     up();
   } else {
     stop()
   }
       
 } else {
   _rollTime = calibratedTimeDown*(currentPosition - newPosition)/100;
   _rollStartTime = millis();
   if ((millis() - _rollStartTime) <= _rollTime) {
     down();
   } else {
     stop();
   }
 }
}

void Shutter::up() {
  digitalWrite(_pinDown, LOW);
  delay(100);
  digitalWrite(_pinUp, HIGH);
 
  
}
void Shutter::down() {
  digitalWrite(_pinUp, LOW);
  delay(100);
  digitalWrite(_pinDown, HIGH);


}
void Shutter::stop() {
  digitalWrite(_pinUp, LOW);
  digitalWrite(_pinDown, LOW);

}
