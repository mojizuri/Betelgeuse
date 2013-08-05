/* ------------------------------------
 * ●Betelgeuseプログラム
 * 作成日時：2013.07.28  Y.Yamashiro
 * 更新日時：2013.08.05  Y.Yamashiro
 *
 * ●更新履歴
 * Ver.0.1 2013.07.28 Y.Yamashiro
 * Ver.0.9 2013.08.05 Y.Yamashiro
 * ------------------------------------ */

#include <EEPROM.h>

/* -------------------- */
// 味付け
/* -------------------- */
unsigned char PowerOnDelay = 1; // 電源ON長押し時間1秒
unsigned char PowerOffDelay = 2; // 電源OFF長押し時間2秒
unsigned char PowerOffKeyConfig = B00010110;  // 電源OFFキー組み合わせ
unsigned char interlock = 1;  // インタロック機能有効？
unsigned char preview = 1;  // プレビュー機能有効？
unsigned char restore_lastcolor = 1; // 最後に表示した色を復元する？

unsigned char MAIN_R_1 = 150;
unsigned char MAIN_G_1 = 0;
unsigned char MAIN_B_1 = 0;

unsigned char MAIN_R_2 = 0;
unsigned char MAIN_G_2 = 150;
unsigned char MAIN_B_2 = 0;

unsigned char MAIN_R_3 = 0;
unsigned char MAIN_G_3 = 0;
unsigned char MAIN_B_3 = 150;

unsigned char MAIN_R_4 = 150;
unsigned char MAIN_G_4 = 150;
unsigned char MAIN_B_4 = 150;


/* -------------------- */
// グローバル変数
/* -------------------- */
unsigned char stable_USBP = 0;
unsigned char stable_SW1 = 0;
unsigned char stable_SW2 = 0;
unsigned char stable_SW3 = 0;
unsigned char stable_SW4 = 0;
unsigned char stable_SW5 = 0;
unsigned char stable_SWS = 0;

/* -------------------- */
// ポート名付け
/* -------------------- */
const unsigned char in_USBP = 2;  // USB電源供給を検出
const unsigned char in_SW1 = 18;  // SW1接続
const unsigned char in_SW2 = 16;  // SW2接続
const unsigned char in_SW3 = 13;  // SW3接続
const unsigned char in_SW4 = 8;  // SW4接続
const unsigned char in_SW5 = 15;  // SW5接続
const unsigned char out_SH = 14;  // 自己保持出力
const unsigned char pwm_MLED_R = 6;  // メインLED赤 PWM制御
const unsigned char pwm_MLED_G = 3;  // メインLED緑 PWM制御
const unsigned char pwm_MLED_B = 5;  // メインLED青 PWM制御
const unsigned char out_SLED3 = 7;  // サブLED3 点灯
const unsigned char out_SLED4 = 12;  // サブLED4 点灯
const unsigned char out_SLED5= 19;  // サブLED5 点灯
const unsigned char out_SLED6 = 17;  // サブLED6 点灯
const unsigned char pwm_SLED_R = 11;  // サブLED赤 PWM制御
const unsigned char pwm_SLED_G = 9;  // サブLED緑 PWM制御
const unsigned char pwm_SLED_B = 10;  // サブLED青 PWM制御
const unsigned char out_NA1 = 4;  // 何も接続しない
const unsigned char out_NA2 = 20;  // 何も接続しない
const unsigned char out_NA3 = 21;  // 何も接続しない

