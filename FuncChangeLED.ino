/* TaskLEDのキューにデータ送信して点灯制御する */
void FuncChangeLED(byte mode = 0, byte brightness = 0xFF) {
  static int message = 0;
  if (brightness < 10) brightness = 1; // 消灯は制御の都合により1にしておく（0だと処理をスキップするので消えない）
  message = (mode<<8) + brightness;
  xQueueSend(LEDQueue, &message, 1);
}
