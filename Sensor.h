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
    static bool idle;                      //idle state of state machine
    static uint8_t numSensors;
    static uint8_t numObjectsDetected;
    static int tempThreshold;
    static float maxTemp;
    static int maxTempThreshold;
  
    Sensor(uint8_t addy);
    int readTemp(uint8_t registerAddy);
    bool checkTimeout();
    void resetStatic();
    void setObjectDetected(bool obj);
    bool getObjectDetected();
    void setAmbientTemp(int);
    int getAmbientTemp();
  private:
    uint8_t address;
    bool active;
    bool objectDetected;
    int ambientTemp;
};

#endif
