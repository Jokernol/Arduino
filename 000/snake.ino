#include <Adafruit_SH1106.h>

#define SCREEN_WIDTH 128  // OLED display width
#define SCREEN_HEIGHT 64  // OLED display height

#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SH1106 oled(OLED_RESET);

#define UP 9
#define DOWN 10
#define LEFT 7
#define RIGHT 8

#define BOX_WIDTH 96
#define BOX_HEIGHT 64

uint8_t snake_max_length = 99;  //蛇身体最大长度
uint8_t snake_x[100];           //蛇身x坐标
uint8_t snake_y[100];           //蛇身y坐标
uint8_t snake_body_width = 2;   //蛇身方块宽度(正方形)
uint8_t food_x;                 //食物位置坐标x
uint8_t food_y;                 //食物位置坐标y
uint8_t snake_length;           //定义初始化蛇身长度
uint8_t snake_dir = RIGHT;
uint8_t game_speed = 90;  //设置游戏速度（数值越大，贪吃蛇移动的越慢）
uint16_t score;

void setup() {
    pinMode(2, INPUT_PULLUP);
    pinMode(UP, INPUT_PULLUP);
    pinMode(DOWN, INPUT_PULLUP);
    pinMode(LEFT, INPUT_PULLUP);
    pinMode(RIGHT, INPUT_PULLUP);

    oled.begin(SH1106_SWITCHCAPVCC, 0x3C);

    attachInterrupt(digitalPinToInterrupt(2), pin2IRQ, FALLING);

    startScreen();
}

void loop() { snake(); }

void startScreen() {  // Staru up screen
    oled.clearDisplay();
    oled.setTextSize(2);  // at double size character
    oled.setTextColor(WHITE);
    oled.setCursor(7, 15);
    oled.println(F("BadGateway"));  // Title
    oled.setTextSize(1);
    oled.setCursor(25, 45);
    oled.println(F("a simple snake"));  // Version
    oled.display();                     // actual display here
    delay(1500);
    oled.clearDisplay();
}

void snake() {
    int flag = 1;  //标志
    snake_x[0] = 15;
    snake_y[0] = 15;  // snake起始坐标
    snake_x[1] = snake_x[0] - 1;
    snake_y[1] = snake_y[0];  // snake起始坐标 //定义贪吃蛇的初始的位置以及长度；
    snake_x[2] = snake_x[1] - 1;
    snake_y[2] = snake_y[1];  // snake起始坐标
    score = 0;                //
    game_speed = 90;
    snake_length = 3;
    snake_dir = RIGHT;
    food();

    while (flag) {
        snake_frame();      //将此时的贪吃蛇的状态显示在OLED 屏幕上；
        delay(game_speed);  //这里是贪吃蛇调节速度的另外一个方法；

        if (snake_eat_food(snake_dir)) {
            score++;

            food();

            if (snake_length < 10) {
                game_speed = 90;
            }
            if (snake_length < 30 && snake_length >= 10) {
                game_speed = 80;
            }
            if (snake_length < 50 && snake_length >= 30) {
                game_speed = 50;
            }
            if (snake_length >= 50) {
                game_speed = 0;
            }
        } else {
            if ((knock_wall(snake_dir)) || (eat_self(snake_dir))) {
                game_over();
                flag = 0;
            } else {
                snake_move(snake_dir);
            }
        }
    }
}

void food() {
    bool flag = 1;
    while (flag) {
        food_x = random(0, (BOX_WIDTH - 2) / 3);
        food_y = random(0, (BOX_HEIGHT - 2) / 3);
        for (int8_t i = 0; i < snake_length; i++) {
            if ((food_x == snake_x[i]) && (food_y == snake_y[i])) {
                break;
            }
            flag = 0;
        }
    }
}

void snake_frame() {
    oled.clearDisplay();
    oled.drawRect(0, 0, 96, 64, WHITE);
    oled.setTextSize(1);
    oled.setCursor(98, 12);
    oled.println(F("score"));
    oled.setCursor(110, 32);
    oled.println(score);
    oled.fillRect(food_x, food_y, 2, 2, WHITE);
    for (int8_t i = 0; i < snake_length; i++) {
        oled.fillRect(snake_x[i], snake_y[i], snake_body_width, snake_body_width, WHITE);
    }
    oled.display();
}

void snake_move(int direction) {
    for (uint8_t i = snake_length; i > 0; i--) {
        snake_x[i] = snake_x[i - 1];
        snake_y[i] = snake_y[i - 1];
    }

    switch (direction) {
        case UP: {
            snake_y[0] -= 1;
            break;
        }
        case DOWN: {
            snake_y[0] += 1;
            break;
        }
        case LEFT: {
            snake_x[0] -= 1;
            break;
        }
        case RIGHT: {
            snake_x[0] += 1;
            break;
        }
    }
}

void eat() {
    for (uint8_t i = snake_length; i > 0; i--) {
        snake_x[i] = snake_x[i - 1];
        snake_y[i] = snake_y[i - 1];
    }

    snake_x[0] = food_x;
    snake_y[0] = food_y;

    snake_length++;
}

uint8_t snake_eat_food(uint8_t direction) {
    uint8_t x = snake_x[0];
    uint8_t y = snake_y[0];

    switch (direction) {
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
    }

    if ((x == food_x) && (y == food_y)) {
        eat();
        return 1;
    } else {
        return 0;
    }
}

uint8_t knock_wall(uint8_t direction) {
    uint8_t x = snake_x[0];
    uint8_t y = snake_y[0];

    switch (direction) {
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
    }

    if ((x <= 0) || (x >= BOX_WIDTH - 1) || (y <= 0) || (y >= BOX_HEIGHT)) {
        return 1;
    } else {
        return 0;
    }
}

uint8_t eat_self(uint8_t direction) {
    uint8_t x = snake_x[0];
    uint8_t y = snake_y[0];

    switch (direction) {
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
    }

    for (uint8_t i = 1; i < snake_length; i++) {
        if ((snake_x[0] == snake_x[i]) && (snake_y[0] == snake_y[i])) {
            return 1;
        }
    }

    return 0;
}

void game_over() {
    oled.clearDisplay();
    oled.setTextSize(2);  // at double size character
    oled.setTextColor(WHITE);
    oled.setCursor(7, 15);
    oled.println(F("Game Over!"));
    oled.display();
    delay(500);
}

void pin2IRQ() {
    uint8_t x;
    x = PINB;

    if ((x & 0x04) == 0) {
        if (snake_dir != RIGHT) {
            snake_dir = LEFT;
        }
    }

    if ((x & 0x08) == 0) {
        if (snake_dir != LEFT) {
            snake_dir = RIGHT;
        }
    }

    if ((x & 0x01) == 0) {
        if (snake_dir != DOWN) {
            snake_dir = UP;
        }
    }

    if ((x & 0x02) == 0) {
        if (snake_dir != UP) {
            snake_dir = DOWN;
        }
    }
}