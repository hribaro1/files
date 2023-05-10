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
   rollTimeUp = calibratedTimeUp*(newPosition - currentPosition)/100;
   rollStartTime= milldown();
   if ((millis()-rollStartTime)<=rollTimeUp) {
     up();
   } else {
     stop()
   }
       
 } else {
   rollTimeDown = calibratedTimeDown*(currentPosition - newPosition)/100;
   rollStartTime= millis();
   if ((millis()-rollStartTime)<=rollTimeDown) {
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
