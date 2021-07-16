#include <U8glib.h>  //引用U8G头文件

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);  //设置设备名称：I2C-SSD1306-128*64(OLED)
//
#define UP 15
#define DOWN 16
#define LEFT 14
#define RIGHT 17
#define analogRead_num 950
#define delay_time 300
/************************/
int box_x = 0;                 //定义变量
int box_y = 0;                 //定义变量
int box_x_length = 126;        // x方向
int box_y_length = 62;         // y方向
int snake_max_length = 100;    //蛇身体最大长度
int snake_x[100];              //蛇身x坐标
int snake_y[100];              //蛇身y坐标
int snake_body_width = 3;      //蛇身方块宽度（正方形）
int food_x;                    //食物位置坐标x
int food_y;                    //食物位置坐标y
int snake_length = 3;          //定义初始化蛇身长度
unsigned int game_speed = 90;  //设置游戏速度（数值越大，贪吃蛇移动的越慢）

const uint8_t ji[] PROGMEM = {  //封装汉子“”的数组；
    0x08, 0x00, 0x88, 0x0F, 0x88, 0x08, 0x88, 0x08, 0xBF, 0x08, 0x88, 0x08, 0x8C, 0x08, 0x9C, 0x08, 0xAA, 0x08, 0xAA, 0x08, 0x89, 0x08, 0x88, 0x48, 0x88, 0x48, 0x48, 0x48, 0x48, 0x70, 0x28, 0x00};
const uint8_t qi[] PROGMEM = {  //封装汉子“”的数组；
    0x00, 0x00, 0x7C, 0x3E, 0x44, 0x22, 0x44, 0x22, 0x7C, 0x3E, 0x80, 0x04, 0x80, 0x08, 0xFF, 0x7F, 0x40, 0x01, 0x30, 0x06, 0x0C, 0x18, 0x03, 0x60, 0x7C, 0x3E, 0x44, 0x22, 0x44, 0x22, 0x7C, 0x3E};
const uint8_t dong[] PROGMEM = {  //封装汉子“”的数组；
    0x00, 0x02, 0x00, 0x02, 0x3E, 0x02, 0x00, 0x02, 0x80, 0x3F, 0x00, 0x22, 0x7F, 0x22, 0x04, 0x22, 0x04, 0x22, 0x04, 0x21, 0x12, 0x21, 0x22, 0x21, 0xBF, 0x20, 0xA2, 0x20, 0x40, 0x14, 0x20, 0x08};
const uint8_t li[] PROGMEM = {  //封装汉子“”的数组；
    0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0xFE, 0x1F, 0x40, 0x10, 0x40, 0x10, 0x40, 0x10, 0x40, 0x10, 0x20, 0x10, 0x20, 0x10, 0x10, 0x10, 0x10, 0x10, 0x08, 0x11, 0x04, 0x0A, 0x02, 0x04};
/***********************************************************************/
void setup(void) {
    Serial.begin(9600);           //打开串口；
    pinMode(UP, INPUT_PULLUP);    //定义控制贪吃蛇上下的传感器引脚为输入模式，这里的INPUT_PULLUP意思是将该引脚的电平拉高；
    pinMode(LEFT, INPUT_PULLUP);  //定义控制贪吃蛇左右的传感器引脚为输入模式，这里的INPUT_PULLUP意思是将该引脚的电平拉高；
    welcome();                    //欢迎界面；
    delay(1000);
}

void show_init_one_one()  //
{
    String shangbiao = “JiQiDongLi”;

    for (int i = 0; i < 40; i++) {
        u8g.firstPage();
        do {
            u8g.setPrintPos(15 + i, 55);
            u8g.print(shangbiao);
        } while (u8g.nextPage());
    }
}

