#include <I2C.h>
#include "Sensor.h"


int LED_B = 7;                            //Blue LED = object detected
int LED_G = 6;                            //Green LED = below max temp threshold
int LED_R = 5;                            //Red LED = above max temp threshold, connected in parallel with speaker

int trial = 0;

double exitTime = 0;

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

//Sensor SensorArr[7] = {Sensor1, Sensor2, Sensor4, Sensor5, Sensor6, Sensor7, Sensor8};
void setup() {
  pinMode(LED_B, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  Serial.begin(9600);
  Serial.println("Initializing");
  I2c.begin();
  I2c.timeOut(100);
  I2c.pullup(true);
  initialization(SensorArr);
  Serial.println("Completed Initialization");
  Serial.println("Trial, Sen1, Sen2, Sen3, Sen4, Sen5, Sen6, Sen7, Sen8, Sen9, Sen10, Sen11, Sen12, Sen13, Sen14, Sen15, Sen16, Max Temp, Ambient Temp, Max Sen, Trigger Sen, Time");
}

//////////////////195.69 on sensor 5A

void loop(){                  //simple state machine
  idleState(SensorArr, Sensor::numSensors, Sensor::ambientTempRegister, Sensor::objTempRegister, Sensor::tempThreshold, Sensor::activateThreshold, LED_B);
  int maxTemp = objDetectedState(SensorArr, Sensor::numSensors, Sensor::objTempRegister, Sensor::tempThreshold, Sensor::deactivateThreshold, Sensor::prevMillis, Sensor::timeoutTime, LED_B);
  outputPassFail(LED_B, LED_G, LED_R, Sensor::maxTempThreshold, maxTemp);
  outputSensorTemps(SensorArr);
  resetSensors(SensorArr, Sensor::numSensors);
}

void initialization(Sensor SensorArr[]){
  for (int i = 0; i < Sensor::numSensors; i++){
    unsigned long sum = 0;
    for (int j = 0; j < 10; j++){
      sum += SensorArr[i].readTemp(Sensor::ambientTempRegister);
    }
    sum /= 10;
    SensorArr[i].setAmbientTemp(sum);
    //Serial.print("Sensor");
    //Serial.print(i+1, DEC);
    //Serial.print(" Initial Ambient Temp: ");
    //Serial.println(sum, DEC);
  }
}

int idleState(Sensor SensorArr[], int numSensors, uint8_t ambientTempRegister, uint8_t objTempRegister, int tempThreshold, uint8_t activateThreshold, int blueLED){
  while (true){
    for (int i = 0; i < numSensors; i++){
      SensorArr[i].updateAmbientTemp(SensorArr[i].readTemp(Sensor::ambientTempRegister));                                                            //update ambient temperature    
      if (abs(SensorArr[i].readTemp(Sensor::objTempRegister) - SensorArr[i].getAmbientTemp()) > Sensor::tempThreshold*100){                       //object detected on sensor
        SensorArr[i].setObjectDetected(true);
        if (Sensor::numObjectsDetected >= Sensor::activateThreshold){                                                                             //leave idle if enough sensors have been activated
          Sensor::prevMillis = millis();                                                                                                        //record time when leaving idle for timeout
          Sensor::triggerSensor = SensorArr[i].getAddy();
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
        Sensor::maxSensor = SensorArr[i].getAddy();
        SensorArr[i].setObjectDetected(true);
      }
      if (objTemp > SensorArr[i].getMaxTempSensor()){
        SensorArr[i].setMaxTempSensor(objTemp);
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
        exitTime = millis();
        exitTime /= 1000;
        return Sensor::maxTemp;
      }
      if (Sensor::numObjectsDetected <= deactivateThreshold){                                                                                //enough sensors no longer detecting objects, output results and return to idle
        digitalWrite(blueLED, LOW);
        exitTime = millis();
        exitTime /= 1000;
        return Sensor::maxTemp;
      }
    }
  }  
}


void outputPassFail(int blueLED, int greenLED, int redLED, int maxThreshold, int maxTemp){
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

void outputSensorTemps(Sensor SensorArr[]){
  trial += 1;
  unsigned long avgAmbients = 0;
  Serial.print(trial, DEC);
  Serial.print(", ");
  for (int i = 0; i < Sensor::numSensors; i++){
    Serial.print(SensorArr[i].getMaxTempSensor(), DEC);
    Serial.print(", ");
    avgAmbients += SensorArr[i].getAmbientTemp();
  }
  avgAmbients /= 16;
  Serial.print((Sensor::maxTemp), DEC);
  Serial.print(", ");
  Serial.print(avgAmbients, DEC);
  Serial.print(", ");
  Serial.print(Sensor::maxSensor, DEC);
  Serial.print(", ");
  Serial.print(Sensor::triggerSensor, DEC);
  Serial.print(", ");
  Serial.println(exitTime, DEC);
}

void resetSensors(Sensor SensorArr[], int numSensors){                                        //reset maxtemp and make sure no sensors are detecting objects
  for (int i = 0; i < Sensor::numSensors; i++){
    SensorArr[i].setObjectDetected(false);
    SensorArr[i].setMaxTempSensor(0);
  }
  Sensor::maxTemp = 0;
}
