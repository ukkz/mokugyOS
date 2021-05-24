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
#include <NeoPixelBus.h>
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <MIDIUSB.h>
#include "Chanting.h"
#include "Config.h"


/* 共有変数類 */
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> np(LED_NUM, LED_PIN);
Adafruit_MMA8451 mma = Adafruit_MMA8451();
Chanting chanting(CHANT_KEYBOARD_ENTER);
typedef struct { float x = 0.0; float y = 0.0; float z = 0.0; } vector3_t;
struct { vector3_t current; vector3_t previous; } acc;


/* RTOS関係の定義（タスクの実装は別ファイル） */
void TaskLED(void *pvParameters);
void TaskManager(void *pvParameters);
//void TaskMicrophone(void *pvParameters);
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
  np.Begin();
  np.Show();
  FuncWelcomeLED();
  
  /**
   * 各タスクのスタックサイズはできるだけ小さい値にまで調整済み（増やすとOSがしんどくなる）
   * 引数: タスクポインタ, タスク名, スタックサイズ, パラメータ, 優先度, ハンドラ
   */
  xTaskCreate(TaskLED, "", 78, NULL, 1, NULL);
  xTaskCreate(TaskManager, "", 50, NULL, 1, NULL);
  xTaskCreate(TaskApplication, "", 144, NULL, 1, &ApplicationHandler);
}


/* ループは加速度値を継続的に取得し共有変数を更新するタスクとしての扱い（スタックサイズ192） */
void loop() {
  mma.read();
  acc.current.x = (mma.x_g * STD_GRAVITY * RC_RATIO) + ((1.0 - RC_RATIO) * acc.previous.x);
  acc.current.y = (mma.y_g * STD_GRAVITY * RC_RATIO) + ((1.0 - RC_RATIO) * acc.previous.y);
  acc.current.z =  mma.z_g * STD_GRAVITY;
  delay(10);
  acc.previous = acc.current;
}
