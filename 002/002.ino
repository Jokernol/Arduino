/*
    簡易オシロ (_20190131_OLEDoscilloscope.ino)
    AC電源波形観察用の設定になっている。
    2019/01/31版 ラジオペンチ
*/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>


#define SCREEN_WIDTH 128      // OLED display width
#define SCREEN_HEIGHT 64      // OLED display height
#define SCREEN_ADDRESS 0x3D   // OLED display address

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int dataBuff[200]; // データーバッファ RAMがギリギリ
char chrBuff[5];   // 表示フォーマットバッファ
String hScale = "xxxAs";

int dataMin;
int dataMax;
int dataAve;
int rangeMax;
int rangeMin;
int trigP; // データバッファ上のトリガ位置
boolean trigSync;

void setup() {
  pinMode(2, INPUT_PULLUP); // Range0 0:10ms, 1:0.2ms
  pinMode(3, INPUT_PULLUP); // Range1 リザーブ
  pinMode(4, INPUT_PULLUP); //
  pinMode(5, INPUT_PULLUP); // トリガ極性（HIGH:Rising、LOW:Faling
  pinMode(13, OUTPUT);      // 状態表示
  //  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { // Address 0x3C for 128x64
    //       Serial.println(F("SSD1306 failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  startScreen(); // 共通部分を描画
}

void loop() {
  digitalWrite(13, HIGH);
  readWave();            // 波形読み取り (最小1.6ms )
  digitalWrite(13, LOW); //
  dataAnalize();         // データーの各種情報を収集(0.4-0.7ms)
  writeCommonImage();    // 固定イメージの描画(4.6ms)
  plotData();            // グラフプロット(5.4ms)
  dispInf();             // 各種情報表示(6.2ms)
  display.display();     // バッファを転送して表示(37ms)
}

void writeCommonImage() {
  display.clearDisplay();                   // 画面全消去(0.4ms)
  display.setTextColor(WHITE);              // Draw white text
  display.setCursor(0, 0);                  // Start at top-left corner
  display.println("             av:    V"); // 1行目固定文字表示(3.2ms)
  display.drawFastVLine(26, 9, 55, WHITE);  // 左縦線
  display.drawFastVLine(127, 9, 55, WHITE); // 左縦線

  display.drawFastHLine(24, 9, 7, WHITE);  // 左端Max値の補助マーク
  display.drawFastHLine(24, 36, 2, WHITE); // 　center
  display.drawFastHLine(24, 63, 7, WHITE); // 　Min

  display.drawFastHLine(51, 9, 3, WHITE);  // Max値の補助マーク
  display.drawFastHLine(51, 63, 3, WHITE); // Min

  display.drawFastHLine(76, 9, 3, WHITE);  // Max値の補助マーク
  display.drawFastHLine(76, 63, 3, WHITE); // Min

  display.drawFastHLine(101, 9, 3, WHITE);  // Max値の補助マーク
  display.drawFastHLine(101, 63, 3, WHITE); // Min

  display.drawFastHLine(123, 9, 5, WHITE);  // 右端Max値の補助マーク
  display.drawFastHLine(123, 63, 5, WHITE); // 　Min

  for (int x = 26; x <= 128; x += 5) {
    display.drawFastHLine(x, 36, 2, WHITE); // 中心線(水平線)を点線で描く
  }
  for (int x = (127 - 25); x > 30; x -= 25) {
    for (int y = 10; y < 63; y += 5) {
      display.drawFastVLine(x, y, 2, WHITE); // 縦線を点線で3本描く
    }
  }
}

void readWave() { // 波形をメモリーに記録
  int mode;
  mode = digitalRead(2) + 2 * digitalRead(3);
  switch (mode) {
  case 0: {
    waveRead_0();
    break;
  }
  case 1: {
    waveRead_1();
    break;
  }
  case 2: {
    waveRead_2();
    break;
  }
  case 3: {
    waveRead_3();
    break;
  }
  }
}

void waveRead_0() { // 波形読み取り (サンプリング周期＝400us)
  hScale = " 10ms";
  ADCSRA = ADCSRA & 0xf8; // 下3ビットをクリア
  ADCSRA = ADCSRA | 0x07; // 分周比設定(arduinoのオリジナル）
  for (int i = 0; i <= 199; i++) { // 200データー
    dataBuff[i] = analogRead(0);   // 約115μs
    delayMicroseconds(285);        // サンプリング周期調整
  }
}

void waveRead_1() { // 波形読み取り (サンプリング周期＝200us)
  hScale = "  5ms";
  ADCSRA = ADCSRA & 0xf8;          // 下3ビットをクリア
  ADCSRA = ADCSRA | 0x07;          // 分周比128(arduinoのオリジナル）
  for (int i = 0; i <= 199; i++) { // 200データー
    dataBuff[i] = analogRead(0); // 約115μs（レジスタ操作でもっと高速化が可能）
    delayMicroseconds(85); // サンプリング周期調整
  }
}

void waveRead_2() { // 波形読み取り (サンプリング周期＝40us)
  hScale = "  1ms";
  ADCSRA = ADCSRA & 0xf8; // 下3ビットをクリア
  ADCSRA =
      ADCSRA |
      0x04; // 分周比16(0x1=2, 0x2=4, 0x3=8, 0x4=16, 0x5=32, 0x6=64, 0x7=128)
  for (int i = 0; i <= 199; i++) { // 200データー
    dataBuff[i] = analogRead(0);   // 約16μs
    delayMicroseconds(24);         // サンプリング周期調整
  }
}

