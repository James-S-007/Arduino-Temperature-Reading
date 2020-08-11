#include "Sensor.h"
#include "Arduino.h"
#include <I2C.h>

//78 degree ambient, does not detect person at 10ft


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
  int errTotal = 1;
  uint8_t LSB;
  uint8_t MSB;
  uint8_t PEC;
  while (errCount < 3 && errTotal != 0) {
    errTotal = 0;
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
      errCount += 1;
    }
  }
  if (errCount ==3){
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
    /*
    if (result >= 10000){
      Serial.print("Sensor Number: ");
      Serial.print(address, DEC);
      Serial.print(", Temp: ");
      Serial.println(result, DEC);
    }
    */
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
