#include <I2C.h>
#include "Sensor.h"


int LED_B = 7;
int LED_G = 6;
int LED_R = 5;                            //Connected in parallel with speaker

Sensor Sensor1(0x5A);
Sensor Sensor2(0x01);
Sensor Sensor3(0x02);
Sensor SensorArr[3] = {Sensor1, Sensor2, Sensor3};

void setup() {
  pinMode(LED_B, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  Serial.begin(9600);
  I2c.begin();
  I2c.timeOut(100);
  I2c.pullup(true);

}

//////////////////195.69 on sensor 5A

void loop(){
  while (Sensor::idle == true){  
    for (int i = 0; i < Sensor::numSensors; i++){                                                         //can't use iterators w/o STL library so will save space w/ this
      SensorArr[i].setAmbientTemp(SensorArr[i].readTemp(Sensor::ambientTempRegister));                    //update ambient temperature

      //Serial.println(SensorArr[i].readTemp(Sensor::ambientTempRegister));
      
      if (abs(SensorArr[i].readTemp(Sensor::objTempRegister) - SensorArr[i].getAmbientTemp()) > Sensor::tempThreshold*100){       //leave idle if object breaks temp threshold
        SensorArr[i].setObjectDetected(true);
        if (Sensor::numObjectsDetected >= Sensor::activateThreshold){
          Sensor::idle = false;                                                                                ///////////////////////////////////Add sensors activated and count for reset
          Sensor::prevMillis = millis();                                                                    //record time when leaving idle for timeout
          //Serial.println("Object Detected");
          digitalWrite(LED_B, HIGH);
          break;
        }
      }
      else{
        SensorArr[i].setObjectDetected(false);                                                              //sensor no longer detecting object but still in idle
      }
    }
  }

  while (Sensor::idle == false){
    for (int i = 0; i < Sensor::numSensors; i++){
      int objTemp = SensorArr[i].readTemp(Sensor::objTempRegister);
      if (objTemp > Sensor::maxTemp){                                     //update maxTemp if new peak found
        Sensor::maxTemp = objTemp;
        SensorArr[i].setObjectDetected(true);
        //Serial.print("New Max Temp: ");
        //Serial.println(objTemp);
      }
      if (abs(objTemp - SensorArr[i].getAmbientTemp()) < Sensor::tempThreshold*100){       //object no longer detected on sensor-->stop sampling that sensor
        SensorArr[i].setObjectDetected(false);
      }
      else{
        SensorArr[i].setObjectDetected(true);
      }
      if (millis() - Sensor::prevMillis > Sensor::timeoutTime){             //timeout return to idle
        Sensor::idle = true;
        digitalWrite(LED_B, LOW);
        Serial.println("Timeout Activated");
        Serial.print("Max Temp of Object: ");
        Serial.println(Sensor::maxTemp, DEC);
        outputPassFail(LED_G, LED_R, Sensor::maxTempThreshold, Sensor::maxTemp);
        SensorArr[i].resetStatic();
        for (int i = 0; i < Sensor::numSensors; i++){                       //make all sensors active when returning to idle
          SensorArr[i].setObjectDetected(false);
        }
        break;                                                              //leave for loop and return to idle
      }
      else if (Sensor::numObjectsDetected <= Sensor::deactivateThreshold){                          //no sensors active return to idle
        Sensor::idle = true;
        digitalWrite(LED_B, LOW);
        Serial.println("Sensors no longer detecting objects");
        Serial.print("Max Temp of Object: ");
        Serial.println(Sensor::maxTemp, DEC);
        outputPassFail(LED_G, LED_R, Sensor::maxTempThreshold, Sensor::maxTemp);
        SensorArr[i].resetStatic();
        for (int i = 0; i < Sensor::numSensors; i++){                      //make all sensors active when returning to idle
          SensorArr[i].setObjectDetected(false);
        }
        break;                                                             //leave for loop and return to idle
      }
    }
  }
}

void outputPassFail(int greenPIN, int redPIN, int maxThreshold, int maxTemp){
  if (maxTemp > maxThreshold*100){
    for (int i = 0; i < 3; i++){
      digitalWrite(redPIN, HIGH);              //flash 3 times if temp is too high
      delay(250);
      digitalWrite(redPIN, LOW);
      delay(250);
    }
  }
  else {
    digitalWrite(greenPIN, HIGH);              //pass
    delay(1000);
    digitalWrite(greenPIN, LOW);
  }
}
