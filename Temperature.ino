#include <I2C.h>
#include "Sensor.h"


int LED_B = 7;                            //Blue LED = object detected
int LED_G = 6;                            //Green LED = below max temp threshold
int LED_R = 5;                            //Red LED = above max temp threshold, connected in parallel with speaker

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

void loop(){                  //simple state machine
  idleState(SensorArr, Sensor::numSensors, Sensor::ambientTempRegister, Sensor::objTempRegister, Sensor::tempThreshold, Sensor::activateThreshold, LED_B);
  int maxTemp = objDetectedState(SensorArr, Sensor::numSensors, Sensor::objTempRegister, Sensor::tempThreshold, Sensor::deactivateThreshold, Sensor::prevMillis, Sensor::timeoutTime, LED_B);
  outputPassFail(LED_B, LED_G, LED_R, Sensor::maxTempThreshold, maxTemp);
  resetSensors(SensorArr, Sensor::numSensors);
}


int idleState(Sensor SensorArr[], int numSensors, uint8_t ambientTempRegister, uint8_t objTempRegister, int tempThreshold, uint8_t activateThreshold, int blueLED){
  while (true){
    for (int i = 0; i < numSensors; i++){
      SensorArr[i].setAmbientTemp(SensorArr[i].readTemp(Sensor::ambientTempRegister));                                                            //update ambient temperature    
      if (abs(SensorArr[i].readTemp(Sensor::objTempRegister) - SensorArr[i].getAmbientTemp()) > Sensor::tempThreshold*100){                       //object detected on sensor
        SensorArr[i].setObjectDetected(true);
        if (Sensor::numObjectsDetected >= Sensor::activateThreshold){                                                                             //leave idle if enough sensors have been activated
          Sensor::prevMillis = millis();                                                                                                          //record time when leaving idle for timeout
          digitalWrite(blueLED, HIGH);
          return 0;
        }
      }
      else{
        SensorArr[i].setObjectDetected(false);                                                                                                    //sensor not detecting object but still in idle
      }
    }
  }
}

int objDetectedState(Sensor SensorArr[], int numSensors, uint8_t objTempRegister, int tempThreshold, uint8_t deactivateThreshold, unsigned long prevMillis, int timeoutTime, int blueLED) {
  while(true){
    for (int i = 0; i < numSensors; i++){
      int objTemp = SensorArr[i].readTemp(Sensor::objTempRegister);
      if (objTemp > Sensor::maxTemp){                                                                                                             //update maxTemp if new peak found
        Sensor::maxTemp = objTemp;
        SensorArr[i].setObjectDetected(true);
      }
      if (abs(objTemp - SensorArr[i].getAmbientTemp()) < tempThreshold*100){                                                                      //object no longer detected on sensor
        SensorArr[i].setObjectDetected(false);
      }
      else{
        SensorArr[i].setObjectDetected(true);
      }
      if (millis() - prevMillis > timeoutTime){                                                                                                   //timeout activated
        Serial.println("Timeout Activated");
        digitalWrite(blueLED, LOW);
        return Sensor::maxTemp;
      }
      else if (Sensor::numObjectsDetected <= deactivateThreshold){                                                                                //enough sensors no longer detecting objects, output results and return to idle
        digitalWrite(blueLED, LOW);
        return Sensor::maxTemp;
      }
    }
  }  
}


void outputPassFail(int blueLED, int greenLED, int redLED, int maxThreshold, int maxTemp){
  Serial.print("Max Temp of Object: ");
  Serial.println(Sensor::maxTemp, DEC);
  if (maxTemp > maxThreshold*100){
    for (int i = 0; i < 3; i++){
      digitalWrite(redLED, HIGH);                                                             //flash 3 times if temp is too high (fail)
      delay(250);
      digitalWrite(redLED, LOW);
      delay(250);
    }
  }
  else {
    digitalWrite(greenLED, HIGH);                                                             //pass
    delay(1000);
    digitalWrite(greenLED, LOW);
  }
}

void resetSensors(Sensor SensorArr[], int numSensors){                                        //reset maxtemp and make sure no sensors are detecting objects
  for (int i = 0; i < Sensor::numSensors; i++){
    SensorArr[i].setObjectDetected(false);
  }
  Sensor::maxTemp = 0;
}