/* -------------------- */
// 初期設定
/* -------------------- */
void setup(){
  // 入出力設定
  // 0
  // 1
  pinMode(in_USBP, INPUT); // 2
  digitalWrite(pwm_MLED_B,HIGH);pinMode(pwm_MLED_B, OUTPUT);digitalWrite(pwm_MLED_B,HIGH);delay(30); // 3
  pinMode(out_NA1, OUTPUT);  // 4
  digitalWrite(pwm_MLED_G,HIGH);pinMode(pwm_MLED_G, OUTPUT);digitalWrite(pwm_MLED_G,HIGH);delay(30); // 5
  digitalWrite(pwm_MLED_R,HIGH);pinMode(pwm_MLED_R, OUTPUT);digitalWrite(pwm_MLED_R,HIGH);delay(30); // 6
  pinMode(out_SLED3, OUTPUT); // 7
  pinMode(in_SW4, INPUT_PULLUP); // 8
  pinMode(pwm_SLED_R, OUTPUT); // 9
  pinMode(pwm_SLED_G, OUTPUT); // 10
  pinMode(pwm_SLED_B, OUTPUT); // 11
  pinMode(out_SLED4, OUTPUT); // 12
  pinMode(in_SW3, INPUT_PULLUP); // 13
  pinMode(out_SH, OUTPUT); // 14
  pinMode(in_SW5, INPUT_PULLUP); // 15
  pinMode(in_SW2, INPUT_PULLUP); // 16
  pinMode(out_SLED6, OUTPUT); // 17
  pinMode(in_SW1, INPUT_PULLUP); // 18
  pinMode(out_SLED5, OUTPUT); // 19
  pinMode(out_NA2, OUTPUT);  // 20
  pinMode(out_NA3, OUTPUT);  // 21

  Serial.begin(9600); // ボーレート設定
  Serial.println("Betelgeuse Ver.C84");
  Serial.println("Please type \"mojizuri\" for enter the expart mode.");

  digitalWrite(out_NA1, LOW);
  digitalWrite(out_NA2, LOW);
  digitalWrite(out_NA3, LOW);

  restore_EEPROM();  // EEPROM読み出し
  delay(PowerOnDelay * 1000); // 起動チェック（SW5長押しで起動）
  digitalWrite(out_SH,LOW);  // 自己保持出力

  dechattering();delay(1);
  dechattering();delay(1);
  dechattering();delay(1);
  dechattering();delay(1);
  dechattering();delay(1);
  dechattering();delay(1);
  dechattering();delay(1);
  dechattering();delay(1);

  return;
}

/* -------------------- */
// メインループ
/* -------------------- */
void loop(){
  dechattering(); // 入力状態を安定させる（チャタリング対策）
  check_poweroff(); // 電源OFF？

  change_MLED(); // メインLED点灯色変更
  change_SLED(); // サブLED出力
  communication(); // シリアルコミュニケーション
  return;
}

/* -------------------- */
// 電源をOFFにする
/* -------------------- */
void check_poweroff(){
  if(stable_SWS!=PowerOffKeyConfig){
    return;
  }
  delay(PowerOffDelay * 125);dechattering();
  delay(PowerOffDelay * 125);dechattering();
  delay(PowerOffDelay * 125);dechattering();
  delay(PowerOffDelay * 125);dechattering();
  delay(PowerOffDelay * 125);dechattering();
  delay(PowerOffDelay * 125);dechattering();
  delay(PowerOffDelay * 125);dechattering();
  delay(PowerOffDelay * 125);dechattering();
  if(stable_SWS!=PowerOffKeyConfig){
    return;
  }
  digitalWrite(out_SH,HIGH);  // 自己保持終了
  digitalWrite(pwm_MLED_R,HIGH);
  digitalWrite(pwm_MLED_G,HIGH);
  digitalWrite(pwm_MLED_B,HIGH);

  delay(1000);

  while(stable_SW5==0){
    delay(100);
    dechattering();
  }  
  while(stable_SW5==1){
    dechattering();
    if(stable_SW5==0){
      delay(PowerOnDelay * 1000); // 起動チェック（SW5長押しで起動）
      if(stable_SW5==0){
        digitalWrite(out_SH,LOW);  // 自己保持出力
        if(restore_lastcolor == 1){
          analogWrite(pwm_MLED_R,EEPROM.read(31));
          analogWrite(pwm_MLED_G,EEPROM.read(32));
          analogWrite(pwm_MLED_B,EEPROM.read(33));
        }    
        return;
      }
    }
  }
  return;
}

