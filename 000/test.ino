#define START 6

void setup() {
    Serial.begin(9600);

    pinMode(2, INPUT_PULLUP);
    pinMode(START, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(2), pin2IRQ, FALLING);
}

void loop() {}

void pin2IRQ() {
    uint8_t y = PIND;
    Serial.println(y);
}