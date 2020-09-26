#include "Sensor.h"
#include "Arduino.h"
#include <Wire.h>

const uint8_t Sensor::ambientTempRegister = 0x06;
const uint8_t Sensor::objTempRegister = 0x07;
unsigned long Sensor::prevMillis = 0;             //for timeout
uint8_t Sensor::numSensors = 0;
uint8_t Sensor::numObjectsDetected = 0;
int Sensor::maxTemp = 0;
uint8_t Sensor::maxSensor = 0;
uint8_t Sensor::triggerSensor = 0;

//static values to adjust
int Sensor::timeoutTime = 5000;
uint8_t Sensor::activateThreshold = 1;
uint8_t Sensor::deactivateThreshold = 0;          //number of required sensors that no longer detect an object to return to idle
int Sensor::tempThreshold = 600;                    //tempThreshold is *100; i.e. tempThreshold = 600 is 6.00 degree threshold
int Sensor::maxTempThreshold = 9500;                //maxTempThreshold is *100; i.e. maxTempThreshold = 9750 is 97.50 degree

Sensor::Sensor(uint8_t addy){
  address = addy;
  active = true;
  objectDetected = false;
  numSensors += 1;
  ambientTemp = 0;
  maxTempSensor = 0;

  //ambientArr[10] = {};
  //index = 0;
}

uint8_t Sensor::getAddy(){
  return address;
}

int Sensor::readTemp(uint8_t registerAddy){               //returns 100*temp (for greater speed)
  uint8_t errCount = 0;
  uint8_t LSB;
  uint8_t MSB;
  uint8_t PEC;
  while (errCount < 5) {
    Wire.beginTransmission(this->address);
    Wire.write(registerAddy);
    Wire.endTransmission(false);                 //stop = false: allows for repeated start condition
    Wire.requestFrom(this->address,3, true);     //stop = true: release bus once transmission is finished
    while(Wire.available() > 0){
      if (Wire.available() == 3){
        LSB = Wire.read();
      }
      else if (Wire.available() == 2){
        MSB = Wire.read();
      }
      else if (Wire.available() == 1){
        PEC = Wire.read();
      }
    }
    if (MSB == 0 && LSB == 0 && PEC == 0){
        errCount += 1;
    }
    else{
      break;
    }
  }
  if (errCount == 5){
    Serial.print("Error reading from sensor with address: ");
    Serial.println(address, HEX);
    Serial.println("Try resetting and ensuring all connections are secure");
    Serial.println("If problem persists, replace hardware or call a technician");
    delay(10000000000000000000000);                           //stop running program if sensor is not working properly
  }
  else{
    int result = MSB << 8;
    result |= LSB;
    result = ((result*.02 - 273.15)*9/5+32)*100;
    return result;
  }
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

void Sensor::setMaxTempSensor(int temp){
  maxTempSensor = temp;
}

int Sensor::getMaxTempSensor(){
  return maxTempSensor;
}

void Sensor::setAmbientTemp(int temp){
  ambientTemp = temp;
}

int Sensor::getAmbientTemp(){
  return ambientTemp;
}

void Sensor::updateAmbientTemp(int newVal){
  int newAmb = ambientTemp*.9;
  newAmb = newAmb + newVal*.1;
  ambientTemp = newAmb;
}
