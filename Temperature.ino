#include <I2C.h>

  uint8_t defaultAddress = 0x01;
  uint8_t ambientRegister = 0x06;
  uint8_t objRegister = 0x07;
  int numBytes = 3;
  bool idle = true;
  float objTemp = 0;
  float ambientTemp = 0;
  int tempThreshold = 3;
  float maxTemp = 0;
  unsigned long prevMillis = 0;
  int timeout = 5000;

void setup() {
  Serial.begin(9600);
  I2c.begin();
  I2c.timeOut(100);
  I2c.pullup(true);
  //csvSetup();
}

void csvSetup(){
  Serial.print("Temperature, ");
  Serial.println("Time");
}

float readSensor(int sensorAddress, int registerAddress, int numBytes){
  int errMessage = I2c.read(sensorAddress, registerAddress, numBytes);
  //float seconds = millis();
  //seconds = seconds/1000;                   //can't do in 1 line for some reason
  if (errMessage != 0){
    Serial.print("Error Number: ");
    Serial.print(errMessage, DEC);
    Serial.print(",");
    Serial.print("Available Bytes: ");
    Serial.println(I2c.available());
    return errMessage;
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
    return dispTemp;
    //Serial.print(dispTemp, 2);
    //Serial.print(",");
    //Serial.println(seconds, DEC);
  }
      
}

void loop() {
  if (idle == true){                                                      //Idle state
    objTemp = readSensor(defaultAddress, objRegister, numBytes);          //Read object temp
    ambientTemp = readSensor(defaultAddress, ambientRegister, numBytes);  //Read ambient temp
    if (abs((objTemp - ambientTemp)) > tempThreshold){                    //Move to object detected state
      idle = false;
      Serial.println("Object Detected");
      prevMillis = millis();
      //Serial.print("Ambient Temperature for Reference: ");
      //Serial.println(ambientTemp);
    }
  }
  else if (idle == false){
    objTemp = readSensor(defaultAddress, objRegister, numBytes);      //Read object temp only
    if ((abs((objTemp - ambientTemp)) < tempThreshold) || (millis() - prevMillis > timeout)){                //Return to idle state if object temp ~= previous ambient temp or if timeout engaged
      idle = true;
      Serial.println("Object Out of View, Reentering idle state");
      Serial.print("Max Temp of Object Was: ");
      Serial.println(maxTemp);
      maxTemp = 0;
    }
    else{
      if (objTemp > maxTemp){
        maxTemp = objTemp;
        //Serial.print("New max temp: ");
        //Serial.println(maxTemp);
      }
    }
  }

  //delay(10);                             

}
