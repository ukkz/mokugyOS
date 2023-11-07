/*
 * Arduino_FreeRTOS < 10.4.4-2
*/

#define MODE_CHANT_KEYBOARD           1
#define MODE_CHANT_SERIAL             2
#define MODE_CHANT_MIDI               3
#define MODE_KEYBOARD                 4
#define MODE_MOUSE                    5
#define MODE_GAMING                   6


#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <MIDIUSB.h>
#include "Chanting.h"
#include "Config.h"


/* 共有変数類 */
Adafruit_NeoPixel np(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_MMA8451 mma = Adafruit_MMA8451();
Chanting chanting(CHANT_KEYBOARD_ENTER);
typedef struct { float x = 0.0; float y = 0.0; float z = 0.0; } vector3_t;
struct { vector3_t current; vector3_t previous; } acc;
struct { int current = 0; int previous = 0; int diff = 0; } mic;


/* RTOS関係の定義（タスクの実装は別ファイル） */
void TaskLED(void *pvParameters);
void TaskManager(void *pvParameters);
void TaskApplication(void *pvParameters);
TaskHandle_t ApplicationHandler = NULL;
QueueHandle_t LEDQueue = NULL;
QueueHandle_t ModeQueue = NULL;


/* メイン */
void setup() {
  Serial.begin(9600);
  Keyboard.begin();
  Mouse.begin();
  
  // キュー作成
  LEDQueue = xQueueCreate(1, sizeof(int));
  ModeQueue = xQueueCreate(1, sizeof(byte));
  
  // 3軸加速度センサ
  mma.begin();
  mma.setRange(MMA8451_RANGE_2_G);
  mma.read(); // 初期値取得
  
  // NeoPixel
  np.begin();
  np.setBrightness(255);
  np.clear();
  FuncWelcomeLED();
  
  /**
   * 各タスクのスタックサイズはできるだけ小さい値にまで調整済み（増やすとOSがしんどくなる）
   * 引数: タスクポインタ, タスク名, スタックサイズ, パラメータ, 優先度, ハンドラ
   */
  xTaskCreate(TaskLED, "", 78, NULL, 1, NULL);
  xTaskCreate(TaskManager, "", 50, NULL, 1, NULL);
  xTaskCreate(TaskApplication, "", 144, NULL, 1, &ApplicationHandler);
}


/* ループは加速度値とマイク値を継続的に取得し共有変数を更新するタスクとしての扱い（スタックサイズ192） */
void loop() {
  // 加速度（RCフィルタ）
  acc.previous = acc.current;
  mma.read();
  acc.current.x = (mma.x_g * STD_GRAVITY * RC_RATIO) + ((1.0 - RC_RATIO) * acc.previous.x);
  acc.current.y = (mma.y_g * STD_GRAVITY * RC_RATIO) + ((1.0 - RC_RATIO) * acc.previous.y);
  acc.current.z =  mma.z_g * STD_GRAVITY;
  delay(20);
  
  // マイク値（平均と差分）
  mic.previous = mic.current;
  unsigned long micbuf = 0;
  for (uint8_t i=0; i<MIC_LOOPS; i++) {
    int16_t rawLevel = analogRead(MIC_PIN); // 1回あたり約0.1ms
    rawLevel -= MIC_DC_LEVEL;
    micbuf += abs(rawLevel);
  }
  mic.current = micbuf / MIC_LOOPS;
  mic.diff = abs(mic.current - mic.previous);
  delay(1);
}
