/*
 * メインアプリケーションタスク
 * モードによって挙動が変わります
 */

#define SHOT_THRESHOLD_ON           3.0
#define SHOT_THRESHOLD_OFF          1.0
#define DIRECTION_THRESHOLD_ON      4.0
#define DIRECTION_THRESHOLD_OFF     2.0
#define MICLEVEL_DIFF_THRESHOLD     100


void TaskApplication(void *pvParameters) {
  struct { float x = 0.0; float y = 0.0; float z = 0.0; float current = 0.0; float previous = 0.0; } diff;
  struct { boolean current = false; boolean previous = false; } hit;
  struct { boolean left = false; boolean down = false; boolean up = false; boolean right = false; } direction;
  int moveX, moveY;
  midiEventPacket_t noteOn   = {0x09, 0x90 | MIDI_CHANNEL, MIDI_PITCH, 100};
  midiEventPacket_t noteOff  = {0x08, 0x80 | MIDI_CHANNEL, MIDI_PITCH, 0};
  unsigned long lastHitMillis = 0;
  boolean noteOffSent = true;
  byte mode = MODE_CHANT_KEYBOARD;
  
  for (;;) {
    // モード値を更新する
    xQueueReceive(ModeQueue, &mode, 0);
    
    // 加速度は前回との差分の絶対値
    diff.x = acc.current.x - acc.previous.x;
    diff.y = acc.current.y - acc.previous.y;
    diff.z = acc.current.z - acc.previous.z;
    diff.current = abs(sqrt((diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z)));

    // マウス移動量の計算（マウスモード・ゲーミングモードのみ）
    moveX = X_DIRECTION * (int)(acc.current.x * acc.current.x * 0.6);
    moveY = Y_DIRECTION * (int)(acc.current.y * acc.current.y * 0.6);
    
    // 叩かれた瞬間の状態
    // 今回と前回の差分値がいずれも±SHOT_THRESHOLD_ONを超えるとOK
    // かつマイク差分レベルがMICLEVEL_DIFF_THRESHOLDを超えること
    if (!hit.previous && mic.diff >= MICLEVEL_DIFF_THRESHOLD && diff.current >= SHOT_THRESHOLD_ON) {
      hit.current = true;
      lastHitMillis = millis();
      // LEDを点灯させる
      FuncChangeLED(0, 255); // モード（0はレインボー）・明るさ
      // モードごとの挙動
      if (mode == MODE_CHANT_KEYBOARD) {
        String phrase = chanting.get();
        Keyboard.print(phrase);
      }
      if (mode == MODE_CHANT_SERIAL) Serial.write(SERIAL_HIT_CODE);
      if (mode == MODE_CHANT_MIDI) {
        MidiUSB.sendMIDI(noteOff);
        MidiUSB.sendMIDI(noteOn);
        MidiUSB.flush();
        noteOffSent = false;
      }
      if (mode == MODE_KEYBOARD) Keyboard.press(KEY_RETURN);
      if (mode == MODE_MOUSE) Mouse.press(MOUSE_LEFT);
      if (mode == MODE_GAMING) Mouse.press(MOUSE_LEFT);
      vTaskDelay(4);
    }
    
    // 叩かれフラグを元の状態に戻す
    if (hit.previous && abs(diff.previous) < SHOT_THRESHOLD_OFF) {
      hit.current = false;
      if (mode == MODE_KEYBOARD) Keyboard.release(KEY_RETURN);
      if (mode == MODE_MOUSE) Mouse.release(MOUSE_LEFT);
      if (mode == MODE_GAMING) Mouse.release(MOUSE_LEFT);
    }
    
    // 左入力（開始）
    if (!direction.left && acc.current.x <= (DIRECTION_THRESHOLD_ON * X_DIRECTION)) {
      direction.left = true;
      if (mode == MODE_CHANT_SERIAL) Serial.write(SERIAL_LEFT_CODE);
      if (mode == MODE_KEYBOARD) Keyboard.press(KEYBOARD_LEFT);
    }
    // 左入力（終了）
    if (direction.left && acc.current.x > (DIRECTION_THRESHOLD_OFF * X_DIRECTION)) {
      direction.left = false;
      if (mode == MODE_KEYBOARD) Keyboard.release(KEYBOARD_LEFT);
    }
    
    // 右入力（開始）
    if (!direction.right && acc.current.x >= (DIRECTION_THRESHOLD_ON * X_DIRECTION * -1)) {
      direction.right = true;
      if (mode == MODE_CHANT_SERIAL) Serial.write(SERIAL_RIGHT_CODE);
      if (mode == MODE_KEYBOARD) Keyboard.press(KEYBOARD_RIGHT);
    }
    // 右入力（終了）
    if (direction.right && acc.current.x < (DIRECTION_THRESHOLD_OFF * X_DIRECTION * -1)) {
      direction.right = false;
      if (mode == MODE_KEYBOARD) Keyboard.release(KEYBOARD_RIGHT);
    }
    
    // 下入力（開始）
    if (!direction.down && acc.current.y <= (DIRECTION_THRESHOLD_ON * Y_DIRECTION)) {
      direction.down = true;
      if (mode == MODE_CHANT_KEYBOARD && DOWN_ENTER_SUPPORT) {
        Keyboard.press(KEY_RETURN);
        vTaskDelay(4);
        Keyboard.release(KEY_RETURN);
      }
      if (mode == MODE_CHANT_SERIAL) Serial.write(SERIAL_DOWN_CODE);
      if (mode == MODE_KEYBOARD) Keyboard.press(KEYBOARD_DOWN);
      if (mode == MODE_GAMING) Keyboard.press(GAMING_DOWN);
    }
    // 下入力（終了）
    if (direction.down && acc.current.y > (DIRECTION_THRESHOLD_OFF * Y_DIRECTION)) {
      direction.down = false;
      if (mode == MODE_KEYBOARD) Keyboard.release(KEYBOARD_DOWN);
      if (mode == MODE_GAMING) Keyboard.release(GAMING_DOWN);
    }
    
    // 上入力（開始）
    if (!direction.up && acc.current.y >= (DIRECTION_THRESHOLD_ON * Y_DIRECTION * -1)) {
      direction.up = true;
      if (mode == MODE_CHANT_KEYBOARD && UP_SPACE_SUPPORT) Keyboard.write(' ');
      if (mode == MODE_CHANT_SERIAL) Serial.write(SERIAL_UP_CODE);
      if (mode == MODE_KEYBOARD) Keyboard.press(KEYBOARD_UP);
      if (mode == MODE_GAMING) Keyboard.press(GAMING_UP);
    }
    // 上入力（終了）
    if (direction.up && acc.current.y < (DIRECTION_THRESHOLD_OFF * Y_DIRECTION * -1)) {
      direction.up = false;
      if (mode == MODE_KEYBOARD) Keyboard.release(KEYBOARD_UP);
      if (mode == MODE_GAMING) Keyboard.release(GAMING_UP);
    }

    // マウスモード時のカーソル移動
    if (mode == MODE_MOUSE) {
      moveX = (direction.left) ? moveX : (direction.right) ? -1 * moveX : 0;
      moveY = (direction.up)   ? moveY : (direction.down)  ? -1 * moveY : 0;
      if (direction.left || direction.right || direction.down || direction.up)  Mouse.move(moveX, moveY, 0);
    }

    // ゲーミングモード時の左右マウス
    if (mode == MODE_GAMING) {
      moveX = (direction.left) ? moveX : (direction.right) ? -1 * moveX : 0;
      if (direction.left || direction.right) Mouse.move(moveX, 0, 0);
    }

    // MIDI時に最後に叩かれてから一定時間経過後にノートオフを送る
    if (mode == MODE_CHANT_MIDI && !noteOffSent && millis() >= lastHitMillis + AUTO_NOTEOFF_MAX) {
      MidiUSB.sendMIDI(noteOff);
      MidiUSB.flush();
      noteOffSent = true;
    }
    
    // 次回ループ用
    diff.previous = diff.current;
    hit.previous = hit.current;
    vTaskDelay(1);
  }
}
