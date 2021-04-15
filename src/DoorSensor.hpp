#ifndef DOORSENSOR_HPP
#define DOORSENSOR_HPP

#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "SensorBase.hpp"

class DoorSensor : public SensorBase
{
public:
    DoorSensor();
    DoorSensor(TFT_eSPI* tft);
    ~DoorSensor();
protected:    
    virtual void preSetupState();
    virtual void setupSetup();
    virtual void setupRuntime();
    virtual void setupSleep();
    virtual void setupReset();

    virtual void loopSetup();
    virtual void loopRuntime();
    virtual void loopSleep();
    virtual void loopReset();
};

#endif // DOORSENSOR_HPP