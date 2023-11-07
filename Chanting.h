/**
 * 解説: https://zenn.dev/ukkz/articles/02c243a6ba7795
 * 
 * ＜注意＞
 * あらかじめ "Sutra Writer" でEEPROMに使用したいお経を書き込んでおくこと
 * 
 * ＜記事解説からの変更点＞
 * ・PROGMEMでも厳しくなったのでEEPROMに格納したバイト列から変換するようにしました。
 * ・Stringはメモリ消費が激しいので通常のchar列で文字列表現しています。
 */

#include <EEPROM.h>

class Chanting {
  private:
    int cursor = 0; // 読経位置インデックス
    bool finished = false; // 末端まで達したか
    char output[12] = ""; // 最大5文字 + 改行コード等最大3文字 + スペース予備3文字 + 終端 = 12
    char enterCode[4]; // CR,LF,CR+LF,space+CRLFなど（WinやMacなど環境にあわせてコンストラクタで設定のこと）
    // 以下 文字数ごとの音節配列
    char syl_1[4][2]  = { "a", "i", "u", "e" };
    char syl_2[26][3] = { "ka", "ku", "ke", "ko", "si", "ta", "ti", "ni", "ne", "ha", "fu", "ma", "mi", "mu", "ge", "go", "ze", "ji", "ju", "jo", "do", "bi", "bu", "bo", "ra", "ri" };
    char syl_3[24][4] = { "kai", "kuu", "kou", "sai", "sou", "tei", "tou", "nai", "nou", "hou", "gya", "sha", "shu", "sho", "zai", "zou", "jou", "dai", "dou", "nya", "nyo", "rou", "unn", "onn" };
    char syl_N[8][4]  = { "kan", "ken", "san", "sin", "ten", "han", "gen", "jin" };
    char syl_4[19][5] = { "iltu", "satu", "siki", "setu", "soku", "sowa", "toku", "noku", "hara", "metu", "yaku", "watu", "gyou", "shuu", "shou", "jitu", "chuu", "butu", "myou" };
    char syl_5[6][6]  = { "saltu", "toltu", "miltu", "zeltu", "bultu", "myaku" };
    
  public:
    Chanting(const char* enter) {
      strcpy(enterCode, enter);
    }

    char* get() {
      // 終了している場合は改行を返してからリセット
      if (finished || cursor >= EEPROM.length()) {
        reset();
        return enterCode;
      }
      
      // 現在のコードと1つ先のコードをEEPROMから取得
      byte code = EEPROM.read(cursor);
      byte next = EEPROM.read(cursor + 1);
      cursor++; // 進めておく
      
      // 上位3ビットで音節配列選択・下位5ビットでインデックス選択
      byte sylClass = code >> 5;
      byte sylIndex = code & 0x1F;
      switch (sylClass) {
        case 0:
          strcpy(output, syl_1[sylIndex]);
          break;
        case 1:
          strcpy(output, syl_2[sylIndex]);
          break;
        case 2:
          strcpy(output, syl_3[sylIndex]);
          break;
        case 3:
          strcpy(output, syl_N[sylIndex]);
          strcat(output, "n"); // ここだけnを付加する
          break;
        case 4:
          strcpy(output, syl_4[sylIndex]);
          break;
        case 5:
          strcpy(output, syl_5[sylIndex]);
          break;
        default:
          strcpy(output, ""); // 対応する音節配列がない
      }

      // 先読みしたコードが特殊操作（0xF0 ~ 0xFF）なとき
      if (next >= 0xF0) {
        if (next == 0xF0 || next == 0xFF) finished = true;
        if (next & 0x02) strcat(output, " "); // 0xF3はSPACE+ENTER
        if (next & 0x01) strcat(output, enterCode);
        cursor++; // もう1つ次に飛ばしておく（特殊コードは連続しない前提）
      }
      
      return output;
    }
    
    // インデックスを最初に戻す
    void reset() {
      cursor = 0;
      finished = false;
    }
};