//程序初始化需要显示的内容=============================
void show_init_one()             //
{                                //
    for (int i = 0; i < 4; i++)  //
    {                            //
        u8g.firstPage();         //打开加载界面； //
        do {                     //
            u8g.setFont(u8g_font_gdr14r);
            u8g.setPrintPos(0, 20);
            u8g.print(“Snake Game”);  //在OLED 屏幕上显示”Snake Game”；【u8g.setFont(字体);表示要显示的是哪种字体， //
            // u8g.setPrintPos(x,y);表示要显示的位置，u8g.print(“内容”);表示在OLED 屏幕上显示“内容”；】 //
            u8g.setFont(u8g_font_9x18);
            u8g.setPrintPos(15, 55);
            u8g.print(“JiQiDongLi”);  //用u8g_font_9x18的字体在OLED 屏幕上的（15,55）显示"JiQiDongLi". //
        } while (u8g.nextPage());     //画面加载完毕； （这里使用了do…while（）语句，先执行do中的语句，再判断while（）中的语句是否符合条件，如果符合，跳出该循环，否则，继续执行循环） //
        delay(delay_time);            //
    }                                 //
    //
    for (int i = 0; i < 100; i++)  //
    {                              //
        u8g.firstPage();           //
        do {                       //
            u8g.setFont(u8g_font_gdr14r);
            u8g.setPrintPos(0, 20);
            u8g.print(“Snake Game”);                 //在OLED 屏幕上显示”Snake Game”；【u8g.setFont(字体);表示要显示的是哪种字体， //
            u8g.drawXBMP(25 + i, 45, 16, 16, ji);    //在OLED 屏幕显示“”； //
            u8g.drawXBMP(45 + i, 45, 16, 16, qi);    //在OLED 屏幕显示“”； //
            u8g.drawXBMP(65 + i, 45, 16, 16, dong);  //在OLED 屏幕显示“”； //
            u8g.drawXBMP(85 + i, 45, 16, 16, li);    //在OLED 屏幕显示“”； //
        } while (u8g.nextPage());                    //
    }                                                //
}  //
//
void show_init_two()               //
{                                  //
    for (int i = 0; i < 2; i++) {  //显示"Ready." //
        u8g.firstPage();           //
        do {                       //
            u8g.setFont(u8g_font_9x18);
            u8g.setPrintPos(25, 42);
            u8g.print(“Ready”);         //用u8g_font_9x18的字体在OLED 屏幕上的（25,42）显示"Ready". //
            u8g.drawBox(70, 42, 3, 3);  //紧接着在（70,42）显示宽和高都为3个像素的实心矩形；显示的 //
            //整体效果为“Ready." //
        } while (u8g.nextPage());  //
        delay(delay_time);         //
    }                              //
    for (int i = 0; i < 2; i++) {  //显示“Ready…" //
        u8g.firstPage();           //
        do {                       //
            u8g.setFont(u8g_font_9x18);
            u8g.setPrintPos(25, 42);
            u8g.print(“Ready”);  //用u8g_font_9x18的字体在OLED 屏幕上的（15,55）显示"Ready". //
            u8g.drawBox(70, 42, 3, 3);
            u8g.drawBox(74, 42, 3, 3);  //紧接着在（70,42）和（74,42）显示宽和高都为3个像素的两个 //
            //实心矩形；显示的整体效果为“Ready…” //
        } while (u8g.nextPage());  //
        delay(delay_time);         //
    }                              //
    for (int i = 0; i < 2; i++) {  //显示"Ready…" //
        u8g.firstPage();           //
        do {                       //
            u8g.setFont(u8g_font_9x18);
            u8g.setPrintPos(25, 42);
            u8g.print(“Ready”);  //
            u8g.drawBox(70, 42, 3, 3);
            u8g.drawBox(74, 42, 3, 3);
            u8g.drawBox(78, 42, 3, 3);  //同理，显示的整体效果为“Ready…” //
        } while (u8g.nextPage());       //
        delay(delay_time);              //
    }                                   //
}  //
//
void show_init_three()             //初始化显示的第三部分（该部分主要是显示“GO！”） //
{                                  //
    for (int i = 0; i < 2; i++) {  //
        u8g.firstPage();           //
        do {                       //
            u8g.setFont(u8g_font_9x18);
            u8g.setPrintPos(50, 42);
            u8g.print(“GO !”);     //
        } while (u8g.nextPage());  //
        delay(delay_time);         //
    }                              //
}  //
//============================

/************************************************
程序刚开始要显示的部分
************************************************/
void welcome()  //开机需要执行的程序；
{
    // while(digitalRead(UP)) {show_init_one();} //加载开机的第一个界面；
    show_init_one();
    // show_init_one_one();
    show_init_two();    //显示"Ready." 显示"Ready…" 显示"Ready…"
    show_init_three();  //显示"GO!"
}

/************************************************
读取传感器的状态，并返回相应的数值
************************************************/
int read_key() {
    int key_temp;
    if (digitalRead(UP) = LOW) {
        key_temp = UP;
        return key_temp;
    }
    if (analogRead(UP) >= analogRead_num) {
        key_temp = DOWN;
        return key_temp;
    }
    if (digitalRead(LEFT) == LOW) {
        key_temp = LEFT;
        return key_temp;
    }
    if (analogRead(LEFT) >= analogRead_num) {
        key_temp = RIGHT;
        return key_temp;
    }
    return 0;
}

