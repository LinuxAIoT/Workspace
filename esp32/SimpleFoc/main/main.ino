#include <Wire.h>
#include "device.h"

ESP32FOCDevice device;
void setup() {
    device.init();
}

void loop() {
   device.run();
}