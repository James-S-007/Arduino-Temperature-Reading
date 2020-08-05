#include "Sensor.h"
#include "Arduino.h"
#include <I2C.h>

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
int Sensor::tempThreshold = 6;
int Sensor::maxTempThreshold = 100;

Sensor::Sensor(uint8_t addy){
  address = addy;
  active = true;
  objectDetected = false;
  numSensors += 1;
  ambientTemp = 0;
  maxTempSensor = 0;
}

uint8_t Sensor::getAddy(){
  return address;
}

uint16_t Sensor::readRAM(uint8_t ramAddress){
  uint8_t errCount = 0;
  int errTotal = 1;
  uint8_t LSB;
  uint8_t MSB;
  uint8_t PEC;
  while (errCount < 3 && errTotal != 0) {
    errTotal = 0;
    errTotal += I2c._start();
    errTotal += I2c._sendAddress(this->address<<1);              //left shifted with W bit (0)
    errTotal += I2c._sendByte(ramAddress);                  
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
    uint16_t result = MSB << 8;
    result |= LSB;
    return result;
  }
}

bool Sensor::writeRAM(uint8_t ramAddress, uint8_t MSB, uint8_t LSB, uint8_t CRC1, uint8_t CRC2){
  
  I2c._start();
  I2c._sendAddress(this->address<<1);     //left shift w/ write bit
  I2c._sendByte(ramAddress);
  I2c._sendByte(0x00);                    //clear EEPROM
  I2c._sendByte(0x00);                    //clear EEPROM

  if (I2c._sendByte(CRC1) == 0){        ///////////////////must look up in online calculator
    I2c._stop();
    Serial.println("Erased RAM Data");            
  }
  else{
    I2c._stop();
    Serial.println("Failed to erase data");
    return false;
  }
  
  for (int i = 0; i < 256; i++){
    I2c._start();
    I2c._sendAddress(this->address<<1);
    I2c._sendByte(ramAddress);
    I2c._sendByte(LSB);
    I2c._sendByte(MSB);

    if (I2c._sendByte(i) == 0) {            //PEC get around without having to calculate
      I2c._stop();
      return true;
    }
  }
  I2c._stop();
  Serial.println("Failure");
  return false;

  /*
  I2c._start();
  I2c._sendAddress(this->address<<1);
  I2c._sendByte(ramAddress);
  I2c._sendByte(LSB);
  I2c._sendByte(MSB);
  if (I2c._sendByte(CRC2) == 0) {            ////No idea why this doesn't write to RAM
      I2c._stop();
      return true;
  }
  I2c._stop();
  Serial.println("Failed to write to RAM");
  return false;
  */
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
