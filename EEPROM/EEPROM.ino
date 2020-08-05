#include <I2C.h>
#include "Sensor.h"

Sensor Sensork(0x10);
int k = 16;
/*
Sensor Sensor1(0x01);
Sensor Sensor2(0x02);
Sensor Sensor3(0x03);
Sensor Sensor4(0x04);
Sensor Sensor5(0x05);
Sensor Sensor6(0x06);
Sensor Sensor7(0x07);
Sensor Sensor8(0x08);
Sensor Sensor9(0x09);
Sensor SensorA(0x0A);
Sensor SensorB(0x0B);
Sensor SensorC(0x0C);
Sensor SensorD(0x0D);
Sensor SensorE(0x0E);
Sensor SensorF(0x0F);
Sensor Sensor10(0x10);

Sensor SensorArr[16] = {Sensor1, Sensor2, Sensor3, Sensor4, Sensor5, Sensor6, Sensor7, Sensor8, Sensor9, SensorA, SensorB, SensorC, SensorD, SensorE, SensorF, Sensor10};
*/

//Sensor SensorArr[8] = {Sensor1, Sensor2, Sensor3, Sensor4, Sensor5, Sensor6, Sensor7, Sensor8};
Sensor SensorArr[1] = {Sensork};

uint8_t MSB = 0xB4;
uint8_t LSB = 0x74;

uint8_t CRC1[16] = {0xAF, 0xDB, 0xF7, 0x33, 0x1F, 0x6B, 0x47, 0xE4, 0xC8, 0xBC, 0x90, 0x54, 0x78, 0x0C, 0x20, 0x4D};
uint8_t CRC2[16] = {0x5C, 0x28, 0x04, 0xC0, 0xEC, 0x98, 0xB4, 0x17, 0x3B, 0x4F, 0x63, 0xA7, 0x8B, 0xFF, 0xD3, 0xBE};
uint8_t configRegister1 = 0x25;                     ///////////////EEPROM access always follows form 001x xxxx so must add 0x20 to whatever address you wish to access!
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
  Serial.println(SensorArr[0].writeRAM(configRegister1, MSB, LSB, CRC1[k-1], 0x00));                     //clear then write B474 to configRegister1 for all sensors
  Serial.println(SensorArr[0].readRAM(configRegister1), HEX);
  Serial.println("Power Cycle Device");
  for (int i = 3; i > 0; i--){
    Serial.println(i);
    delay(1000);
  }
}

void loop() {
  for (int i=0; i < Sensor::numSensors; i++){
    Serial.println(SensorArr[i].readTemp(Sensor::ambientTempRegister), DEC);
    delay(100);
    Serial.println(SensorArr[i].readRAM(configRegister1), HEX);
    delay(100);
    Serial.println(SensorArr[i].readRAM(0x2E), HEX);
  }
  delay(10000);
}