void waveRead_3() { // 波形読み取り (サンプリング周期＝8us)
  hScale = "200us";
  ADCSRA = ADCSRA & 0xf8; // 下3ビットをクリア
  ADCSRA =
      ADCSRA |
      0x02; // 分周比:4(0x1=2, 0x2=4, 0x3=8, 0x4=16, 0x5=32, 0x6=64, 0x7=128)
  for (int i = 0; i <= 199; i++) {
    dataBuff[i] = analogRead(0); // 約6μs
    // 時間調整　1.875μs（nop 1つで1クロック、0.0625μs @16MHz)
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
  }
}

void dataAnalize() { // 作図のために最大最小値を求める
  int d;
  long sum = 0;

  // 最大最小値を求める
  dataMin = 1023;                  // 最小
  dataMax = 0;                     // 最大値記録変数を初期化
  for (int i = 0; i <= 199; i++) { // 最大と最小値を求める
    d = dataBuff[i];
    sum = sum + d;
    if (d < dataMin) { // 最小と
      dataMin = d;
    }
    if (d > dataMax) { // 最大値を記録
      dataMax = d;
    }
  }
  // 平均値を求める
  dataAve = sum / 200; // 平均値計算

  // グラフ表示の縦軸レンジを計算
  rangeMin = dataMin - 20;         // 表示レンジ下限を-20下に設定
  rangeMin = (rangeMin / 10) * 10; // 10ステップに丸め
  if (rangeMin < 0) {
    rangeMin = 0; // 但し下限は0
  }
  rangeMax = dataMax + 20; // 表示レンジ上限を+20上に設定
  rangeMax = ((rangeMax / 10) + 1) * 10; // 切り上げで10ステップに丸め
  if (rangeMax > 1020) {
    rangeMax = 1023; // 但し1020以上なら1023で抑える
  }
  // トリガ位置を探す
  for (trigP = 49; trigP < 150;
       trigP++) { // データー範囲の中央で、中央値を跨いでいるポイントを探す
    if (digitalRead(5) == HIGH) { // トリガ極性スイッチがHIGHなら
      if ((dataBuff[trigP - 1] < (dataMax + dataMin) / 2) &&
          (dataBuff[trigP] >= (dataMax + dataMin) / 2)) {
        break; // 立ち上がりトリガ検出！
      }
    } else { // HIGHでなかったら（つまりLOW）
      if ((dataBuff[trigP - 1] > (dataMax + dataMin) / 2) &&
          (dataBuff[trigP] <= (dataMax + dataMin) / 2)) {
        break;
      } // 立下りトリガ検出！
    }
  }
  trigSync = true;
  if (trigP == 150) { // トリガが見つからなかったら中央にしておく
    trigP = 100;
    trigSync = false; // Unsync表示用フラグ
  }
}

void startScreen() { // 共通図形の作画
  display.clearDisplay();
  display.setTextSize(2);       // 文字を2倍角で、
  display.setTextColor(WHITE);  //
  display.setCursor(0, 20);     //
  display.println("OSO start"); // 開始画面表示
  display.println(" v0.2");     // グラフタイトルの表示
  display.display();            // 表示
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1); // 以下は標準文字サイズ
}

void dispInf() { // 文字情報の表示
  float voltage;

  display.setCursor(24, 0); // 波形の左上に
  display.print(hScale);    // 横軸スケール(time/div)表示

  display.setCursor(66, 0); // 波形の中央上に
  if (digitalRead(5) == HIGH) {
    display.print(char(0x18)); // トリガ極性表示↑
  } else {
    display.print(char(0x19)); //               ↓
  }

  voltage = dataAve * 5.0 / 1023.0; // 平均値を電圧に換算
  dtostrf(voltage, 4, 2, chrBuff);  // x.xx 形式に変換
  display.setCursor(96, 0);         // 波形の右上に
  display.print(chrBuff);           // 電圧の平均値を表示

  if (trigSync == false) {     // トリガの検出に失敗していたら
    display.setCursor(60, 55); // 画面の中央下に
    display.print("Unsync");   // Unsync を表示
  }

  voltage = rangeMax * 5.0 / 1023.0; // 電圧に換算
  dtostrf(voltage, 4, 2, chrBuff);   // *.** 形式に変換
  display.setCursor(0, 9);
  display.print(chrBuff); // Max値表示

  voltage = ((rangeMax + rangeMin) / 2) * 5.0 / 1023.0;
  dtostrf(voltage, 4, 2, chrBuff);
  display.setCursor(0, 33);
  display.print(chrBuff); // 中心値表示

  voltage = rangeMin * 5.0 / 1023.0;
  dtostrf(voltage, 4, 2, chrBuff);
  display.setCursor(0, 57);
  display.print(chrBuff); // Min値表示
}

void plotData() { // 配列の値に基づきデーターをプロット
  long y;
  for (int x = 0; x <= 99; x++) {
    y = map(dataBuff[x + trigP - 50], rangeMin, rangeMax, 63,
            9);                          // プロット座標へ変換
    display.drawPixel(x + 27, y, WHITE); // データをプロット
  }
}
