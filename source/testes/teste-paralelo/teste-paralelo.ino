#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include <Adafruit_NeoPixel.h>

#define PIN   6
#define NLED  10

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NLED, PIN, NEO_GRB + NEO_KHZ800);
uint8_t sensor[NLED] = {A3, 5, 4, A2, A1, A0, A4, A5, 3, 2};

void setup() {
  Serial.begin(115200);

  strip.begin();
  strip.clear();
  strip.show();

  for (uint8_t i = 0; i < NLED; ++i) {
    pinMode(sensor[i], INPUT);
  }

}

void loop() {

  for (uint16_t i = 0; i < NLED; i++) {
//    Serial.print(i);
    if (!digitalRead(sensor[i])) {
//      Serial.println(": ON");
      strip.setPixelColor(i, strip.Color(0, 255, 0));
    } else {
//      Serial.println(": OFF");
      strip.setPixelColor(i, strip.Color(0, 0, 255));
    }
  }

  strip.show();
  //  Serial.println();
  //  delay(500);

  //  for (byte i = 0; i < NLED; ++i) {
  //    strip.setPixelColor(i, strip.Color(255, 255, 255));
  //    strip.setPixelColor(i, strip.Color(255, 0, 255));
  //    strip.setPixelColor(i, strip.Color(255, 0, 0));
  //  }
  //  strip.show();

}

