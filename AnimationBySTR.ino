#include <Arduino.h>

#include "./animatiocnDecoder.hpp"

#define PINLEDR 0
#define PINLEDG 0
#define PINLEDB 0

const char* animation =
    "R255B100G0F1500W500,"
    "R200W500,"
    "R240G40F100,";

void setup() {
    Serial.begin(115200);
    // while (!Serial) {
    // }
    delay(5000);
    Serial.println("Start");
    setAnimation(animation, strlen(animation));
}

void loop() { loopAnimation(); }