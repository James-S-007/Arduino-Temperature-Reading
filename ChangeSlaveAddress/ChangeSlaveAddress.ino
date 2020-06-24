/*
Most of code based on Chris Ramsay's article using the Arduino with the MLX90614
https://chrisramsay.co.uk/posts/2017/09/arduino-and-multiple-mlx90614-sensors-take-two/#id5
*/
#include <I2C.h>

  uint8_t defaultSlaveAddress = 0x5A;
  uint8_t universalSlaveAddress = 0x00;
  uint8_t newSlaveAddress = 0x01;         //Valid addresses are from 0x00 to 0x7F (supports 127 devices)
  uint8_t SMBusAddress = 0x2E;            //EEPROM is 001xxxxx, SMBus Address is 0x0E so maybe the sum? (2E)

  uint8_t objRegister = 0x07;

void setup() {
  Serial.begin(9600);
  I2c.begin();
  I2c.timeOut(100);
  I2c.pullup(true);
  delay(5000);

  Serial.println("Reading SMBusAddress on Universal SA");
  uint16_t origAddress = readSMBusAddress(universalSlaveAddress);
  Serial.print("SMBusAddress: ");
  Serial.println(origAddress, HEX);
  Serial.println("Updating Address");
  changeSMBusAddress(0x00, newSlaveAddress);
  uint16_t newAddress = readSMBusAddress(universalSlaveAddress);
  Serial.print("Updated Address is : ");
  Serial.println(newAddress, HEX);
  Serial.println("Power Cycle Device");
  Serial.println("Testing will resume in 10 seconds");
  delay(10000);
  Serial.println("Attempting to read SMBus Address with new address");
  newAddress = readSMBusAddress(newSlaveAddress);
  Serial.println("SMBus Address: ");
  Serial.println(newAddress, HEX);
}

void loop() {
  uint16_t newAddress = readSMBusAddress(universalSlaveAddress);
  Serial.println(newAddress, HEX);
  delay(2000);
}

uint16_t readSMBusAddress(uint8_t addr){
  int errMessage = I2c.read(addr, 0x2E, 3);
  if (errMessage != 0) {
    Serial.print("Error Number: ");
    Serial.println(errMessage, DEC);
    delay(1000);
  }
  else {
    uint8_t LSB = I2c.receive();
    uint8_t MSB = I2c.receive();
    if (MSB > 0x80){
      Serial.println("Error Flag Raised");
    }
    uint16_t result = MSB << 8;                                //MSB left shifted a byte
    result |= LSB;
    int PEC = I2c.receive();                              //Packet Error Code
    //Serial.println(result, HEX);
    return result;
  }
}

int changeSMBusAddress(uint8_t MSB, uint8_t LSB){
  I2c._start();
  I2c._sendAddress(0x00);
  I2c._sendByte(0x2E);          //SMBus EEPROM address
  I2c._sendByte(0x00); 
  I2c._sendByte(0x00);          //Clearing EEPROM contents at location 2E

  if (I2c._sendByte(0x6F) == 0){        //PEC
    I2c._stop();
    Serial.println("changeSMBusAddress successfully erased data");
  }
  else{
      I2c._stop();
      Serial.println("Failed to erase data");
      return -1;
  }

  for (int i = 0; i < 256; i++){
    I2c._start();
    I2c._sendAddress(0x00);
    I2c._sendByte(0x2E);
    I2c._sendByte(LSB);
    I2c._sendByte(MSB);

    if (I2c._sendByte(i) == 0) {            //PEC get around without having to calculate
      I2c._stop();
      return i;
    }
  }
  I2c._stop();
  Serial.println("Failure");
  return -1;
}
