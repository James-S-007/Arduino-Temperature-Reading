#ifndef SENSOR_H
#define SENSOR_H

#include "Arduino.h"

/*
 * Class for MLX90614 Sensor
 * By: James Springer
 * 
 */

class Sensor
{
  public:  
    static const uint8_t ambientTempRegister;
    static const uint8_t objTempRegister;
    static unsigned long prevMillis;
    static int timeoutTime;
    static uint8_t activateThreshold;     //to save space
    static uint8_t deactivateThreshold;
    static uint8_t numSensors;
    static uint8_t numObjectsDetected;
    static int tempThreshold;
    static int maxTemp;
    static int maxTempThreshold;
    static uint8_t maxSensor;
    static uint8_t triggerSensor;
  
    Sensor(uint8_t);
    int readTemp(uint8_t);
    uint8_t getAddy();
    bool checkTimeout();
    void setObjectDetected(bool);
    bool getObjectDetected();
    void setMaxTempSensor(int);
    int getMaxTempSensor();
    void setAmbientTemp(int);
    int getAmbientTemp();
    void updateAmbientTemp(int);

  private:
    uint8_t address;
    bool active;
    bool objectDetected;
    int ambientTemp;
    int maxTempSensor;
    
    //int ambientArr[10];
    //uint8_t index;
};

#endif
