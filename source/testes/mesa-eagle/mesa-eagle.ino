// Add 300 - 500 Ohm resistor on first pixel's data input

//https://learn.sparkfun.com/tutorials/shift-registers

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define IR_SENS   6

#define SHIFT_QH  11
#define SHIFT_INH 9
#define SHIFT_SH  8
#define SHIFT_CLK 12

#define LEDPIN    3
#define NLEDS     6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NLEDS, LEDPIN, NEO_GRB + NEO_KHZ800);

uint32_t color1 = 0xFF0000;
uint32_t color2 = 0x0000FF;

void setup() {
  pinMode(SHIFT_SH, OUTPUT);
  pinMode(SHIFT_INH, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_QH, INPUT);

  digitalWrite(SHIFT_CLK, HIGH);
  digitalWrite(SHIFT_SH, HIGH);

  strip.begin();
  strip.clear();
  strip.show();
}

void loop() {
  byte incoming = read_shift_regs();

  for (uint8_t i = 0; i < NLEDS; i++) {
    byte n = (incoming >> i) & 0x01;
    uint32_t ledColor = (n) ? (LerpColor(color1, color2, float(i) / NLEDS)) : (0x000000);
    strip.setPixelColor(i, ledColor);
  }
  strip.show();
}

byte read_shift_regs() {
  byte the_shifted = 0;  // An 8 bit number to carry each bit value of A-H

  // Trigger loading the state of the A-H data lines into the shift register
  digitalWrite(SHIFT_SH, LOW);
  delayMicroseconds(5); // Requires a delay here according to the datasheet timing diagram
  digitalWrite(SHIFT_SH, HIGH);
  delayMicroseconds(5);

  // Required initial states of these two pins according to the datasheet timing diagram
  digitalWrite(SHIFT_CLK, HIGH);
  digitalWrite(SHIFT_INH, LOW); // Enable the clock

  // Get the A-H values
  the_shifted = shiftIn(SHIFT_QH, SHIFT_CLK, MSBFIRST);
  digitalWrite(SHIFT_INH, HIGH); // Disable the clock

  return the_shifted;
}

uint32_t LerpColor(const uint32_t& from, const uint32_t& to, float amount) {
  if (amount < 0) amount = 0;
  if (amount > 1) amount = 1;

  float a1 = ((from >> 24) & 0xff);
  float r1 = (from >> 16) & 0xff;
  float g1 = (from >> 8) & 0xff;
  float b1 = from & 0xff;
  float a2 = (to >> 24) & 0xff;
  float r2 = (to >> 16) & 0xff;
  float g2 = (to >> 8) & 0xff;
  float b2 = to & 0xff;

  return ((round(a1 + (a2 - a1) * amount) << 24) |
          (round(r1 + (r2 - r1) * amount) << 16) |
          (round(g1 + (g2 - g1) * amount) << 8)  |
          (round(b1 + (b2 - b1) * amount)));
}

