#include <Arduino.h>

#include "DoorSensor.hpp"

// TTGO Display
TFT_eSPI tft = TFT_eSPI();

SensorBase* doorSensor = new DoorSensor(&tft);

void setup() {
  doorSensor->setup();
}

void loop() {
  doorSensor->loop();
}