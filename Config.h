/* センサ接続に関する設定 SENSOR CONNECTIONS SETTING */
#define SDA                   4  // 3軸加速度モジュール I2C SDA - Pro Micro D2
#define SCL                   5  // 3軸加速度モジュール I2C SCL - Pro Micro D3
#define LED_NUM               5  // フルカラーLEDの個数（標準:5）
#define LED_PIN               10 // フルカラーLED信号線 - Pro Micro D10


/* 読経モード CHANTING KEYBOARD MODE */
#define CHANT_KEYBOARD_ENTER  "\n"  // 読経時の改行扱い（Win/Mac/Linuxなど環境に応じて設定してください）
#define UP_SPACE_SUPPORT      false // 上入力でスペースキー入力して漢字変換をサポートするか
#define DOWN_ENTER_SUPPORT    false // 下入力でエンター入力して変換確定をサポートするか


/* シリアルモード SERIAL OUTPUT MODE */
#define SERIAL_HIT_CODE       '.' // 叩かれた時に送信するコード
#define SERIAL_UP_CODE        '^' // 上入力時に送信するコード
#define SERIAL_LEFT_CODE      '<' // 左入力時に送信するコード
#define SERIAL_DOWN_CODE      '_' // 下入力時に送信するコード
#define SERIAL_RIGHT_CODE     '>' // 右入力時に送信するコード


/* MIDIモード MIDI DRUM MODE */
#define MIDI_CHANNEL          0    // MIDI送信するチャンネル
#define MIDI_PITCH            60   // デフォルトの音階（標準ド: 60）
#define AUTO_NOTEOFF_MAX      2000 // 次に叩かれなかったときに自動でノートオフするまでのミリ秒


/* キーボードモード GENERIC DIRECTION KEYBOARD MODE */
// 上下左右矢印
#define KEYBOARD_UP           (KEY_UP_ARROW)
#define KEYBOARD_LEFT         (KEY_LEFT_ARROW)
#define KEYBOARD_DOWN         (KEY_DOWN_ARROW)
#define KEYBOARD_RIGHT        (KEY_RIGHT_ARROW)
// WASDキーも使える
//#define KEYBOARD_UP           'w'
//#define KEYBOARD_LEFT         'a'
//#define KEYBOARD_DOWN         's'
//#define KEYBOARD_RIGHT        'd'


/* ゲーミングモード GAMING MODE */
#define GAMING_UP             'w'
#define GAMING_DOWN           's'


/* キーボード・マウス・ゲーミング各モード 共通 COMMON SETTINGS FOR KEYBOARD & MOUSE & GAMING */
#define X_DIRECTION           -1 // 1で左右正しい方向
#define Y_DIRECTION           -1 // 1で上下正しい方向


/* 開発用(変更の必要ありません) FOR DEVELOPMENT */
#define RC_RATIO              0.2
#define STD_GRAVITY           9.80665f