/* -------------------- */
// プレビュLEDを点灯
/* -------------------- */
void change_SLED(){
  if((interlock==1&&stable_SW5==0&&preview==1)||(interlock==0&&preview==1)){
    analogWrite(pwm_SLED_R,255 - MAIN_R_1);
    analogWrite(pwm_SLED_G,255 - MAIN_G_1);
    analogWrite(pwm_SLED_B,255 - MAIN_B_1);
    digitalWrite(out_SLED6,HIGH);
    delayMicroseconds(100);
    digitalWrite(out_SLED6,LOW);

    analogWrite(pwm_SLED_R,255 - MAIN_R_2);
    analogWrite(pwm_SLED_G,255 - MAIN_G_2);
    analogWrite(pwm_SLED_B,255 - MAIN_B_2);
    digitalWrite(out_SLED5,HIGH);
    delayMicroseconds(100);
    digitalWrite(out_SLED5,LOW);

    analogWrite(pwm_SLED_R,255 - MAIN_R_3);
    analogWrite(pwm_SLED_G,255 - MAIN_G_3);
    analogWrite(pwm_SLED_B,255 - MAIN_B_3);
    digitalWrite(out_SLED4,HIGH);
    delayMicroseconds(100);
    digitalWrite(out_SLED4,LOW);

    analogWrite(pwm_SLED_R,255 - MAIN_R_4);
    analogWrite(pwm_SLED_G,255 - MAIN_G_4);
    analogWrite(pwm_SLED_B,255 - MAIN_B_4);
    digitalWrite(out_SLED3,HIGH);
    delayMicroseconds(100);
    digitalWrite(out_SLED3,LOW);
  } 
  return;
}

/* -------------------- */
// メインLEDの色を変更
/* -------------------- */
void change_MLED(){
  static unsigned char last = 0;
  unsigned char now = 0;
  unsigned char tmp_stable_SWS = stable_SWS;  
  if (interlock==0){
    tmp_stable_SWS&=B01111;
  }
  unsigned char MAIN_R_0 = 0;
  unsigned char MAIN_G_0 = 0;
  unsigned char MAIN_B_0 = 0;

  switch (tmp_stable_SWS){
  case B01110:
    MAIN_R_0 = MAIN_R_1;
    MAIN_G_0 = MAIN_G_1;
    MAIN_B_0 = MAIN_B_1;
    now=1;
    break;

  case B01101:
    MAIN_R_0 = MAIN_R_2;
    MAIN_G_0 = MAIN_G_2;
    MAIN_B_0 = MAIN_B_2;
    now=2;
    break;

  case B01011:
    MAIN_R_0 = MAIN_R_3;
    MAIN_G_0 = MAIN_G_3;
    MAIN_B_0 = MAIN_B_3;
    now=3;
    break;

  case B00111:
    MAIN_R_0 = MAIN_R_4;
    MAIN_G_0 = MAIN_G_4;
    MAIN_B_0 = MAIN_B_4;
    now=4;
    break;

  default:
    return;
  }
  if(last==now){return;}
  last=now;
  analogWrite(pwm_MLED_R,255 - MAIN_R_0);
  analogWrite(pwm_MLED_G,255 - MAIN_G_0);
  analogWrite(pwm_MLED_B,255 - MAIN_B_0);

  if(restore_lastcolor == 1){
    EEPROM.write(0, B10010110);        // EEPROMをロック
    EEPROM.write(31, 255 - MAIN_R_0);
    EEPROM.write(32, 255 - MAIN_G_0);
    EEPROM.write(33, 255 - MAIN_B_0);
    EEPROM.write(0,B01110001);         // EEPROMロック解除
  }
  return;
}

