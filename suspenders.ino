#include <FastLED.h>
#include <Bounce2.h>

// How many leds in your strip?
#define NUM_LEDS 47

#define BRIGHTNESS_DEFAULT 100

#define CHIPSET WS2812B
#define COLOR_ORDER GRB

#define DATA_PIN01 11
#define DATA_PIN02 12

#define NUM_PATTERNS 7
typedef void (* GenericFP)();
GenericFP patterns[NUM_PATTERNS] = {&usa, &rainbow,
                                    &larson_scanner_wipe,
                                    &random_complementary,
                                    &chasing_random_colors_alternating,
                                    &random_complementary_bars,
                                    &chasing_random_colors
                                   };
#define NUM_LINES 2

unsigned long frame = 0;
unsigned long pattern_start_ts = 0;
unsigned long pattern_duration = 0;
unsigned long now = millis();
unsigned long elapsed = now - pattern_start_ts;
int current_pattern_index = 0;
int frame_interval = 100;
int last_button_state = HIGH;

CRGB global_random_color = CRGB::Black;

CRGB leds[NUM_LINES][NUM_LEDS];

#define brightness_pin A0
#define auto_toggle_pin 4
#define next_pattern_pin 5

Bounce next_pattern_pin_debouncer = Bounce();

void setup() {
  FastLED.addLeds<CHIPSET, DATA_PIN01, COLOR_ORDER>(leds[0], NUM_LEDS);
  FastLED.addLeds<CHIPSET, DATA_PIN02, COLOR_ORDER>(leds[1], NUM_LEDS);
  FastLED.clear();
  FastLED.setBrightness(BRIGHTNESS_DEFAULT);
  FastLED.show();
  randomSeed(analogRead(A1));
  random16_add_entropy(random());



  pinMode(next_pattern_pin, INPUT_PULLUP);
  pinMode(auto_toggle_pin, INPUT_PULLUP);

  // After setting up the button, setup the Bounce instance :
  next_pattern_pin_debouncer.attach(next_pattern_pin);
  next_pattern_pin_debouncer.interval(20); // interval in ms 
}

void loop() {
  now = millis();
  update_pattern(now);
  update_brightness();

  unsigned long newFrame = now / frame_interval;
  if (newFrame != frame) {
    frame = newFrame;
    patterns[current_pattern_index]();
    FastLED.show();
  }
}

void update_brightness(){
  //should return a number between 0 - 1023 on Nano
  int brightness_reading = analogRead(brightness_pin);
  brightness_reading = brightness_reading >> 7; // divide by 128
  brightness_reading = brightness_reading << 5; // multiply by 32
  FastLED.setBrightness(brightness_reading);
}

void update_pattern(unsigned long now){
  next_pattern_pin_debouncer.update();
  int next_pattern_pin_state = next_pattern_pin_debouncer.read();
  int auto_toggle = digitalRead(auto_toggle_pin);

  elapsed = now - pattern_start_ts;

  //check if the pattern time has elapsed - overflow safe!
  if ((auto_toggle == HIGH && elapsed > pattern_duration) || next_pattern_pin_state == LOW && last_button_state != LOW) {
    pattern_start_ts = now;
    pattern_duration = random16(30000,60000);
    current_pattern_index = (current_pattern_index + 1) % NUM_PATTERNS;
    global_random_color = get_random_color();
  }
  last_button_state = next_pattern_pin_state;  
}

CRGB get_random_color() {

  CHSV hsv(random(256), 255, 255);
  CRGB rgb;
  hsv2rgb_rainbow( hsv, rgb);
  return rgb;
}

void rainbow() {
  int rainbow_density = 8; //lower==less dense
  shift_all_leds(1);
  for (int z = 0; z < NUM_LINES; z++) {
    fill_rainbow(leds[z], 1, frame * rainbow_density);
  }
}

void usa() {
  CRGB usa_pattern[9] = {
    CRGB::Red, CRGB::Red, CRGB::Red,
    CRGB::White, CRGB::White, CRGB::White,
    CRGB::Blue, CRGB::Blue, CRGB::Blue
  };
  generic_pattern(usa_pattern, 9);
}

void generic_pattern(CRGB * pattern, int pattern_length) {
  shift_all_leds(1);
  int pattern_index = frame % pattern_length;
  for (int i = 0; i < NUM_LINES; i++) {
    leds[i][0] = pattern[pattern_index];
  }
}

void shift_leds(CRGB * leds_to_shift, int length, int shift_amount) {
  CRGB temp[length];
  for (int x = 0; x < length - shift_amount; x++) {
    temp[x + shift_amount] = leds_to_shift[x];
  }
  memcpy(leds_to_shift, temp, sizeof(CRGB) * length);
}

void shift_all_leds(int shift_amount) {
  for (int i = 0; i < NUM_LINES; i++) {
    shift_leds(leds[i], NUM_LEDS, shift_amount);
  }
}

void chasing_random_colors() {
  chasing_random_colors(false);
}

void chasing_random_colors_alternating() {
  chasing_random_colors(true);
}

void chasing_random_colors(bool alternating) {
  CRGB new_color = CRGB::Black;
  if (!leds[0][0] && !leds[0][1]) {
    new_color = get_random_color();
  }

  for (int i = 0; i < NUM_LINES; i++) {
    if (alternating and (i % 2 == 1)) {
      shift_leds(leds[i], NUM_LEDS, -1);
      leds[i][NUM_LEDS - 1] = new_color; 
    } else {
      shift_leds(leds[i], NUM_LEDS, 1);
      leds[i][0] = new_color;
    }
  }
}

CRGB get_complementary_color(CRGB color){
  CHSV hsv_color = rgb2hsv_approximate(color);
  hsv_color.hue = hsv_color.hue - 128;
  CRGB rgb_complement;
  hsv2rgb_rainbow( hsv_color, rgb_complement);
  return rgb_complement;
}

void larson_scanner_wipe(){
  frame_interval = 30;
  unsigned int pattern_frame = elapsed / frame_interval;
  // 0 == forwards, 1 = backwards
  int direction = (pattern_frame / NUM_LEDS) % 2;
  for(int i = 0; i < NUM_LINES; i++){
    if(direction == 0){
      shift_leds(leds[i], NUM_LEDS, 1);
      leds[i][0] = global_random_color;    
    } else {
      shift_leds(leds[i], NUM_LEDS, -1);
      leds[i][NUM_LEDS - 1] = get_complementary_color(global_random_color);     
    }
  }
}

void random_complementary(){
  frame_interval = 100;
  complementary_color_bars(get_random_color(), 5);
}

void random_complementary_bars(){
  complementary_color_bars(global_random_color, 5);
}

void complementary_color_bars(CRGB color, unsigned int length){
  CRGB complementary_color = get_complementary_color(color);
  shift_all_leds(1);

  for(int i = 0; i < NUM_LINES; i++){
    if(frame % (length*2) < length){      
      leds[i][0] = color;
    } else {
      leds[i][0] = complementary_color;
    }
  }
}