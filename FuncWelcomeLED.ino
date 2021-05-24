/* 起動時にグラデーション表示するだけのやつ */
void FuncWelcomeLED() {
  delay(1000);
  float hue = 0.0; // 0.0 ~ 1.0
  for (byte deg=0; deg<180; deg++) {
    float rad = (2 * PI / 360) * deg;
    // 虹色のところ: Hue,Saturation,Volume
    // 0.0625 = 4096/65536
    for (int i=0; i<np.PixelCount(); i++) np.SetPixelColor(i, HsbColor(hue + (i * 0.0625), 1.0, sin(rad)));
    np.Show();
    hue += 0.0078125; // = 512/65536
    if (hue >= 1.0) hue -= 1.0; // ループ
    delay(30);
  }
  np.ClearTo(RgbColor(0, 0, 0));
  np.Show();
  delay(1000);
}
