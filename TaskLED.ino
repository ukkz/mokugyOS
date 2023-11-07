/*
 * フルカラーLEDの制御
 * - NeoPixel WS2812Bまたはその互換品 (http://ssci.to/1399)
 * - LED_NUMはNeoPixelの総数（標準：5）
 * - LED_PINはNeoPixelの信号線を接続するPro Microのピン番号
 */

void TaskLED(void *pvParameters) {
  int message = 0;
  uint16_t hue = 0;
  uint8_t brightness = 0;
  uint8_t currentMode = 0;
  uint8_t previousMode = 0;
  uint32_t blue   = np.gamma32(np.ColorHSV(43690)); // 65536*2/3
  uint32_t yellow = np.gamma32(np.ColorHSV(10922)); // 65536/6
  uint32_t red    = np.gamma32(np.ColorHSV(0));
  uint32_t white  = np.Color(255, 255, 255);
  uint32_t orange = np.gamma32(np.ColorHSV(5461));  // 65536/12
  
  for (;;) {
    if (currentMode == 0) {
      // 叩かれて減衰するパターン
      previousMode = 0;
      if (brightness > 10) {
        // ある程度明るさがあったらつける
        for (uint16_t i=0; i<LED_NUM; i++) np.setPixelColor(i, np.gamma32(np.ColorHSV(hue + (i * 2048), 255, brightness)));
        brightness -= 7; // 直線減衰
        hue += 128;
        np.show();
      } else if (brightness > 0) {
        // けす（これ以降はキューなどからbrightnessが11以上にされないと点灯しない）
        brightness = 0;
        np.clear();
        np.show();
      }
    } else if (currentMode != previousMode) {
      // モード切り替え時の単色パターン（国際仏旗色順）
      previousMode = currentMode;
      switch (currentMode) {
        case MODE_CHANT_KEYBOARD:
          np.fill(blue); // 青
          break;
        case MODE_CHANT_SERIAL:
          np.fill(yellow); // 黄
          break;
        case MODE_CHANT_MIDI:
          np.fill(red); // 赤
          break;
        case MODE_KEYBOARD:
          np.fill(white); // 白
          break;
        case MODE_MOUSE:
          np.fill(orange); // 樺
          break;
        case MODE_GAMING:
          np.setPixelColor(0, blue);
          np.setPixelColor(1, yellow);
          np.setPixelColor(2, red);
          np.setPixelColor(3, white);
          np.setPixelColor(4, orange);
          break;
        default:
          np.clear();
      }
      np.show();
    }
    
    // 点灯パターンを受け取る（vTaskDelay(1) の代わりにもなる）
    if (xQueueReceive(LEDQueue, &message, 1) == pdPASS) {
      currentMode = message >> 8;     // モード番号
      brightness = 0x00FF & message;  // 明るさ（モードの場合は関係ない）
    }
  }
}
