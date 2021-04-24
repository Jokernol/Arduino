// RAMの使用状況チェックプログラム
// グローバル変数、ヒープ、空き、スタック領域のアドレスとサイズをシリアルに出力
// 2019/5/13 ラジオペンチ
// (コンパイル結果：スケッチが4266バイトを使っています。グローバル変数が222バイト、)

// メモリチェック用変数（ここから）
int aRamStart = 0x0100;  // RAM先頭アドレス（固定値）
int aGvalEnd;            // グローバル変数領域末尾アドレス
int aHeapEnd;  // ヒープ領域末尾アドレス(次のヒープ用アドレス）
int aSp;  // スタックポインタアドレス（次のスタック用アドレス）
char aBuff[6];  // 表示フォーマット操作バッファ

void setup() {
    Serial.begin(115200);
    checkMem();  // メモリーの使用状況をチェック
    printMem();  // チェック結果をシリアルに表示
}

void loop() {}

//-------メモリ使用状況表示-------------------------------------------------
// ここから↓
void printMem() {  // RAM使用状況を表示
    Serial.println();
    Serial.println(F("RAM allocation table"));
    Serial.println(F("usage   start          end           size"));

    Serial.print(F("groval: "));  // 固定アドレス
    printHexDecimal(aRamStart);   // 開始
    Serial.print(F(" - "));
    printHexDecimal(aGvalEnd);  // 終了
    Serial.print("  ");
    printHexDecimal(aGvalEnd - aRamStart + 1);  // サイズ
    Serial.println();

    Serial.print(F("heap  : "));    // ヒープ
    printHexDecimal(aGvalEnd + 1);  // 開始
    Serial.print(F(" - "));
    printHexDecimal(aHeapEnd -
                    1);  // 終了(aHeapEndは次のヒープ用のアドレスなので-1）
    Serial.print(F("  "));
    printHexDecimal(aHeapEnd - 1 - (aGvalEnd + 1) + 1);  // サイズ
    Serial.println();

    Serial.print(F("free  : "));  // Free領域
    printHexDecimal(aHeapEnd);    // 開始
    Serial.print(F(" - "));
    printHexDecimal(aSp);  // 終了
    Serial.print(F("  "));
    printHexDecimal(aSp - aHeapEnd + 1);  // サイズ
    Serial.println();

    Serial.print(F("stack : "));  // スタック領域
    printHexDecimal(aSp + 1);  // 開始（aSpは次のスタック用アドレスなので+1)
    Serial.print(F(" - "));
    printHexDecimal(RAMEND);  // 終了（RAMENDは0x8fffでシステム側で定義）
    Serial.print(F("  "));
    printHexDecimal(RAMEND - (aSp + 1) + 1);  // サイズ
    Serial.println();
    Serial.println();
}

void checkMem() {  // RAM使用状況を記録
    // 意味不明なところもあるが、そのまま使用
    uint8_t *heapptr, *stackptr;
    stackptr = (uint8_t *)malloc(4);  // とりあえず4バイト確保
    heapptr = stackptr;               // save value of heap pointer
    free(stackptr);                   // 確保したメモリを返却
    stackptr =
        (uint8_t
             *)(SP);  // SPの値を保存（SPには次のスタック用の値が入っている）
    aSp = (int)stackptr;      // スタックポインタの値を記録
    aHeapEnd = (int)heapptr;  // ヒープポインタの値を記録
    aGvalEnd =
        (int)__malloc_heap_start - 1;  // グローバル変数領域の末尾アドレスを記録
}

void printHexDecimal(int x) {   // 引数を16進と10進で表示 0xHHHH(dddd)
    sprintf(aBuff, "%04X", x);  // 16進4桁に変換
    Serial.print(F("0x"));
    Serial.print(aBuff);
    Serial.print(F("("));
    sprintf(aBuff, "%4d", x);  // 10進4桁に変換
    Serial.print(aBuff);
    Serial.print(F(")"));
}
//　ここまで↑　コピペ