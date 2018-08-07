#define SHIFT_QH  11
#define SHIFT_INH 9
#define SHIFT_SH  8
#define SHIFT_CLK 12

#define NSHIFT 4

byte incoming[NSHIFT] = {0};

void setup() {
  Serial.begin(115200);

  pinMode(SHIFT_SH, OUTPUT);
  pinMode(SHIFT_INH, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_QH, INPUT);

  digitalWrite(SHIFT_CLK, HIGH);
  digitalWrite(SHIFT_SH, HIGH);

}

void loop() {
  read_shift_regs();

  for (byte i = 0; i < NSHIFT; ++i) {
    Serial.print(incoming[i], BIN);
    if (i != NSHIFT - 1) Serial.print('-');
  }
  Serial.println();

  delay(100);
}

void read_shift_regs() {
  //  byte the_shifted = 0;  // An 8 bit number to carry each bit value of A-H

  // Trigger loading the state of the A-H data lines into the shift register
  digitalWrite(SHIFT_SH, LOW);
  delayMicroseconds(5); // Requires a delay here according to the datasheet timing diagram
  digitalWrite(SHIFT_SH, HIGH);
  delayMicroseconds(5);

  // Required initial states of these two pins according to the datasheet timing diagram
  digitalWrite(SHIFT_CLK, HIGH);
  digitalWrite(SHIFT_INH, LOW); // Enable the clock

  // Get the NSHIFT*(A-H) values
  for (byte i = 0; i < NSHIFT; ++i) {
    incoming[i] = shiftIn(SHIFT_QH, SHIFT_CLK, MSBFIRST);
  }

  digitalWrite(SHIFT_INH, HIGH); // Disable the clock

  //  return the_shifted;
}

