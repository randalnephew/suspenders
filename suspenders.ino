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

	unsigned long lastFrame = 0;
	int pattern_length = 0;
	unsigned long pattern_start_time = 0;
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
		int cur_time = millis();
		if(cur_time > (pattern_length + pattern_start_time)){
			pattern_start_time = cur_time;
			pattern_length = 4000;//random16(4000,16000);
			current_pattern_index = (current_pattern_index + 1) % NUM_PATTERNS;
		}
		unsigned long frame = cur_time/frame_interval;
		if(frame > lastFrame || frame == 0){
			lastFrame = frame;
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

	void rainbow(int frameInterval) {
		int rainbow_density = 8; //lower==less dense
		increment_all_leds();
		for(int z=0;z<NUM_LINES;z++){
			fill_rainbow(leds[z], 1, lastFrame*rainbow_density);
		}
	}

	void usa(){
		increment_all_leds();
		int strip_length = 3;
		int usa_index = lastFrame % (3*strip_length);
		int usa_color_index = usa_index / strip_length;
		CRGB usa_colors[3] = {CRGB::Red, CRGB::White, CRGB::Blue};
		for(int z=0;z<NUM_LINES;z++){
			leds[z][0] = usa_colors[usa_color_index];
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
