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

void readSensor(int sensorAddress, int registerAddress, int numBytes){
  int errMessage = I2c.read(sensorAddress, registerAddress, numBytes);
  float seconds = millis();
  seconds = seconds/1000;                   //can't do in 1 line for some reason
  if (errMessage != 0){
    Serial.print("Error Number: ");
    Serial.print(errMessage, DEC);
    Serial.print(",");
    Serial.print("Available Bytes: ");
    Serial.println(I2c.available());
    //return errMessage;
  }
  else {
    uint8_t LSB = I2c.receive();                          //LSB first
    uint8_t MSB = I2c.receive();
    if (MSB > 0x80){
      Serial.println("Error Flag Raised");
    }
    int result = MSB << 8;                                //MSB left shifted a byte
    result |= LSB;
    //float temp = (result*.02 - 273.15)*9/5+32;          //convert to C then to F via formula on datasheet
    int temp = ((result*.02 - 273)*9/5+32)*100;
    float dispTemp = temp;
    dispTemp /= 100;
    int PEC = I2c.receive();                              //Packet Error Code
    Serial.print(dispTemp, 2);
    Serial.print(",");
    Serial.println(seconds, DEC);
  }
      
}

void loop() {
  
  readSensor(defaultAddress, objRegister, numBytes);
  //delay(10);                             

}
