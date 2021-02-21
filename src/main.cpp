#include <Arduino.h>

#include <FastLED.h>

#define LED_COUNT 30
#define COLOR_ORDER BRG
#define LED_TYPE WS2811
#define LED_PIN 8
#define KNOB_PIN A3
#define BTN_PIN 3
#define BTN_LED_PIN 9
#define STEP_COUNT 1000

#define BLUE_STATE 0
#define RED_STATE 1
#define GREEN_STATE 2
#define RUN_STATE 3
#define NIGHT_STATE 4
#define POWER_STATE 5
#define RAINBOW_STATE 6
#define WATER_STATE 7
#define COLOR_WAVE_STATE 8

#define TOTAL_STATE 9

CRGBArray<LED_COUNT> leds;

int knobVal = 0;
int bgt = 0;
int curentState = COLOR_WAVE_STATE;
long step = 0;
long curentStep = 0;
bool isStepChange = false;
int index = 0;
int hue = 0;
int gHue = 0;
bool isFadeUp = true;

void setBrightness() {
  knobVal = analogRead(KNOB_PIN);
  FastLED.setBrightness(bgt);
}

void setColorAll(CRGB color) {
  for (size_t i = 0; i < LED_COUNT; i++) {
      leds[i] = color;
  }
}

void setLeds() {
  switch (curentState) {
  case BLUE_STATE:
    setColorAll(CRGB(0, 255, 255));
    break;

  case RED_STATE:
    setColorAll(CRGB(255, 0, 0));
    break;

  case GREEN_STATE:
    setColorAll(CRGB(0, 255, 0));
    break;

  case RUN_STATE:

    if (isStepChange && curentStep % 20 == 0) {
      if (index > LED_COUNT) {
        index = 0;
      } else {
        index++;
      }
    }
    
    for (size_t i = 0; i < LED_COUNT; i++) {
      if (index == i) {
        leds[i] = CRGB(0, 255, 0);
        if (i > 1) {
          leds[i - 2] = CRGB(255, 0, 0);
        }

        // if (i > 5) {
        //   leds[i - 6] = CRGB(200, 110, 20);
        // }
      } else {
        if (i <= (LED_COUNT - 1) / 2) {
          leds[i] = CRGB(101, 67, 33);
        } else {
          leds[i] = CRGB(0, 200, 255);
        }
      }
    }
    
    break;

  case NIGHT_STATE:
    setColorAll(CRGB(175, 0, 100));

    for (size_t i = 0; i < LED_COUNT; i++) {
        if (i >= LED_COUNT / 2) {
          leds[i] = CRGB(25, 0, 25);
        } else {
          leds[i] = CRGB(250, 0, 175);
        }
      }

    if( random16() < 120) {
      leds[ random16(LED_COUNT / 2, LED_COUNT) ] += CRGB::Yellow;
    }

    break;

  case POWER_STATE:
    if (isStepChange && step % 60 == 0) {
      for (size_t i = 0; i < LED_COUNT; i++) {
        leds[i] = CHSV(random16(0, 255), 255, random16(100, 255));
      }
    }
      
    break;  

    case RAINBOW_STATE:

      if (isStepChange) {
        if (gHue > 255) {
          gHue = 0;
        }
        gHue++;
      }

      fill_rainbow(leds, LED_COUNT, gHue, LED_PIN);
    break;

  case WATER_STATE:
    if (isStepChange && step % 50 == 0) {
      for (size_t i = 0; i < LED_COUNT; i++) {
        leds[i] = CHSV(random16(120, 145), 255, random16(150, 255));
      }
    }
    
    break;

  case COLOR_WAVE_STATE:
    if (isStepChange && step % 20 == 0) {
      hue++;
      if (hue > 255) {
          hue = 0;  
      }
      for (size_t i = 0; i < LED_COUNT; i++) {
        leds[i] = CHSV(hue, 255, 255);
      }
    }
    break;
  
  default:

    break;
  }

  setBrightness();
  FastLED.show();
  
}

void setBtn() {
  analogWrite(BTN_LED_PIN, bgt);

  if (digitalRead(BTN_PIN) == LOW) {

    if (curentState >= TOTAL_STATE - 1) {
      curentState = 0;
    } else {
      curentState++;
    }

    analogWrite(BTN_LED_PIN, bgt);
    delay(100);
    analogWrite(BTN_LED_PIN, 0);
    delay(100);
    analogWrite(BTN_LED_PIN, bgt);
    delay(100);
    analogWrite(BTN_LED_PIN, 0);
    delay(100);
  }
  
}

void setup() {
  Serial.begin(9600);

  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(BTN_LED_PIN, OUTPUT);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, LED_COUNT).setCorrection(TypicalLEDStrip);
}

void loop() {
  bgt = map(knobVal, 0, 1023, 255, 5);
  setLeds();
  setBtn();

  EVERY_N_MILLISECONDS(10) { 
    if (step > STEP_COUNT) {
      step = 0;
    } else {
      step++;
    }
  }

  isStepChange = curentStep != step;
  curentStep = step;
}