/* -------------------- */
// EEPROMから復元
/* -------------------- */
void restore_EEPROM(){
  if (EEPROM.read(0) != B01110001){
    store_EEPROM();  // EEPROMを初期化
  }
  else{
    // EEPROMは書き込み済み？
    PowerOnDelay = EEPROM.read(1);
    PowerOffDelay = EEPROM.read(2);
    PowerOffKeyConfig = EEPROM.read(3);
    interlock = EEPROM.read(4);
    preview = EEPROM.read(5);

    MAIN_R_1 = EEPROM.read(6);
    MAIN_G_1 = EEPROM.read(7);
    MAIN_B_1 = EEPROM.read(8);

    MAIN_R_2 = EEPROM.read(9);
    MAIN_G_2 = EEPROM.read(10);
    MAIN_B_2 = EEPROM.read(11);

    MAIN_R_3 = EEPROM.read(12);
    MAIN_G_3 = EEPROM.read(13);
    MAIN_B_3 = EEPROM.read(14);

    MAIN_R_4 = EEPROM.read(15);
    MAIN_G_4 = EEPROM.read(16);
    MAIN_B_4 = EEPROM.read(17);

    restore_lastcolor = EEPROM.read(30);
    if(restore_lastcolor == 1){
      analogWrite(pwm_MLED_R,EEPROM.read(31));
      analogWrite(pwm_MLED_G,EEPROM.read(32));
      analogWrite(pwm_MLED_B,EEPROM.read(33));
    }    
  } 
  return;
}

/* -------------------- */
// EEPROMへ保存
/* -------------------- */
void store_EEPROM(){
  EEPROM.write(0, B10010110);  // EEPROMをロック

  EEPROM.write(1,PowerOnDelay);
  EEPROM.write(2,PowerOffDelay);
  EEPROM.write(3,PowerOffKeyConfig);
  EEPROM.write(4,interlock);
  EEPROM.write(5,preview);

  EEPROM.write(6,MAIN_R_1);
  EEPROM.write(7,MAIN_G_1);
  EEPROM.write(8,MAIN_B_1);

  EEPROM.write(9,MAIN_R_2);
  EEPROM.write(10,MAIN_G_2);
  EEPROM.write(11,MAIN_B_2);

  EEPROM.write(12,MAIN_R_3);
  EEPROM.write(13,MAIN_G_3);
  EEPROM.write(14,MAIN_B_3);

  EEPROM.write(15,MAIN_R_4);
  EEPROM.write(16,MAIN_G_4);
  EEPROM.write(17,MAIN_B_4);

  EEPROM.write(30,restore_lastcolor);

  EEPROM.write(0,B01110001);  // EEPROMを書き込み済みにする。
  return;
}

/* -------------------- */
// チャタリング対策
/* -------------------- */
void dechattering(){
  static unsigned char tmp_USBP = 0;
  static unsigned char tmp_SW1 = 0;
  static unsigned char tmp_SW2 = 0;
  static unsigned char tmp_SW3 = 0;
  static unsigned char tmp_SW4 = 0;
  static unsigned char tmp_SW5 = 0;

  tmp_USBP <<= 1;
  tmp_SW1 <<= 1;
  tmp_SW2 <<= 1;
  tmp_SW3 <<= 1;
  tmp_SW4 <<= 1;
  tmp_SW5 <<= 1;

  tmp_USBP &= B11111110;
  tmp_SW1 &= B11111110;
  tmp_SW2 &= B11111110;
  tmp_SW3 &= B11111110;
  tmp_SW4 &= B11111110;
  tmp_SW5 &= B11111110;

  tmp_USBP |= ((digitalRead(in_USBP)==HIGH)?1:0);
  tmp_SW1 |= ((digitalRead(in_SW1)==HIGH)?1:0);
  tmp_SW2 |= ((digitalRead(in_SW2)==HIGH)?1:0);
  tmp_SW3 |= ((digitalRead(in_SW3)==HIGH)?1:0);
  tmp_SW4 |= ((digitalRead(in_SW4)==HIGH)?1:0);
  tmp_SW5 |= ((digitalRead(in_SW5)==HIGH)?1:0);

  if (tmp_USBP==B00111111){stable_USBP=1;}
  if (tmp_SW1==B00111111){stable_SW1=1;}
  if (tmp_SW2==B00111111){stable_SW2=1;}
  if (tmp_SW3==B00111111){stable_SW3=1;}
  if (tmp_SW4==B00111111){stable_SW4=1;}
  if (tmp_SW5==B00111111){stable_SW5=1;}

  if (tmp_USBP==B11000000){stable_USBP=0;}
  if (tmp_SW1==B11000000){stable_SW1=0;}
  if (tmp_SW2==B11000000){stable_SW2=0;}
  if (tmp_SW3==B11000000){stable_SW3=0;}
  if (tmp_SW4==B11000000){stable_SW4=0;}
  if (tmp_SW5==B11000000){stable_SW5=0;}

  stable_SWS = (stable_SW5 << 4) | (stable_SW4 << 3) | (stable_SW3 << 2) | (stable_SW2 << 1) | (stable_SW1 << 0);

  return;
}


