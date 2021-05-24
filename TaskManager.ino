/**
 * タスクマネージャ（モード切り替え）
 */

#define STANDING_THRESHOLD_ON         -8.5
#define STANDING_THRESHOLD_OFF        -4.0
#define SWITCH_THRESHOLD_ON           4.0
#define SWITCH_THRESHOLD_OFF          1.0

void TaskManager(void *pvParameters) {
  bool rootMenu = false;
  bool lock = false;
  int mode = MODE_CHANT_KEYBOARD;
  
  for (;;) {
    // 各種モードからルートメニューへ
    if (!rootMenu && acc.current.y <= STANDING_THRESHOLD_ON) {
      rootMenu = true;
      vTaskSuspend(ApplicationHandler);
      // お経リセットする
      chanting.reset();
      // MIDIリセット
      if (mode == MODE_CHANT_MIDI) {
        midiEventPacket_t midiMessage = {0x08, 0x80 | MIDI_CHANNEL, MIDI_PITCH, 0}; // ノートオフ
        MidiUSB.sendMIDI(midiMessage);
        MidiUSB.flush();
      }
      // キー押しっぱなしかもしれないので解除
      if (mode == MODE_KEYBOARD || mode == MODE_GAMING) Keyboard.releaseAll();
      // LEDを現在のモードカラーに
      FuncChangeLED(mode);
    }

    // ルートメニューから各種モードへ
    if (rootMenu && acc.current.y > STANDING_THRESHOLD_OFF) {
      rootMenu = false;
      vTaskResume(ApplicationHandler);
      // LED消灯
      FuncChangeLED(0, 0);
    }
    
    // ルートメニュー時は左右の加速度を監視してモード切替動作
    if (rootMenu) {
      if (!lock && abs(acc.current.x) >= SWITCH_THRESHOLD_ON) {
        lock = true;
        mode += (acc.current.x > 0) ? 1 : -1;
        if (mode > MODE_GAMING) mode = 1; // 上オーバー
        if (mode < 1) mode = MODE_GAMING; // 下オーバー
        // LEDに送る
        FuncChangeLED(mode);
        // モード値変更をアプリケーションに送る（常に最新の1つの値のみ保持されResume後に読まれる）
        xQueueOverwrite(ModeQueue, &mode);
      }
      if (lock && abs(acc.current.x) <= SWITCH_THRESHOLD_OFF) lock = false;
    }
    
    vTaskDelay(1);
  }
}