/************************************************
| GAME OVER!（对游戏结束进行介绍） |
************************************************/
void game_over()  //对游戏结束进行定义
{
    for (int i = 0; i < 2; i++) {
        u8g.firstPage();
        do {
            u8g.setFont(u8g_font_gdr14r);
            u8g.setPrintPos(0, 40);
            u8g.print(" GAME OVER!");
        }  //在OLED 屏幕上的（0,40）的位置显示“GAME OVER!”
        while (u8g.nextPage());
        delay(500);
    }
    snake_length = 3;  //=============================
    snake_x[0] = 15;
    snake_y[0] = 15;  // snake起始坐标 //这几步代表的意思是恢复贪吃蛇
    snake_x[1] = snake_x[0] - 1;
    snake_y[1] = snake_y[0];  // snake起始坐标 //初始的状态（包括长度和位置）
    snake_x[2] = snake_x[1] - 1;
    snake_y[2] = snake_y[1];  // snake起始坐标 //=============================
}

/************************************************
对贪吃蛇参数进行定义
************************************************/
void snake() {
    int flag = 1;  //标志
    snake_x[0] = 15;
    snake_y[0] = 15;  // snake起始坐标
    snake_x[1] = snake_x[0] - 1;
    snake_y[1] = snake_y[0];  // snake起始坐标 //定义贪吃蛇的初始的位置以及长度；
    snake_x[2] = snake_x[1] - 1;
    snake_y[2] = snake_y[1];  // snake起始坐标
    int snake_dir = RIGHT;    //初始方向 right
    game_speed = 90;
    int food_flag = 1;
    Serial.println(“snake !”);
    food();
    while (flag) {
        snake_frame();       //将此时的贪吃蛇的状态显示在OLED 屏幕上；
        delay(game_speed);   //这里是贪吃蛇调节速度的另外一个方法；
        switch (read_key())  //此时读取摇杆模块的摆动方位的状态（分为上下左右四个摆动方位）；
        {
            case UP:
                if (snake_dir != DOWN) {
                    snake_dir = UP;
                    break;
                }  //================================================================
            case DOWN:
                if (snake_dir != UP) {
                    snake_dir = DOWN;
                    break;
                }  //这四行代码表示：摇杆摆动到不同位置时，变量snake_dir所对应的数值；
            case LEFT:
                if (snake_dir != RIGHT) {
                    snake_dir = LEFT;
                    break;
                }  //如果摇杆向右摆动，那么此时snake_dir=RIGHT(17);
            case RIGHT:
                if (snake_dir != LEFT) {
                    snake_dir = RIGHT;
                    break;
                }
            default:
                break;
        }
        if (snake_eat_food(snake_dir) = 1) {
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
            if (snake_knock_wall(snake_dir = 1)) {
                game_over();
                flag = 0;
            } else {
                if (snake_eat_body(snake_dir) = 1)  //=================================================================
                {                                   //判断贪吃蛇是否吃到自己的身体，
                    game_over();                    //如果吃到，游戏结束；
                    flag = 0;                       //=================================================================
                } else {                            //=================================================================
                    change_con(snake_dir);          //如果贪吃蛇没有吃到食物，没有撞墙，没有吃到自己的身体，那么贪吃蛇
                }                                   //执行change_con(snake_dir);函数；
            }                                       //=================================================================
        }
    }
}

/************************************************
显示部分
**********************************************/
void snake_frame() {
    u8g.firstPage();
    do {
        u8g.drawFrame(box_x, box_y, 126, 62);                                                                       //显示宽为126，高为62的空心矩形框
        u8g.drawFrame(food_xsnake_body_width + 1, food_ysnake_body_width + 1, snake_body_width, snake_body_width);  //显示食物
        for (int i = 0; i < snake_length; i++)                                                                      //显示贪吃蛇的身体函数；
        {
            if (i == 0) {
                u8g.drawFrame(snake_x[i] * snake_body_width + 1, snake_y[i] * snake_body_width + 1, snake_body_width, snake_body_width);
            } else {
                u8g.drawBox(snake_x[i] * snake_body_width + 1, snake_y[i] * snake_body_width + 1, snake_body_width, snake_body_width);
            }
        }
    } while (u8g.nextPage());
}

/************************************************
对实物进行定义
************************************************/
void food() {
    int flag = 1;
    while (flag) {
        food_x = random(0, (box_x_length - 2) / 3);
        food_y = random(0, (box_y_length - 2) / 3);  //这里用到了random()函数，会随机显示食物
        for (int i = 0; i < snake_length; i++) {
            if ((food_x = snake_x[i]) && (food_y == snake_y[i])) {
                break;
            }
            flag = 0;
        }
    }
}

