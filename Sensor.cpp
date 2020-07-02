#include "Sensor.h"
#include "Arduino.h"
#include <I2C.h>

const uint8_t Sensor::ambientTempRegister = 0x06;
const uint8_t Sensor::objTempRegister = 0x07;
unsigned long Sensor::prevMillis = 0;             //for timeout
bool Sensor::idle = true;
uint8_t Sensor::numSensors = 0;
uint8_t Sensor::numObjectsDetected = 0;
float Sensor::maxTemp = 0;

//static values to adjust
int Sensor::timeoutTime = 5000;
uint8_t Sensor::activateThreshold = 1;
uint8_t Sensor::deactivateThreshold = 0;          //number of required sensors that no longer detect an object to return to idle
int Sensor::tempThreshold = 4;
int Sensor::maxTempThreshold = 85;

Sensor::Sensor(uint8_t addy){
  address = addy;
  active = true;
  objectDetected = false;
  numSensors += 1;
  ambientTemp = 0;
}


int Sensor::readTemp(uint8_t registerAddy){               //returns 100*temp (for greater speed)
  int errTotal = 0;
  uint8_t LSB;
  uint8_t MSB;
  uint8_t PEC;
  errTotal += I2c._start();
  errTotal += I2c._sendAddress(this->address<<1);              //left shifted with W bit (0)
  errTotal += I2c._sendByte(registerAddy);                  
  errTotal += I2c._start();
  errTotal += I2c._sendAddress((this->address<<1)+1);       //left shifted with R bit (1)
  errTotal += I2c._receiveByte(0xFF, &LSB);                 //demand ACK
  errTotal += I2c._receiveByte(0xFF, &MSB);                 //demand ACK
  errTotal += I2c._receiveByte(0x00, &PEC);                 //last byte; don't demand ACK
  errTotal += I2c._stop();
  if (errTotal != 0){
    Serial.print("Error Reading from Sensor at Address: ");
    Serial.println(this->address, HEX);
    return -1;                                          /////////////addition needed
  }
  else{
    int result = MSB << 8;
    result |= LSB;
    result = ((result*.02 - 273)*9/5+32)*100;
    return result;
  }
}

void Sensor::resetStatic(){
  maxTemp = 0;
}

void Sensor::setObjectDetected(bool obj){
  bool oldVal = objectDetected;
  objectDetected = obj;
  if (obj == true && obj != oldVal){                //only update number of objects detected if set is different from previous value
    numObjectsDetected += 1;
  }
  else if (obj == false && obj != oldVal){
    numObjectsDetected -= 1;
  }
}

bool Sensor::getObjectDetected(){
  return objectDetected;
}

void Sensor::setAmbientTemp(int temp){
  ambientTemp = temp;
}

int Sensor::getAmbientTemp(){
  return ambientTemp;
}
