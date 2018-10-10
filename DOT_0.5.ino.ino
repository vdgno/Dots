/*********************************************************************
  This is an example for our Monochrome OLEDs based on SSD1306 drivers
  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98
  This example is for a 128x64 size display using I2C to communicate
  3 pins are required to interface (2 I2C and one reset)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Limor Fried/Ladyada  for Adafruit Industries.
  BSD license, check license.txt for more information
  All text above, and the splash screen must be included in any redistribution
*********************************************************************/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include  <Chrono.h>
#include <Button.h>
// new branch

// If using software SPI (the default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
#define CLOCK A2
#define ENCA 2
#define ENCB 3
#define GATEOUT A1
#define BUTTON_TRIG 4
#define RECORD A5
#define RECLED A4
#define RETRIG A3

Button button_trig = Button(BUTTON_TRIG);
Button clock_input = Button(CLOCK);

Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


int STEP = 0;
int STEP_prev = 0;
int trig_btn_state = 0;
int rec_btn_state = 0;
int retrig_state = 0;
int encstateA = 0;
int encstateB = 0;
int pattern_length = 8;
//int x_point = 0;
//int y_point = 0;
int x_point_prev = 0;
int y_point_prev = 0;
int x_wrap = 0;

Chrono trigtimer;


int seqArray[64] = {0, 1, 1, 0, 0, 0, 0, 1,
                    0, 0, 1, 0, 0, 0, 0, 1,
                    0, 1, 0, 0, 1, 1, 0, 1,
                    1, 1, 1, 0, 0, 0, 0, 0,
                    0, 1, 1, 0, 0, 0, 0, 0,
                    1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 0, 0, 1, 1, 0,
                    0, 0, 1, 1, 1, 1, 1, 1,

                   };


void setup()   {
  pinMode (CLOCK, INPUT);
  pinMode (GATEOUT, OUTPUT);
  pinMode (BUTTON_TRIG, INPUT);
  pinMode (RECORD, INPUT);
  pinMode (RECLED, OUTPUT);
  pinMode (RETRIG, INPUT);
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  attachInterrupt(0, Step_bw, CHANGE);
  attachInterrupt(1, Step_fw, CHANGE);

}
int x_point(int step_num) {
  int x = 0 ;
  if (step_num <= 8) {
    x = 4 + x_wrap;
  }
  if ( step_num > 8 && step_num <= 16 ) {
    x = 12 + x_wrap;
  }
  if ( step_num > 16 && step_num <= 24 ) {
    x = 20 + x_wrap;
  }
  if ( step_num > 24 && step_num <= 32 ) {
    x = 28 + x_wrap;
  }
  if ( step_num > 32 && step_num <= 40 ) {
    x = 36 + x_wrap;
  }
  if ( step_num > 40 && step_num <= 48 ) {
    x = 44 + x_wrap;
  }
  if ( step_num > 48 && step_num <= 56 ) {
    x = 52 + x_wrap;
  }
  if ( step_num > 56 && step_num <= 64 ) {
    x = 60 + x_wrap;
  }
  if ( step_num > 64 ) {
    x = 68 + x_wrap;
  }
  return x;
}
int y_point(int step_num) {
  int y = 0 ;
  int row_val = 0;
  if (step_num >= 9) {
    row_val = 8;
  }
  if ( step_num >= 17 ) {
    row_val = 16;
  }
  if ( step_num >= 25 ) {
    row_val = 24;
  }
  if ( step_num >= 33 ) {
    row_val = 32;
  }
  if ( step_num >= 41 ) {
    row_val = 40;
  }
  if ( step_num >= 49 ) {
    row_val = 48;
  }
  if ( step_num >= 57 ) {
    row_val = 56;
  }
  if ( step_num >= 65 ) {
    row_val = 64;
  }
  y = step_num - row_val;
  return y;
}

void Show_trigs() {
  //  Странный глюк при включенном клоке смещается дисплей!!!!
  float x_rows = pattern_length  / 8;
  x_wrap = 32 - x_rows * 4;
  display.clearDisplay();
  for ( int i = 1; i <= pattern_length; i++) {
    Serial.print(y_point(i));
    display.fillCircle(x_point(i), y_point(i) * 8 - 4, 0.5, WHITE);
    if (seqArray[i-1] == 1) {
      display.fillCircle(x_point(i), y_point(i) * 8 - 4, 3, WHITE);
    }
  };
  display.display();
}

void Show_run() {
  if (seqArray[STEP - 1] == 1) {
    display.fillCircle(x_point(STEP), y_point(STEP) * 8 - 4, 4, WHITE);
  }
  else {
    display.fillCircle(x_point(STEP), y_point(STEP) * 8 - 4, 1, WHITE);
  }
  //erase previos step
  if (seqArray[STEP_prev - 1] == 1) {
    display.fillCircle(x_point(STEP_prev), y_point(STEP_prev) * 8 - 4, 4, BLACK);
    display.fillCircle(x_point(STEP_prev), y_point(STEP_prev) * 8 - 4, 3, WHITE);
  }
  else {
    display.fillCircle(x_point(STEP_prev), y_point(STEP_prev) * 8 - 4, 1, BLACK);
    display.fillCircle(x_point(STEP_prev), y_point(STEP_prev) * 8 - 4, 0.5, WHITE);
  }
  display.display();



}

void Trig_out() {

  if (seqArray[(STEP - 1)] == 1) {
    Signal_out();

  }
  else {
    Signal_out_stop();
    //Serial.println("SIGNAL_STOP");
  }
}


void Signal_out() {
  trigtimer.restart();
  digitalWrite(GATEOUT, HIGH);
}

void Signal_out_stop() {
  //trigtimer.restart();
  trigtimer.stop();
  digitalWrite(GATEOUT, LOW);
}

void loop() {


  if (trigtimer.hasPassed(10)) {
    Signal_out_stop();
  }

  if (button_trig.uniquePress()) {
    trig_btn_state = 1;
    Signal_out();
  }
  else {
    if (button_trig.stateChanged()) {
      trig_btn_state = 0;
      Signal_out_stop();
      //Serial.println("wasPressed");
    }
  }

  rec_btn_state = digitalRead(RECORD);
  if (rec_btn_state == HIGH) {
    digitalWrite(RECLED, HIGH);
  }
  else {
    digitalWrite(RECLED, LOW);
  }

  retrig_state = digitalRead(RETRIG);
  if (retrig_state == HIGH) {
    //Serial.print("retrig");

  }
  Step_on();

}
void Step_on() {
  if (clock_input.uniquePress()) {
    STEP_prev = STEP;
    STEP++;
    if (STEP > pattern_length) {
      STEP = 1;
    };
    Trig_out();
    //Show_trigs();
    Show_run();

  }

}




void Step_fw() {
  encstateB = digitalRead(ENCB);
  if (encstateB == LOW) {
    pattern_length++;
    Show_trigs();
  }
}
void Step_bw() {
  encstateA = digitalRead(ENCA);
  if (encstateA == LOW) {
    pattern_length--;
    Show_trigs();
  }
}


