#include <FastLED.h>

//#include "FastLED.h"

// How many leds in your strip?
#define NUM_LEDS 47

#define BRIGHTNESS_DEFAULT 100

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN

#define DATA_PIN01 11
#define DATA_PIN02 12

#define NUM_PATTERNS 2
typedef void (* GenericFP)();
GenericFP patterns[NUM_PATTERNS] = {&rainbow, 
                                    &chasing_random_colors};
#define NUM_LINES 2


CRGB leds[NUM_LINES][NUM_LEDS];

// Defines for p_color_temp
#define TEMPERATURE_1 Tungsten100W
#define TEMPERATURE_2 OvercastSky
// How many seconds to show each temperature before switching
#define DISPLAYTIME 20
// How many seconds to show black between switches
#define BLACKTIME   3


void setup() { 
       FastLED.addLeds<WS2812B, DATA_PIN01, GRB>(leds[0], NUM_LEDS);
       FastLED.addLeds<WS2812B, DATA_PIN02, GRB>(leds[1], NUM_LEDS);
       FastLED.clear();
       FastLED.setBrightness(BRIGHTNESS_DEFAULT);
       FastLED.show();
}

void loop() {
//  chasing_random_colors();
  for(int x=0;x<NUM_PATTERNS;x++)
    patterns[x]();
}


CRGB get_random_color() {
  CHSV hsv(random8(), 255, 255);
  CRGB rgb;
  hsv2rgb_rainbow( hsv, rgb);
  return rgb;
//  CRGB target;
//  target.red=random8();
//  target.blue=random8();
//  target.green=random8();
//  return target;
}

void rainbow() {
  int cycles=1000;
  for (int x=0; x < cycles; x++){
    // draw a generic, no-name rainbow
    static uint8_t starthue = 0;
    for(int z=0;z<NUM_LINES;z++)
      fill_rainbow(leds[z], NUM_LEDS, --starthue, 20);
    // Choose which 'color temperature' profile to enable.
    uint8_t secs = (millis() / 1000) % (DISPLAYTIME * 2);
    if( secs < DISPLAYTIME) {
      FastLED.setTemperature( TEMPERATURE_1 ); // first temperature
//      for(int z=0;z<NUM_LINES;z++)
//        leds[z][0] = TEMPERATURE_1; // show indicator pixel
    } else {
      FastLED.setTemperature( TEMPERATURE_2 ); // second temperature
//      for(int z=0;z<NUM_LINES;z++)
//        leds[z][0] = TEMPERATURE_2; // show indicator pixel
    }
    FastLED.show();
    FastLED.delay(8);
  }
  
}

void  chasing_random_colors() {
  FastLED.clear();
  int cycles = 100;
  leds[0][0] = get_random_color();
  leds[1][0] = get_random_color();
  FastLED.show();
  delay(1000);
  for (int cycle; cycle < cycles; cycle++){
    CRGB temp0[NUM_LEDS];
    CRGB temp1[NUM_LEDS];
    for (int x=0; x < NUM_LEDS-1; x++){
      temp0[x+1] = leds[0][x];
      temp1[x+1] = leds[1][x];
//      leds[0][NUM_LEDS-x] = leds[0][NUM_LEDS-x-1];
//      leds[1][NUM_LEDS-x] = leds[1][NUM_LEDS-x-1];
    }
    memcpy(leds[0], temp0, sizeof(CRGB) * NUM_LEDS);
    memcpy(leds[1], temp0, sizeof(CRGB) * NUM_LEDS);
    
    delay(300);
    if (cycle % 3 == 2){
      CRGB new_color = get_random_color();
      leds[0][0] = new_color;
      leds[1][0] = new_color;
    }
    else{
      leds[0][0] = CRGB::Black;
      leds[1][0] = CRGB::Black;   
    }
  FastLED.show();
  }  
}

void  chasing_random_colors_alternating() {
  int cycles = 200;
  for (int x=0;x<NUM_LEDS;x++){
    leds[0][x] = CRGB::Black;
    leds[1][x] = CRGB::Black;
  }
  FastLED.show();
  for (int x=0;x < cycles; x++){
    memcpy(&leds[0][1], &leds[0][0], sizeof(CRGB) * (NUM_LEDS - 1));
    memcpy(&leds[1][1], &leds[1][0], sizeof(CRGB) * (NUM_LEDS - 1));
//  for(int y = NUM_LEDS; y--)
//    for(int y = 1 ; y < NUM_LEDS; y++){
//      leds[0][y] = leds[0][y-1];
//      leds [1][NUM_LEDS - y] = leds[1][NUM_LEDS - (y+1)];
//    }
    if (x % 3 == 0){
      leds[0][0] = get_random_color();
      leds[1][NUM_LEDS] = get_random_color();
    }
    else{
      leds[0][0] = CRGB::Black;
      leds[1][NUM_LEDS] = CRGB::Black;
    }
    FastLED.show();
    delay(50);
  }
}


void all_red(){
  int cycles = 20;
  for (int x = 0; x<cycles; x++){
    for (int y = 0; y < NUM_LEDS; y++){
      leds[0][y]= CRGB::Red;
      leds[1][y]= CRGB::Red;
    }
    FastLED.show();
    delay(50);
  }
  for (int x = 0; x<1; x++){
    for (int y = 0; y < NUM_LEDS; y++){
      leds[0][y]= CRGB::Black;
      leds[1][y]= CRGB::Black;
    }
    FastLED.show();
    delay(50);
  }  
}

