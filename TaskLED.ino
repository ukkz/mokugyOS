/*
 * フルカラーLEDの制御
 * - NeoPixel WS2812Bまたはその互換品 (http://ssci.to/1399)
 * - LED_NUMはNeoPixelの総数（標準：5）
 * - LED_PINはNeoPixelの信号線を接続するPro Microのピン番号
 */

void TaskLED(void *pvParameters) {
  float hue = 0.0;
  byte brightness = 0;
  int message = 0;
  byte currentMode = 0;
  byte previousMode = 0;
  RgbColor blue(0, 0, 255);
  RgbColor yellow(230, 255, 0);
  RgbColor red(230, 0, 0);
  RgbColor white(255, 255, 255);
  RgbColor orange(230, 130, 0);
  RgbColor black(0, 0, 0);
  
  for (;;) {
    if (currentMode == 0) {
      // 叩かれて減衰するパターン
      previousMode = 0;
      if (brightness > 10) {
        // ある程度明るさがあったらつける
        for (int i=0; i<np.PixelCount(); i++) {
          np.SetPixelColor(i, HsbColor(hue + (i * 0.1), 1.0, (float)brightness / 255.0));
        }
        brightness -= 10; // 直線減衰
        hue += 0.006;
        if (hue >= 1.0) hue -= 1.0; // ループ
        np.Show();
      } else if (brightness > 0) {
        // けす（これ以降はキューなどからbrightnessが11以上にされないと点灯しない）
        brightness = 0;
        np.ClearTo(black);
        np.Show();
      }
    } else if (currentMode != previousMode) {
      // モード切り替え時の単色パターン（国際仏旗色順）
      previousMode = currentMode;
      switch (currentMode) {
        case MODE_CHANT_KEYBOARD:
          np.ClearTo(blue); // 青
          break;
        case MODE_CHANT_SERIAL:
          np.ClearTo(yellow); // 黄
          break;
        case MODE_CHANT_MIDI:
          np.ClearTo(red); // 赤
          break;
        case MODE_KEYBOARD:
          np.ClearTo(white); // 白
          break;
        case MODE_MOUSE:
          np.ClearTo(orange); // 樺
          break;
        case MODE_GAMING:
          np.SetPixelColor(0, blue);
          np.SetPixelColor(1, yellow);
          np.SetPixelColor(2, red);
          np.SetPixelColor(3, white);
          np.SetPixelColor(4, orange);
          break;
        default:
          np.ClearTo(black);
      }
      np.Show();
    }
    
    // 点灯パターンを受け取る（vTaskDelay(1) の代わりにもなる）
    if (xQueueReceive(LEDQueue, &message, 1) == pdPASS) {
      currentMode = message >> 8;     // モード番号
      brightness = 0x00FF & message;  // 明るさ（モードの場合は関係ない）
    }
  }
}
