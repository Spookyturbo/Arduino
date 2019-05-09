#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 74

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 4
#define CLOCK_PIN 5

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() { 
   Serial.begin(9600);
      FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, GRB>(leds, NUM_LEDS);
}

void loop() { 
  // Turn the LED on, then pause
  for(int j = 0; j < 3; j++) {
    for(int i = 0; i < NUM_LEDS; i++) {
      int n = (i + j) % NUM_LEDS;
      if(n%3 == 0) {
        leds[i] = CRGB::Purple;
      }
      else {
        leds[i] = CRGB::Black;
      }
    }
    
    FastLED.show();
    delay(200);
  }
}