/* -------------------- */
// シリアルコミュニケーション
/* -------------------- */
void communication(){
  if(Serial.available()<=7){return;}
  if(Serial.read()!='m'||Serial.read()!='o'||Serial.read()!='j'||Serial.read()!='i'||Serial.read()!='z'||Serial.read()!='u'||Serial.read()!='r'||Serial.read()!='i'){return;}
  Serial.println("========================");
  Serial.println("Welcome to Betelgeuse Administration Console!");
  Serial.println("==========MENU==========");
  Serial.println("  1: Setting");
  Serial.println("  2: ColorConfig");
  Serial.println("  3: Exit");
  Serial.println("========================");
  Serial.println("Please type [1-3]?:");
  Serial.flush();while(Serial.available()){Serial.read();}

  while(1){
    while(Serial.available() <= 0){}
    switch(Serial.read()){
    case '1':
      serial_setting();
      break;
    case '2':
      serial_colorconfig();
      break;
    case '3':
      Serial.println("\nBye Bye!\n");
      Serial.println("Please type \"mojizuri\" for enter the expart mode.");
      return;
    default:
      break;
    }

    Serial.println("==========MENU==========");
    Serial.println("  1: Setting");
    Serial.println("  2: ColorConfig");
    Serial.println("  3: Exit");
    Serial.println("========================");
    Serial.println("Please type [1-3]?:");

    Serial.flush();while(Serial.available()){Serial.read();}
  }
  return;
}

/* -------------------- */
// 各種設定
/* -------------------- */
void serial_setting(){
  Serial.println("=====Setting=====");
  Serial.print("  1: Power ON delay [time=");Serial.print(PowerOnDelay,DEC);Serial.println("sec]");
  Serial.print("  2: Power OFF delay [time=");Serial.print(PowerOffDelay,DEC);Serial.println("sec]");
  Serial.print("  3: Power OFF keyconfig [SW(54321)=");Serial.print(PowerOffKeyConfig,BIN);Serial.println("] (0=PUSH, 1=RELEASE)");
  Serial.print("  4: Toggle Interlock [");if(interlock==1){Serial.println("ENABLE]");}else{Serial.println("DISABLE]");}
  Serial.print("  5: Toggle Preview [");if(preview==1){Serial.println("ENABLE]");}else{Serial.println("DISABLE]");}
  Serial.print("  6: Toggle RestoreLastColor [");if(restore_lastcolor==1){Serial.println("ENABLE]");}else{Serial.println("DISABLE]");}
  Serial.println("  7: Exit(with save)");
  Serial.println("  8: Exit(WITHOUT save)");
  Serial.println("  9: Factory Reset");
  Serial.println("========================");
  Serial.println("Please type [1-9]?:");

  while(1){
    Serial.flush();while(Serial.available()){Serial.read();}
    while(Serial.available() <= 0){}
    switch(Serial.read()){
    case '1':
      Serial.println("Please type Power ON delay value (3digits 000-255)");
      Serial.flush();while(Serial.available()){Serial.read();}
      while(Serial.available() <= 2){}
      PowerOnDelay = (Serial.read()-48) * 100;
      PowerOnDelay += (Serial.read()-48) * 10;
      PowerOnDelay += (Serial.read()-48) * 1;
      break;
    case '2':
      Serial.println("Please type Power OFF delay value (3digits 000-255)");
      Serial.flush();while(Serial.available()){Serial.read();}
      while(Serial.available() <= 2){}
      PowerOffDelay = (Serial.read()-48) * 100;
      PowerOffDelay += (Serial.read()-48) * 10;
      PowerOffDelay += (Serial.read()-48) * 1;
      break;
    case '3':
      Serial.println("Please type Power OFF KeyConfig value (5digits BINARY 00000-11111)");
      Serial.flush();while(Serial.available()){Serial.read();}
      while(Serial.available() <= 4){}
      PowerOffKeyConfig = 0;
      if((Serial.read()-48)==1){PowerOffKeyConfig |= (1<<4);}
      if((Serial.read()-48)==1){PowerOffKeyConfig |= (1<<3);}
      if((Serial.read()-48)==1){PowerOffKeyConfig |= (1<<2);}
      if((Serial.read()-48)==1){PowerOffKeyConfig |= (1<<1);}
      if((Serial.read()-48)==1){PowerOffKeyConfig |= (1<<0);}
      break;
    case '4':
      interlock^=1;
      break;
    case '5':
      preview^=1;
      break;
    case '6':
      restore_lastcolor^=1;
      break;
    case '7':
      store_EEPROM();
      return;
    case '8':
      return;
    case '9':
      EEPROM.write(0, B10010110);
      Serial.println("Please plugout USB.");
      while(1){}
      return;
    default:
      break;
    }

    Serial.println("=====Setting=====");
    Serial.print("  1: Power ON delay [time=");Serial.print(PowerOnDelay,DEC);Serial.println("sec]");
    Serial.print("  2: Power OFF delay [time=");Serial.print(PowerOffDelay,DEC);Serial.println("sec]");
    Serial.print("  3: Power OFF keyconfig [SW(54321)=");Serial.print(PowerOffKeyConfig,BIN);Serial.println("] (0=PUSH, 1=RELEASE)");
    Serial.print("  4: Toggle Interlock [");if(interlock==1){Serial.println("ENABLE]");}else{Serial.println("DISABLE]");}
    Serial.print("  5: Toggle Preview [");if(preview==1){Serial.println("ENABLE]");}else{Serial.println("DISABLE]");}
    Serial.print("  6: Toggle RestoreLastColor [");if(restore_lastcolor==1){Serial.println("ENABLE]");}else{Serial.println("DISABLE]");}
    Serial.println("  7: Exit(with save)");
    Serial.println("  8: Exit(WITHOUT save)");
    Serial.println("  9: Factory Reset");
    Serial.println("========================");
    Serial.println("Please type [1-9]?:");

    Serial.flush();while(Serial.available()){Serial.read();}
  }
  return;
}


