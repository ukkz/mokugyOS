/* 起動時にグラデーション表示するだけのやつ */
void FuncWelcomeLED() {
  delay(1000);
  uint16_t hue = 0; // 0 ~ 65535
  for (uint8_t deg=0; deg<180; deg++) {
    float rad = (2 * PI / 360) * deg;
    uint8_t v = (uint8_t)(255.0 * sin(rad));
    // 虹色のところ: Hue,Saturation,Volume
    // 0.0625 = 4096/65536
    for (uint16_t i=0; i<LED_NUM; i++) np.setPixelColor(i, np.gamma32(np.ColorHSV(hue + (i * 4096), 255, v)));
    np.show();
    hue += 512; // = 512/65536
    delay(30);
  }
  np.clear();
  delay(1000);
}
