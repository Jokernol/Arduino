#include <Adafruit_SH1106.h>

#define SCREEN_WIDTH 128  // OLED display width
#define SCREEN_HEIGHT 64  // OLED display height

#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)

#define UP 8
#define DOWN 9
#define LEFT 10
#define RIGHT 11

#define START 6

Adafruit_SH1106 oled(OLED_RESET);

uint8_t x = 32;
uint8_t y = 32;
int8_t Hspeed = 1;
int8_t Vspeed = 1;

uint8_t board_x = 56;
uint8_t board_width = 16;

bool flag = true;


void setup() {
    pinMode(2, INPUT_PULLUP);

    pinMode(START, INPUT_PULLUP);
    pinMode(UP, INPUT_PULLUP);
    pinMode(DOWN, INPUT_PULLUP);
    pinMode(LEFT, INPUT_PULLUP);
    pinMode(RIGHT, INPUT_PULLUP);

    oled.begin(SH1106_SWITCHCAPVCC, 0x3C);

    attachInterrupt(digitalPinToInterrupt(2), pin2IRQ, FALLING);

    startScreen();
}

void loop() {
    drawCommonImage();

    collisionDetection();

    if (flag) {
        drawBlock();

        drawBoard();

        oled.display();
    } else {
        game_over();
    }
}

void startScreen() {  // Staru up screen
    oled.clearDisplay();
    oled.setTextSize(2);  // at double size character
    oled.setTextColor(WHITE);
    oled.setCursor(7, 15);
    oled.println(F("BadGateway"));  // Title
    oled.setTextSize(1);
    oled.setCursor(45, 55);
    oled.println(F("Krakout"));  // Version
    oled.display();                     // actual display here
    delay(1500);
    oled.clearDisplay();
}

void drawCommonImage() {
    oled.clearDisplay();

    oled.drawFastHLine(0, 0, 128, WHITE);

    oled.drawFastVLine(0, 0, 64, WHITE);
    oled.drawFastVLine(127, 0, 64, WHITE);
}

void drawBlock() {
    x += Hspeed;
    y += Vspeed;
    oled.drawRect(x, y, 2, 2, WHITE);
}

void drawBoard(){
    oled.drawFastHLine(board_x, 63, board_width, WHITE);
}

void collisionDetection() {
    if (x == 1) {
        Hspeed = 1;
    }
    if (x == 125) {
        Hspeed = -1;
    }
    if (y == 0) {
        Vspeed = 1;
    }
    if (y == 61) {
        if (x >= board_x && x <= board_x + board_width) {
            Vspeed = -1;
        } else {
            flag = false;
        }
    }
}

void game_over() {
    oled.clearDisplay();
    oled.setTextSize(2);  // at double size character
    oled.setTextColor(WHITE);
    oled.setCursor(7, 15);
    oled.println(F("Game Over!"));
    oled.display();
    delay(500);
    
    init_game();
}

void init_game() {
    x = 32;
    y = 32;
    Hspeed = 1;
    Vspeed = 1;
    board_x = 56;
}

void pin2IRQ() {
    uint8_t x;
    x = PINB;

    if ((x & 0x04) == 0) {
        if (board_x >= 0){
            board_x -= 4;
        }
    }

    if ((x & 0x08) == 0) {
        if (board_x <= 111){
            board_x += 4;
        }
    }

    if ((y & 0x40) == 0) {
        flag = true;
    }
}