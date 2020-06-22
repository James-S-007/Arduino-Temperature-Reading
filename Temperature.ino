#include <I2C.h>

  int defaultAddress = 0x5A;
  int objRegister = 0x07;
  int numBytes = 3;

/*
  int tomaxAdd = 0x00;
  int tominAdd = 0x01;
  int tomax = 0;
  int tomin = 0;
*/

void setup() {
  Serial.begin(9600);
  I2c.begin();
  I2c.timeOut(100);
  I2c.pullup(true);
  csvSetup();
}

void csvSetup(){
  Serial.print("Temperature, ");
  Serial.println("Time");
}

float readSensor(int sensorAddress, int registerAddress, int numBytes){
  int errMessage = I2c.read(sensorAddress, registerAddress, numBytes);
  float seconds = millis();
  seconds = seconds/1000;                   //can't do in 1 line for some reason
  if (errMessage != 0){
    Serial.print("Error Number: ");
    Serial.print(errMessage, DEC);
    Serial.print(",");
    Serial.print("Available Bytes: ");
    Serial.println(I2c.available());
    return errMessage;
  }
  else {
    int firstByte = I2c.receive() << 8;     //uint8_t not working for some reason
    int secondByte = I2c.receive() << 8;
    int result = firstByte;
    result <<= 8;      //left shift by 8
    result |= secondByte;
    //float temp = (result*.02 - 273)*9/5+32;          //convert to C then to F via formula on datasheet
    int temp = ((result*.02 - 273)*9/5+32)*100;
    float dispTemp = temp;
    dispTemp /= 100;
    uint8_t errByte = I2c.receive() << 8;
    if (errByte == 0){
      Serial.print(dispTemp, 2);
      Serial.print(",");
      Serial.println(seconds, DEC);
    }
    /*
    if (errByte == 0){
      Serial.print("Byte 1: ");
      Serial.print(firstByte, HEX);
      Serial.print(", Byte 2: ");
      Serial.println(secondByte, HEX);
    }
    */
    else {
      Serial.print("Error Reading from Buffer");
      Serial.print(",");
      Serial.println(seconds, DEC);
    }
    return result;
  }
      
}

void loop() {
  
  readSensor(defaultAddress, objRegister, numBytes);
  delay(50);                             

}
