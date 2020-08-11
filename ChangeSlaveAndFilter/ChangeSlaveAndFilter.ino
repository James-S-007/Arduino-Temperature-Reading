#include <I2C.h>
#include "Sensor.h"

Sensor Sensor0(0x00);
Sensor SensorArr[1] = {Sensor0};


//////////////////////
//////////////////////
//////////////////////
//this is the one thing you change
//addresses should be 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10

uint8_t newSlaveAddress = 0x10;         //Valid addresses are from 0x00 to 0x7F (supports 127 devices)

//MUST REUPLOAD EACH TIME YOU CHANGE ADDRESS
/////////////////////
/////////////////////
/////////////////////


uint8_t universalSlaveAddress = 0x00;
uint8_t SMBusAddress = 0x2E;            //EEPROM is 001xxxxx, SMBus Address is 0x0E

uint8_t MSB = 0xB4;
uint8_t LSB = 0x74;
uint8_t configCRC0 = 0x83;            //CRC8 for clearing EEPROM at configRegister1
uint8_t addressCRC0 = 0x6F;
//uint8_t CRC1[16] = {0xAF, 0xDB, 0xF7, 0x33, 0x1F, 0x6B, 0x47, 0xE4, 0xC8, 0xBC, 0x90, 0x54, 0x78, 0x0C, 0x20, 0x4D};
//uint8_t CRC2[16] = {0x5C, 0x28, 0x04, 0xC0, 0xEC, 0x98, 0xB4, 0x17, 0x3B, 0x4F, 0x63, 0xA7, 0x8B, 0xFF, 0xD3, 0xBE};
uint8_t configRegister1 = 0x25;                     ///EEPROM access always follows form 001x xxxx so must add 0x20 to whatever address you wish to access!


void setup() {
  Serial.begin(9600);
  I2c.begin();
  I2c.timeOut(100);
  I2c.pullup(true);
  Serial.println("Beginning in...");
  for (int i = 3; i > 0; i--){
    Serial.println(i);
    delay(1000);
  }
  /*
  for (int i = 0; i < 1; i++){
    Serial.println(SensorArr[i].writeRAM(configRegister1, MSB, LSB, CRC1[i], 0x00));                     //clear then write B474 to configRegister1 for all sensors
    Serial.println(SensorArr[i].readRAM(configRegister1), HEX);
    Serial.println("Power Cycle Device");
  }
  */

  //////////Changing Filter Settings from Address 0x00 (universal slave address)////////////////////////
  
  Serial.println(SensorArr[0].writeRAM(configRegister1, MSB, LSB, configCRC0));                     //clear then write B474 to configRegister1 for all sensors
  Serial.println(SensorArr[0].readRAM(configRegister1), HEX);

  /////////Changing Slave Address to newSlaveAddress///////////////////////////////////////////////////

  Serial.println(SensorArr[0].writeRAM(SMBusAddress, 0x00, newSlaveAddress, addressCRC0));
  Serial.println(SensorArr[0].readRAM(SMBusAddress), HEX);
  
  Serial.println("Power Cycle Device");
  for (int i = 5; i > 0; i--){
    Serial.println(i);
    delay(1000);
  }
}

void loop() {
  Serial.print("New Address: ");
  Serial.println(SensorArr[0].readRAM(SMBusAddress), HEX);
  Serial.print("New Filter Setting (Should be B474): ");
  Serial.println(SensorArr[0].readRAM(configRegister1), HEX);
  Serial.print("Ambient Temperature: ");
  Serial.println(SensorArr[0].readTemp(Sensor::ambientTempRegister), DEC);
  delay(2000);
}
