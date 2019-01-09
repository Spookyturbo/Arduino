#include <JoystickAnalog.h>
#include <Servo.h>
#include <FastLED.h>

#define UNUSED_ANALOG_PIN A0

#define LED_PIN 11

#define NUM_LEDS    68
#define CHIPSET     WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS 200;
#define directionLed 0
#define BUTTON_PIN 12

CRGB statusLights[4][3];
CRGB leds[NUM_LEDS];

int joyPins[] = {A5, A4, A3, A2};
int joySwitch[] = {6, 7, 8, 9};
int motorPins[] = {2, 3, 4, 5};
CRGB speedColors[] = {CRGB::Red, CRGB::Blue, CRGB::Purple, CRGB::Orange};
float speedTransition[] = {0, 0.25, 0.5, 1.0};
int motorSpeedDivisor[] = {0, 0, 0, 0};

const int joyArraySize = sizeof(joyPins)/sizeof(int);
const int motorArraySize = sizeof(motorPins)/sizeof(int);

JoystickAnalog joy[joyArraySize];
Servo motors[motorArraySize];

JoystickAnalog joy1(A0, A5, 2);

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT);

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.show();
  for(int i = 0; i < joyArraySize; i++) {
    joy[i].init(UNUSED_ANALOG_PIN, joyPins[i], joySwitch[i]);
  }

  for(int i = 0; i < motorArraySize; i++) {
    motors[i].attach(motorPins[i]);
  }
}

void loop() {
  float negative = 1.0;
  Serial.println(digitalRead(BUTTON_PIN));
  for(int i = 0; i < joyArraySize; i++) {
    if(joy[i].getButton()) {
      motorSpeedDivisor[i]++;
      if(motorSpeedDivisor[i] == 4) {
        motorSpeedDivisor[i] = 0;
      }
    }
      statusLights[i][1] = speedColors[motorSpeedDivisor[i]];
      statusLights[i][2] = speedColors[motorSpeedDivisor[i]];
  }
    for(int i = 0; i < joyArraySize; i++) {
      float percent;
      float y = joy[i].getYAxis();
      if((y >= 500 && y <= 520) || !digitalRead(BUTTON_PIN)) {
        percent = 0;
      }
      else {
        y -= 500.0;
        percent = y / 500.0;
        if(percent < 0) {
          negative = -1.0;
        }
        percent *= negative * percent;
        percent *= 100.0;
        percent *= speedTransition[motorSpeedDivisor[i]];
      }
      int PWMValue = percent * 5 + 1500;
      if(PWMValue < 1480) {
        statusLights[i][directionLed] = CRGB::Green; 
      }
      else if(PWMValue > 1520) {
        statusLights[i][directionLed] = CRGB::Red;
      }
      else {
        statusLights[i][directionLed] = CRGB::Yellow;  
      }
      Serial.print("I = "); Serial.print(i); Serial.print(" PWMVALUE: "); Serial.println(PWMValue);
      motors[i].writeMicroseconds(PWMValue);
    }
  updateLights();
} 

void updateLights() {
  int strip = 0;
  int light = 0;
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = statusLights[strip][light];
     
    light++;
    if((i + 1) % 3 == 0) {
      strip++;
      light = 0;
    }
  }
  FastLED.show();
}

