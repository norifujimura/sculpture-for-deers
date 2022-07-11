//#include <M5StickC.h>

/*
    Description: Control RGB LED to run rainbow light show
    Please install library before compiling:  
    FastLED: https://github.com/FastLED/FastLED
*/
//#include <M5Stack.h>
#include <M5Core2.h>
#include "FastLED.h"

#define Neopixel_PIN 32
#define NUM_LEDS    120 // 60LEDs per 1m

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 127// Set BRIGHTNESS to about 1/4 (max = 255)

CRGB leds[NUM_LEDS];
int counter = 0;
uint8_t gHue = 0;
static TaskHandle_t FastLEDshowTaskHandle = 0;
static TaskHandle_t userTaskHandle = 0;

void setup() {
  M5.begin();
  //M5.Power.begin();

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(YELLOW); 
  M5.Lcd.setTextSize(1); 
  M5.Lcd.setCursor(0,10);
  M5.Lcd.println("Neopixel Example");
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(0, 20);
  M5.Lcd.println("Display rainbow effect");

  // Neopixel initialization
  FastLED.addLeds<WS2811,Neopixel_PIN,GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(10);
  xTaskCreatePinnedToCore(FastLEDshowTask, "FastLEDshowTask", 2048, NULL, 2, NULL, 1);
}

void loop()
{
  M5.update();
 if(M5.BtnA.wasPressed()) {
  Serial.println("BtnA.wasPressed() == TRUE");
  counter = counter + 1;
  if(counter>5){
    counter = 0;
  }
 }
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

void FastLEDshowTask(void *pvParameters)
{
    for(;;) {

        confettiWhite() ;
        // fill_rainbow(leds, NUM_LEDS, gHue, 7);// rainbow effect
        /*
        switch (counter) {
          case 0:
            rainbow(); 
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
          default:
            // statements
            break;
        }
        */
        FastLED.show();// must be executed for neopixel becoming effective
        EVERY_N_MILLISECONDS( 2 ) { gHue++; }
    }
}
