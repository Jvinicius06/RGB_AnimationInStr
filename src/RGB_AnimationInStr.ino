#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include "./animatiocnDecoder.hpp"
#define PIN 48
#define LED_COUNT 1
Adafruit_NeoPixel strip =
    Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRBW + NEO_KHZ800);

#define PINLEDR 0
#define PINLEDG 0
#define PINLEDB 0

const char* animation =
    "R255GOB0F1500,R60G174B0F1500,R255G62B255F1500,R11G62B255F1500,ROGOB255F2602";

void setup() {
    Serial.begin(115200);
    // while (!Serial) {
    // }
    delay(1000);
    Serial.println("Start");
    strip.begin();
    delay(50);
    setHandleLed([](uint8_t R, uint8_t G, uint8_t B) {
        strip.setPixelColor(0, strip.Color(R, G, B, 100));
        strip.show();
    });
    setAnimation(animation, strlen(animation));
    startAnimation();
}

void loop() {
    loopAnimation();
}