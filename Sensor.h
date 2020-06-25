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
    static int objectsDetected;
    static unsigned long prevMillis;
    static int timeoutTime;
    static uint8_t activatedThreshold;     //to save space
    static bool idle;                      //idle state of state machine
    static uint8_t numSensors;
    static uint8_t numInactive;
    static uint8_t tempThreshold;
    static float maxTemp;
  
    Sensor(uint8_t addy);
    float readTemp(uint8_t registerAddy);
    bool checkTimeout();
    void resetStatic();
    void setActive(bool act);
    bool getActive();
  private:
    uint8_t address;
    bool active;
};

#endif
