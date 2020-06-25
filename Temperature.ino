#include <I2C.h>
#include "Sensor.h"

  //uint8_t addressArr[3] = {0x5A, 0x01, 0x02};
  
  Sensor Sensor1(0x5A);
  Sensor Sensor2(0x01);
  Sensor Sensor3(0x02);
  Sensor SensorArr[3] = {Sensor1, Sensor2, Sensor3};
  //Sensor SensorArr[1] = {Sensor1};
  float ambientTemp = 0;          //might move to make every object have own ambient temp as part of class

void setup() {
  Serial.begin(9600);
  I2c.begin();
  I2c.timeOut(100);
  I2c.pullup(true);

}

void loop(){
  while (Sensor::idle == true){
    for (int i = 0; i < Sensor::numSensors; i++){                           //can't use iterators w/o STL library so will save space w/ this
      ambientTemp = SensorArr[i].readTemp(Sensor::ambientTempRegister);     //update ambient temperature
      //Serial.print("Ambient Temp Update: ");
      //Serial.println(ambientTemp);
      if (abs(SensorArr[i].readTemp(Sensor::objTempRegister) - ambientTemp) > Sensor::tempThreshold){       //leave idle if object breaks temp threshold
        Sensor::idle = false;                                                                                ///////////////////////////////////Add sensors activated and count for reset
        Sensor::prevMillis = millis();                                      //record time when leaving idle for timeout
        Serial.println("Object Detected");
        break;
      }
    }
  }

  while (Sensor::idle == false){
    for (int i = 0; i < Sensor::numSensors; i++){
      if (SensorArr[i].getActive() == true){
        Serial.println(millis());
        float objTemp = SensorArr[i].readTemp(Sensor::objTempRegister);
        if (objTemp > Sensor::maxTemp){                                     //update maxTemp if new peak found
          Sensor::maxTemp = objTemp;
          Serial.print("New Max Temp: ");
          Serial.println(objTemp);
        }
        else if (abs(objTemp - ambientTemp) < Sensor::tempThreshold){
          Serial.println("Threshold broken");
          SensorArr[i].setActive(false);
          Sensor::numInactive += 1;
        }
      }
      if (millis() - Sensor::prevMillis > Sensor::timeoutTime){             //timeout return to idle
        Sensor::idle = true;
        Serial.println("Timeout Activated");
        Serial.print("Max Temp of Object: ");
        Serial.println(Sensor::maxTemp, DEC);
        SensorArr[i].resetStatic();
        for (int i = 0; i < Sensor::numSensors; i++){                       //make all sensors active when returning to idle
          SensorArr[i].setActive(true);
        }
        break;                                                              //leave for loop and return to idle
      }
      else if (Sensor::numInactive == Sensor::numSensors){                          //no sensors active return to idle
        Sensor::idle = true;
        Serial.println("Sensors no longer detecting objects");
        Serial.print("Max Temp of Object: ");
        Serial.println(Sensor::maxTemp, DEC);
        SensorArr[i].resetStatic();
        for (int i = 0; i < Sensor::numSensors; i++){                      //make all sensors active when returning to idle
          SensorArr[i].setActive(true);
        break;                                                             //leave for loop and return to idle
        }
      }
    }
  }
}