/************************************************
移动坐标函数
************************************************/
void change_con(int DIR)  //如果贪吃蛇没有吃到食物时，贪吃蛇移时的函数封装；
{
    int temp_x[snake_length + 2];               //定义一个临时存储贪吃蛇整个身体横坐标的数组；
    int temp_y[snake_length + 2];               //定义一个临时存储贪吃蛇整个身体纵坐标的数组；
    for (int i = 0; i < snake_length - 1; i++)  //将旧坐标数据存放到temp数组 最后一位数据无效；
    {
        temp_x[i] = snake_x[i];  //
        temp_y[i] = snake_y[i];  //这两个步骤是将贪吃蛇整个身体的横纵坐标存储到临时定义的两个数组中；
    }
    switch (DIR)  //判断贪吃蛇处于摇杆的哪个位置；
    {
        case RIGHT: {
            snake_x[0] += 1;
            break;
        }
        case LEFT: {
            snake_x[0] -= 1;
            break;
        }
        case UP: {
            snake_y[0] -= 1;
            break;
        }
        case DOWN: {
            snake_y[0] += 1;
            break;
        }
    }
    for (int i = 1; i < snake_length; i++) {
        snake_x[i] = temp_x[i - 1];
        snake_y[i] = temp_y[i - 1];  //所以，贪吃蛇身体横纵坐标依次为临时数组中存储的贪吃蛇身体的横纵坐标；
    }
}

/************************************************
吃到东西后移动函数
************************************************/
void change_pos() {
    int temp_x[snake_length + 2];
    int temp_y[snake_length + 2];               //因为贪吃蛇不仅仅有横着的状态，还有竖着的状态，所以，贪吃蛇这里的Y坐标也需要设置成贪吃蛇的总长度
    for (int i = 0; i < snake_length - 1; i++)  //将旧坐标数据存放到temp数组 最后一位数据无效
    {
        temp_x[i] = snake_x[i];  //将贪吃蛇的X坐标存入新定义的temp_x[]数组中；
        temp_y[i] = snake_y[i];  //将贪吃蛇的Y坐标存入新定义的temp_y[]数组中；
    }
    snake_x[0] = food_x;
    snake_y[0] = food_y;  //如果贪吃蛇吃到食物后，需要将食物的纵坐标保存在贪吃蛇的头部的坐标
    for (int i = 1; i < snake_length; i++) {
        snake_x[i] = temp_x[i - 1];
        snake_y[i] = temp_y[i - 1];
    }
}

/************************************************
判断是否吃到食物 0-没有吃到 1-吃到
************************************************/
int snake_eat_food(int dir) {
    int x_temp = snake_x[0];  //将此时贪吃蛇的头的横坐标保存给变量x_temp;
    int y_temp = snake_y[0];  //将此时贪吃蛇的头的纵坐标保存给变量y_temp;
    switch (dir)              //判断摇杆上一次触发时所在的状态；
    {
        case UP:
            y_temp -= 1;
            break;
        case DOWN:
            y_temp += 1;
            break;
        case LEFT:
            x_temp -= 1;
            break;
        case RIGHT:
            x_temp += 1;
            break;
    }
    if ((x_temp = food_x) && (y_temp = food_y)) {
        snake_length += 1;  //如果坐标相等，贪吃蛇的长度增加1；
        change_pos();       //进入吃到东西移动的函数；
        return 1;
    } else {
        return 0;  //否则，没有吃到食物；
    }
}

/************************************************
判断是否撞墙 0-没有撞到 1-撞到
************************************************/
int snake_knock_wall(int dir) {
    int x_temp = snake_x[0];
    int y_temp = snake_y[0];
    switch (dir) {
        case UP:
            y_temp -= 1;
            break;
        case DOWN:
            y_temp += 1;
            break;
        case LEFT:
            x_temp -= 1;
            break;
        case RIGHT:
            x_temp += 1;
            break;
    }
    if (x_temp < 0 || x_temp > 42 || y_temp < 0 || y_temp > 19) {
        return 1;
    } else {
        return 0;
    }
}

/************************************************
判断是否吃到自己 0-没有吃到 1-吃到
************************************************/
int snake_eat_body(int dir) {
    int x_temp = snake_x[0];
    int y_temp = snake_y[0];
    switch (dir) {
        case UP:
            y_temp -= 1;
            break;
        case DOWN:
            y_temp += 1;
            break;
        case LEFT:
            x_temp -= 1;
            break;
        case RIGHT:
            x_temp += 1;
            break;
    }
    for (int i = 1; i < snake_length; i++) {
        if ((snake_x[0] == snake_x[i]) && (snake_y[0] == snake_y[i])) {
            return 1;
        }
    }
    return 0;
}

/************************************************
主循环函数
************************************************/
void loop(void) {
    while (1) {
        snake();
    }
}