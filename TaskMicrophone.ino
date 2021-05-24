/*
 * 音声取得用タスク
 * （ハードウェア上は実装してるけどまだ上手く使えてません）
 * - SparkFun Electret Microphone Breakout (https://www.sparkfun.com/products/12758)
 * - Pro Micro A0端子に接続
 */

#define MIC_PIN                     A0
#define MIC_DC_OFFSET               30
#define MIC_RC_RATIO                0.25
#define MIC_LOOPS                   20

void TaskMicrophone(void *pvParameters) {
  int micVal = 0;
  unsigned int micSum = 0;
  unsigned int micPrv = 0;
  for (;;) {
    micSum = 0;
    for (byte i=0; i<MIC_LOOPS; i++) {
      micVal = analogRead(MIC_PIN) - 512 - MIC_DC_OFFSET;
      micSum += abs(micVal);
    }
    micSum /= MIC_LOOPS;
    micSum = (MIC_RC_RATIO * micSum) + ((1 - MIC_RC_RATIO) * micPrv);
    micPrv = micSum;
    Serial.println(micSum);
    vTaskDelay(10);
  }
}
