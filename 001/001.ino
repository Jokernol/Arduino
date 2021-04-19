/* データートレンドモニタ(20190127_OLEDgraphTest.ino)
    0.96インチOLEDにアナログ入力の変化グラフを書く
    2019/01/27 ラジオペンチ http://radiopench.blog96.fc2.com/
*/

#include <Wire.h>
#include <Adafruit_GFX.h>      // adafruitのライブラリを使用
#include <Adafruit_SSD1306.h>
#include <MsTimer2.h>         // 定周期割込みに使用

#define SCREEN_WIDTH 128      // OLED display width, in pixels
#define SCREEN_HEIGHT 64      // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3D   // OLED display address

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4          // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int latestData;
int dataBuff[110];                    // データーバッファ
char chrBuff[20];                     // 表示フォーマットバッファ
int dataMin;
int dataMax;
volatile boolean timeFlag = LOW;

void setup() {
//  pinMode(13, OUTPUT);
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);                         // Don't proceed, loop forever
  }

  for (int i = 0; i <= 99; i++) {
    dataBuff[i] = -1;                 // バッファを未定義フラグ(-1)で埋める
  }
  drawCommons();                      // 共通部分を描画
  MsTimer2::set(60, timeUp);         // 60ms秒毎にタイマー割込み
  MsTimer2::start();
}

void loop() {
  while (timeFlag == LOW) {           // MsTimer2割込み待ち
  }
  timeFlag = LOW;
  //  digitalWrite(13, HIGH);
  latestData = analogRead(0);         // アナログポート0のデーターを読む(115us)
  saveBuff();                         // 表示バッファに書き込み(190us)
  dispNewData();                      // 最新値を表示(1.42ms)
  plotData();                         // グラフプロット(7.4ms)
  scaleLine();                        // 目盛り線表示(1.9ms)
  dispVscale();                       // 縦軸目盛り表示(3.9ms)
  display.display();                  // バッファの値を転送して表示(37ms)
  //  digitalWrite(13, LOW);          // 処理時間合計=52ms
}

void drawCommons() {                       // 共通図形の作画
  display.clearDisplay();
  display.setTextSize(1);                  // Normal 1:1 pixel scale
  display.setTextColor(WHITE);             // Draw white text
  display.setCursor(0, 0);                 // Start at top-left corner
  display.println("Trend monitor");
  display.drawRect(26, 9, 102, 55, WHITE); // グラフ領域枠の作画
  display.drawFastHLine(24, 9, 2, WHITE);  // Max値の補助マーク
  display.drawFastHLine(24, 36, 2, WHITE); // center
  display.drawFastHLine(24, 63, 2, WHITE); // Min
  display.display();                       // VRAM転送
}

void dispVscale() {                        // 縦軸目盛り表示
  display.fillRect(0, 9, 24, 55, BLACK);   // 前の表示をまとめて消す（24x55ドット）(460us)
  display.setCursor(0, 9);
  sprintf(chrBuff, "%4d", dataMax); display.print(chrBuff);     // Max値表示

  display.setCursor(0, 33);
  sprintf(chrBuff, "%4d", (dataMax + dataMin) / 2); display.print(chrBuff);   // 中心値表示

  display.setCursor(0, 57);
  sprintf(chrBuff, "%4d", dataMin); display.print(chrBuff);      // Min値表示
}

void saveBuff() {                       // データバッファの更新と最大・最小値の決定
  int d;
  dataMin = 1023;                       // 最小
  dataMax = 0;                          // 最大値記録変数を初期化

  for (int i = 98; i >= 0; i--) {       // 配列に値を保存しながら最大と最小値を求める
    d = dataBuff[i];
    dataBuff[i + 1] = d;                // 配列のデーターを一つ後ろにずらし
    if (d != -1) {                      // ずらしたデータが有効値だったら、
      if (d < dataMin) {                // 最小と
        dataMin = d;
      }
      if (d > dataMax) {                // 最大値を記録
        dataMax = d;
      }
    }
  }
  dataBuff[0] = latestData;             // 配列の先頭には最新データーを記録し、
  if (latestData < dataMin) {           // 最小と
    dataMin = latestData;
  }
  if (latestData > dataMax) {           // 最大値を再確認
    dataMax = latestData;
  }
  dataMin = dataMin - 20;               // 最小値を-20下に設定
  dataMin = (dataMin / 10) * 10;        // 10ステップに丸め
  if (dataMin < 0) {
    dataMin = 0;                        // 但し下限は0
  }
  dataMax = dataMax + 20;               // 最大値を+20上に設定
  dataMax = ((dataMax / 10) + 1) * 10;  // 切り上げで10ステップに丸め
  if (dataMax > 1020) {
    dataMax = 1023;                     // 但し1020以上なら1023で抑える
  }
  //  Serial.print(dataMin); Serial.print(", "); Serial.println(dataMax);
}

void dispNewData() {                         // 最新データーの値を画面の右上に表示
  display.fillRect(104, 0, 24, 8, BLACK);    // 前の表示値を消す(24x8ドット)(320us)
  display.setCursor(104, 0);
  sprintf(chrBuff, "%4d", latestData);       // 4桁右詰めで
  display.print(chrBuff);                    // 最新測定値を表示
}

void plotData() {                            // 配列の値に基づきデーターをプロット
  long yPoint;
  display.fillRect(27, 10, 100, 53, BLACK);  // グラフ表示領域をクリア(100x53ドット)(1.9ms)
  for (int i = 0; i <= 98; i++) {
    if (dataBuff[i] == -1) {                 // データーが未定(-1)なら
      break;                                 // プロット中止
    }
    yPoint = map(dataBuff[i], dataMin, dataMax, 63, 9); // プロット座標へ変換
    display.drawPixel(125 - i, yPoint, WHITE);     // データをプロット
  }
}

void scaleLine() {                             // 目盛り線を作画
  for (int x = 26; x <= 128; x += 4) {
    display.drawFastHLine(x, 36, 2, WHITE);    // 中心線を点線で描く
  }
  for (int x = (127 - 33); x > 30; x -= 33) {
    for (int y = 9; y < 63; y += 4) {
      display.drawFastVLine(x, y, 2, WHITE);   // 縦線を点線で2本描く
    }
  }
  display.setCursor(96, 55);
  display.print("< 2s>");                      // 横軸スケール表示
}

void timeUp() {                                // MsTimer2割込み処理
  timeFlag = HIGH;
}