/* -------------------- */
// 色設定
/* -------------------- */
void serial_colorconfig(){
  Serial.println("=====ColorConfig=====");
  Serial.print("  1: ChangePreset1 [R=");Serial.print(MAIN_R_1,DEC);Serial.print("  G=");Serial.print(MAIN_G_1,DEC);Serial.print("  B=");Serial.print(MAIN_B_1,DEC);Serial.println("]");
  Serial.print("  2: ChangePreset2 [R=");Serial.print(MAIN_R_2,DEC);Serial.print("  G=");Serial.print(MAIN_G_2,DEC);Serial.print("  B=");Serial.print(MAIN_B_2,DEC);Serial.println("]");
  Serial.print("  3: ChangePreset3 [R=");Serial.print(MAIN_R_3,DEC);Serial.print("  G=");Serial.print(MAIN_G_3,DEC);Serial.print("  B=");Serial.print(MAIN_B_3,DEC);Serial.println("]");
  Serial.print("  4: ChangePreset4 [R=");Serial.print(MAIN_R_4,DEC);Serial.print("  G=");Serial.print(MAIN_G_4,DEC);Serial.print("  B=");Serial.print(MAIN_B_4,DEC);Serial.println("]");
  Serial.println("  5: Exit");
  Serial.println("========================");
  Serial.println("Please type [1-5]?:");

  while(1){Serial.flush();while(Serial.available()){Serial.read();}
    while(Serial.available() <= 0){}
    switch(Serial.read()){
    case '1':
      serial_realtimeconfig(1);
      break;
    case '2':
      serial_realtimeconfig(2);
      break;
    case '3':
      serial_realtimeconfig(3);
      break;
    case '4':
      serial_realtimeconfig(4);
      break;
    case '5':
      return;
    default:
      break;
    }

    Serial.println("=====ColorConfig=====");
    Serial.print("  1: ChangePreset1 [R=");Serial.print(MAIN_R_1,DEC);Serial.print("  G=");Serial.print(MAIN_G_1,DEC);Serial.print("  B=");Serial.print(MAIN_B_1,DEC);Serial.println("]");
    Serial.print("  2: ChangePreset2 [R=");Serial.print(MAIN_R_2,DEC);Serial.print("  G=");Serial.print(MAIN_G_2,DEC);Serial.print("  B=");Serial.print(MAIN_B_2,DEC);Serial.println("]");
    Serial.print("  3: ChangePreset3 [R=");Serial.print(MAIN_R_3,DEC);Serial.print("  G=");Serial.print(MAIN_G_3,DEC);Serial.print("  B=");Serial.print(MAIN_B_3,DEC);Serial.println("]");
    Serial.print("  4: ChangePreset4 [R=");Serial.print(MAIN_R_4,DEC);Serial.print("  G=");Serial.print(MAIN_G_4,DEC);Serial.print("  B=");Serial.print(MAIN_B_4,DEC);Serial.println("]");
    Serial.println("  5: Exit");
    Serial.println("========================");
    Serial.println("Please type [1-5]?:");

    Serial.flush();while(Serial.available()){Serial.read();}
  }
  return;
}

