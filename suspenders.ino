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

#define NUM_PATTERNS 3
typedef void (* GenericFP)();
GenericFP patterns[NUM_PATTERNS] = {&usa, &rainbow, 
	&chasing_random_colors_alternating};
#define NUM_LINES 2

	unsigned long frame = 0;
  unsigned long pattern_start_ts = 0;
	unsigned long pattern_duration = 0;
	int current_pattern_index = 0;
	int frame_interval = 150;

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
		unsigned long now = millis();
    unsigned long elapsed = now - pattern_start_ts;
    //check if the pattern time has elapsed - overflow safe!
		if(elapsed > pattern_duration){
      pattern_duration = 4000;//random16(4000,16000)
			pattern_start_ts = now;
			current_pattern_index = (current_pattern_index + 1) % NUM_PATTERNS;
		}
		unsigned long newFrame = now/frame_interval;
		if(newFrame != frame){
			frame = newFrame;
			patterns[current_pattern_index]();
			FastLED.show();			
		}

	}


	CRGB get_random_color() {
		CHSV hsv(random8(), 255, 255);
		CRGB rgb;
		hsv2rgb_rainbow( hsv, rgb);
		return rgb;
	}

	void rainbow() {
		int rainbow_density = 8; //lower==less dense
		increment_all_leds();
		for(int z=0;z<NUM_LINES;z++){
			fill_rainbow(leds[z], 1, frame*rainbow_density);
		}
	}

	void usa(){
		CRGB usa_pattern[9] = {
            CRGB::Red, CRGB::Red, CRGB::Red, 
            CRGB::White, CRGB::White, CRGB::White, 
            CRGB::Blue, CRGB::Blue, CRGB::Blue};
        generic_pattern(usa_pattern, 9);
	}

    void generic_pattern(CRGB * pattern, int pattern_length){
        increment_all_leds();
        int pattern_index = frame % pattern_length;
        for(int i = 0; i < NUM_LINES; i++){
            leds[i][0] = pattern[pattern_index];
        }
    }

	void shift_leds(CRGB * leds_to_shift, int length, int shift_amount){
		CRGB temp[length];
		for (int x=0; x < length-shift_amount; x++){
			temp[x+shift_amount] = leds_to_shift[x];
		}
		memcpy(leds_to_shift, temp, sizeof(CRGB) * length);
	}

	void increment_all_leds(){
		for(int i = 0; i < NUM_LINES; i++){
			shift_leds(leds[i], NUM_LEDS, 1);
		}	
	}

	void  chasing_random_colors_alternating() {
		CRGB new_color = CRGB::Black;	
		if(!leds[0][0] && !leds[0][1]){
			new_color = get_random_color();
		}
		increment_all_leds();		
		for(int z=0;z<NUM_LINES;z++){
			leds[z][0] = new_color;
		}
	}
