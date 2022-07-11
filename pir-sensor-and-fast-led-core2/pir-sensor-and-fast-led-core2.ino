/*
*******************************************************************************
* Copyright (c) 2022 by M5Stack
*                  Equipped with M5Core sample source code
*                          配套  M5Core 示例源代码
* Visit for more information: https://docs.m5stack.com/en/core/gray
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/core/gray
*
* Describe: PIR.  人体红外
* Date: 2021/8/11
*******************************************************************************
  Please connect to Port B(26、36),Human body detection using PIR Unit.
  请连接端口B(26、36),使用PIR Unit进行人体检测。
*/

//#include <M5Stack.h>
#include <M5Core2.h>
#include "FastLED.h"

#define PIR_PIN 33
#define FRAME_MSEC 400

#define Neopixel_PIN 32
#define NUM_LEDS    120 // 60LEDs per 1m

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 200// Set BRIGHTNESS to about 1/4 (max = 255)

CRGB leds[NUM_LEDS];
int counter = 1;
int lightNum = 0;
uint8_t gHue = 0;
static TaskHandle_t FastLEDshowTaskHandle = 0;
static TaskHandle_t userTaskHandle = 0;

void setup() {
    M5.begin();             // Init M5Stack.  初始化M5Stack
    //M5.Power.begin();       // Init power  初始化电源模块
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(YELLOW); 
    M5.Lcd.setTextSize(2); 
    M5.Lcd.setCursor(0,10);
    M5.Lcd.println("LED + PIR Sensor 5M");
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setCursor(0, 20);
    M5.Lcd.println("Display confetti effect");
    
    pinMode(PIR_PIN, INPUT);  // Set pin 36 to input mode.  设置36号引脚为输入模式

    // Neopixel initialization
    FastLED.addLeds<WS2811,Neopixel_PIN,GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(10);
    xTaskCreatePinnedToCore(FastLEDshowTask, "FastLEDshowTask", 2048, NULL, 2, NULL, 1);
}


void loop() {
  M5.update();

     if(M5.BtnA.wasPressed()) {
      Serial.println("BtnA.wasPressed() == TRUE");
      counter = counter + 1;
      if(counter>6){
        counter = 1;
      }
    }
 
 // Draw a black rectangle 180 by 50 at (90,25).
                             // 在(90,25)处画一个宽180高50的黑的矩形
    if (digitalRead(PIR_PIN) ==
        1) {  // If pin 36 reads a value of 1.  如果36号引脚的读取到的值为1

          if(lightNum == 0){
            counter = counter + 1;
            if(counter>6){
              counter = 1;
            }
          }
        lightNum = counter;
        M5.Lcd.fillRect(90, 25, 180, 50,DARKGREY); 
        //M5.Lcd.fillScreen(DARKGREY);
        M5.Lcd.setCursor(95, 25);
        M5.Lcd.print("Sensing");
        M5.Lcd.setCursor(95, 45);
        M5.Lcd.print(digitalRead(PIR_PIN));
    } else {
        lightNum = 0;
        M5.Lcd.fillRect(90, 25, 180, 50,BLACK); 
        //M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(95, 25);
        M5.Lcd.print("Not Sensed");
        M5.Lcd.setCursor(95, 45);
        M5.Lcd.print(digitalRead(PIR_PIN));
    }
    delay(FRAME_MSEC);
}


void FastLEDshowESP32()
{
    if (userTaskHandle == 0) {
        userTaskHandle = xTaskGetCurrentTaskHandle();
        xTaskNotifyGive(FastLEDshowTaskHandle);
        const TickType_t xMaxBlockTime = pdMS_TO_TICKS( 200 );
        ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
        userTaskHandle = 0;
    }
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void confettiWhite() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CRGB::White;
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void off()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  //int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  //leds[pos] += CHSV( gHue, 255, 192);
}

void FastLEDshowTask(void *pvParameters)
{
    for(;;) {
        // confettiWhite() ;
        // fill_rainbow(leds, NUM_LEDS, gHue, 7);// rainbow effect
        
        switch (lightNum) {
          case 0:
            off();
            // OFF 
            break;
          case 1:
            rainbowWithGlitter();
            break;
          case 2:
            confetti();
            break;
          case 3:
            sinelon();
            break;
          case 4:
            bpm();
            break;
          case 5:
            juggle();
            break;
          case 6:
            rainbow(); 
            break;
          default:
            // statements
            break;
        }
        
        FastLED.show();// must be executed for neopixel becoming effective
        EVERY_N_MILLISECONDS( 2 ) { gHue++; }
    }
}