/* -------------------- */
// プリセット編集
/* -------------------- */
void serial_realtimeconfig(unsigned char no){
  unsigned char MAIN_R_P = 0;
  unsigned char MAIN_G_P = 0;
  unsigned char MAIN_B_P = 0;
  unsigned char MAIN_R_N = 0;
  unsigned char MAIN_G_N = 0;
  unsigned char MAIN_B_N = 0;

  switch(no){
  case 1:
    MAIN_R_P = MAIN_R_1;
    MAIN_G_P = MAIN_G_1;
    MAIN_B_P = MAIN_B_1;
    break;
  case 2:
    MAIN_R_P = MAIN_R_2;
    MAIN_G_P = MAIN_G_2;
    MAIN_B_P = MAIN_B_2;
    break;
  case 3:
    MAIN_R_P = MAIN_R_3;
    MAIN_G_P = MAIN_G_3;
    MAIN_B_P = MAIN_B_3;
    break;
  case 4:
    MAIN_R_P = MAIN_R_4;
    MAIN_G_P = MAIN_G_4;
    MAIN_B_P = MAIN_B_4;
    break;
  }
  MAIN_R_N = MAIN_R_P;
  MAIN_G_N = MAIN_G_P;
  MAIN_B_N = MAIN_B_P;

  Serial.print("=====ChangePreset");Serial.print(no);Serial.println("=====");
  Serial.print("PrevColor:  R=");Serial.print(MAIN_R_P,DEC);Serial.print("  G=");Serial.print(MAIN_G_P,DEC);Serial.print("  B=");Serial.print(MAIN_B_P,DEC);Serial.println("");
  Serial.print("NewColor:  R=");Serial.print(MAIN_R_N,DEC);Serial.print("  G=");Serial.print(MAIN_G_N,DEC);Serial.print("  B=");Serial.print(MAIN_B_N,DEC);Serial.println("");
  Serial.println("  1: EditRed");
  Serial.println("  2: EditGreen");
  Serial.println("  3: EditBlue");
  Serial.println("  4: Preview NewColor");    
  Serial.println("  5: Preview PrevColor");
  Serial.println("  6: Exit(with save)");
  Serial.println("  7: Exit(WITHOUT save)");
  Serial.println("========================");
  Serial.println("Please type [1-7]?:");
  analogWrite(pwm_MLED_R,255 - MAIN_R_N);
  analogWrite(pwm_MLED_G,255 - MAIN_G_N);
  analogWrite(pwm_MLED_B,255 - MAIN_B_N);

  while(1){
    Serial.flush();while(Serial.available()){Serial.read();}
    while(Serial.available() <= 0){}
    switch(Serial.read()){
    case '1':
      Serial.println("Please type RED value (3digits 000-255)");
      Serial.flush();while(Serial.available()){Serial.read();}
      while(Serial.available() <= 2){}
      MAIN_R_N = (Serial.read()-48) * 100;
      MAIN_R_N += (Serial.read()-48) * 10;
      MAIN_R_N += (Serial.read()-48) * 1;
      analogWrite(pwm_MLED_R,255 - MAIN_R_N);
      analogWrite(pwm_MLED_G,255 - MAIN_G_N);
      analogWrite(pwm_MLED_B,255 - MAIN_B_N);
      break;
    case '2':
      Serial.println("Please type GREEN value (3digits 000-255)");
      Serial.flush();while(Serial.available()){Serial.read();}
      while(Serial.available() <= 2){}
      MAIN_G_N = (Serial.read()-48) * 100;
      MAIN_G_N += (Serial.read()-48) * 10;
      MAIN_G_N += (Serial.read()-48) * 1;
      analogWrite(pwm_MLED_R,255 - MAIN_R_N);
      analogWrite(pwm_MLED_G,255 - MAIN_G_N);
      analogWrite(pwm_MLED_B,255 - MAIN_B_N);
      break;
    case '3':
      Serial.println("Please type BLUE value (3digits 000-255)");
      Serial.flush();while(Serial.available()){Serial.read();}
      while(Serial.available() <= 2){}
      MAIN_B_N = (Serial.read()-48) * 100;
      MAIN_B_N += (Serial.read()-48) * 10;
      MAIN_B_N += (Serial.read()-48) * 1;
      analogWrite(pwm_MLED_R,255 - MAIN_R_N);
      analogWrite(pwm_MLED_G,255 - MAIN_G_N);
      analogWrite(pwm_MLED_B,255 - MAIN_B_N);
      break;
    case '4':
      analogWrite(pwm_MLED_R,255 - MAIN_R_N);
      analogWrite(pwm_MLED_G,255 - MAIN_G_N);
      analogWrite(pwm_MLED_B,255 - MAIN_B_N);
      break;
    case '5':
      analogWrite(pwm_MLED_R,255 - MAIN_R_P);
      analogWrite(pwm_MLED_G,255 - MAIN_G_P);
      analogWrite(pwm_MLED_B,255 - MAIN_B_P);
      break;
    case '6':
      switch(no){
      case 1:
        MAIN_R_1 = MAIN_R_N;
        MAIN_G_1 = MAIN_G_N;
        MAIN_B_1 = MAIN_B_N;
        break;
      case 2:
        MAIN_R_2 = MAIN_R_N;
        MAIN_G_2 = MAIN_G_N;
        MAIN_B_2 = MAIN_B_N;
        break;
      case 3:
        MAIN_R_3 = MAIN_R_N;
        MAIN_G_3 = MAIN_G_N;
        MAIN_B_3 = MAIN_B_N;
        break;
      case 4:
        MAIN_R_4 = MAIN_R_N;
        MAIN_G_4 = MAIN_G_N;
        MAIN_B_4 = MAIN_B_N;
        break;
      }
      store_EEPROM();
      return;
    case '7':
      return;
    default:
      break;
    }

    Serial.print("=====ChangePreset");Serial.print(no);Serial.println("=====");
    Serial.print("PrevColor:  R=");Serial.print(MAIN_R_P,DEC);Serial.print("  G=");Serial.print(MAIN_G_P,DEC);Serial.print("  B=");Serial.print(MAIN_B_P,DEC);Serial.println("");
    Serial.print("NewColor:  R=");Serial.print(MAIN_R_N,DEC);Serial.print("  G=");Serial.print(MAIN_G_N,DEC);Serial.print("  B=");Serial.print(MAIN_B_N,DEC);Serial.println("");
    Serial.println("  1: EditRed");
    Serial.println("  2: EditGreen");
    Serial.println("  3: EditBlue");
    Serial.println("  4: Preview NewColor");    
    Serial.println("  5: Preview PrevColor");
    Serial.println("  6: Exit(with save)");
    Serial.println("  7: Exit(WITHOUT save)");
    Serial.println("========================");
    Serial.println("Please type [1-7]?:");
    Serial.flush();while(Serial.available()){Serial.read();}
  }
  return;
}

/* ===== END OF PROGRAM ===== */


