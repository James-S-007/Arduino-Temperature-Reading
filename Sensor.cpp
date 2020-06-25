#include "Sensor.h"
#include "Arduino.h"
#include <I2C.h>

const uint8_t Sensor::ambientTempRegister = 0x06;
const uint8_t Sensor::objTempRegister = 0x07;
int Sensor::objectsDetected = 0;                  //not yet used
unsigned long Sensor::prevMillis = 0;             //for timeout
bool Sensor::idle = true;
uint8_t Sensor::numSensors = 0;
uint8_t Sensor::numInactive = 0;
float Sensor::maxTemp = 0;

//static values to adjust
int Sensor::timeoutTime = 5000;
uint8_t Sensor::activatedThreshold = 1;
uint8_t Sensor::tempThreshold = 7;


Sensor::Sensor(uint8_t addy){
  address = addy;
  active = true;
  numSensors += 1;
}

float Sensor::readTemp(uint8_t registerAddy){
  int errMessage = I2c.read(this->address, registerAddy, 3);    //3 bytes from datasheet, LSB then MSB then PEC
  if (errMessage != 0){
    Serial.print("Error Number: ");
    Serial.print(errMessage, DEC);
    Serial.print(" while attempting to read from sensor: ");
    Serial.println(this->address, HEX);
    return -1;
  }
  else {
    uint8_t LSB = I2c.receive();                          //LSB first
    uint8_t MSB = I2c.receive();
    if (MSB > 0x80){
      Serial.println("Error Flag Raised");
    }
    int result = MSB << 8;                                //MSB left shifted a byte
    result |= LSB;
    int temp = ((result*.02 - 273)*9/5+32)*100;
    float dispTemp = temp;
    dispTemp /= 100;
    I2c.receive();                                        //Remove PEC from buffer
    return dispTemp;
  }
}

bool Sensor::checkTimeout(){
  if (millis - prevMillis > timeoutTime){
    return true;
  }
  else{
    return false;
  }
}

void Sensor::resetStatic(){
  maxTemp = 0;
  numInactive = 0;
  objectsDetected = 0;
}

void Sensor::setActive(bool act){
  active = act;
}

bool Sensor::getActive(){
  return active;
}
