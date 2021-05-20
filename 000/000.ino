#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <Adafruit_SH1106.h>
#include <Wire.h>

#define SCREEN_WIDTH 128     // OLED display width
#define SCREEN_HEIGHT 64     // OLED display height
#define SCREEN_ADDRESS 0x3D  // OLED display address

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
    Serial.begin(115200);
}

void loop() {
    trigVoltage();
    delay(500);
}

void trigVoltage() {
    long y;
    float trigVoltage;
    display.clearDisplay();
    trigVoltage = analogRead(1);
    y = map(trigVoltage, 0, 1023, 63, 9);
    Serial.print(trigVoltage);
    Serial.print(" ");
    Serial.println(y);
    y = constrain(y, 9, 63);
    display.drawFastHLine(27, y, 100, WHITE);
    display.display();
}