#include "Ticker.h"
#include <FastLED.h>

#define NUM_LEDS 6

#define KNOB_H 0
#define KNOB_S 1
#define KNOB_V 2
#define FUNC_KEY 3
#define STRIP_PIN 0

#define FPS 60


#define MAX_KNOB_VAL 930
#define MIN_KNOB_VAL 230
#define MAX_COLOR_VAL 255

struct hsvData {
  uint8_t h;
  uint8_t s;
  uint8_t v;
};
hsvData prevHsv[NUM_LEDS];
hsvData nextHsv[NUM_LEDS];

bool funcKeyPress;
bool showSelectedLed;

int currentLed = 0;

CRGBArray<NUM_LEDS> leds;

void knobCheck() {
  int hVal = analogRead(KNOB_H);
  int sVal = analogRead(KNOB_S);
  int vVal = analogRead(KNOB_V);
  float hRatio = float(hVal - MIN_KNOB_VAL) / (MAX_KNOB_VAL - MIN_KNOB_VAL);
  float sRatio = float(sVal - MIN_KNOB_VAL) / (MAX_KNOB_VAL - MIN_KNOB_VAL);
  float vRatio = float(vVal - MIN_KNOB_VAL) / (MAX_KNOB_VAL - MIN_KNOB_VAL);
  if (hRatio < 0 ) {
    hRatio = 0;
  }
  if (sRatio < 0 ) {
    sRatio = 0;
  }
  if (vRatio < 0 ) {
    vRatio = 0;
  }
  if (hRatio > 1 ) {
    hRatio = 1;
  }
  if (sRatio > 1 ) {
    sRatio = 1;
  }
  if (vRatio > 1 ) {
    vRatio = 1;
  }
  uint8_t h8bit = uint8_t(hRatio * 255.0);
  uint8_t s8bit = uint8_t(sRatio * 255.0);
  uint8_t v8bit = uint8_t(vRatio * 255.0);

  if (currentLed >= 0 && currentLed < NUM_LEDS) {
    leds[currentLed] = CHSV(h8bit, s8bit, v8bit);
    nextHsv[currentLed].h = h8bit;
    nextHsv[currentLed].s = s8bit;
    nextHsv[currentLed].v = v8bit;
    if (showSelectedLed) {
      FastLED.show();
    }
  }
}

void printChange() {
  for (int i = 0; i < NUM_LEDS; ++i) {
    if (nextHsv[i].h != prevHsv[i].h || nextHsv[i].s != prevHsv[i].s || nextHsv[i].v != prevHsv[i].v) {
      Serial.print("change on led[");
      Serial.print(i);
      Serial.print("], hsv(");
      Serial.print(prevHsv[i].h);
      Serial.print(", ");
      Serial.print(prevHsv[i].s);
      Serial.print(", ");
      Serial.print(prevHsv[i].v);
      Serial.print(") -> hsv(");
      Serial.print(nextHsv[i].h);
      Serial.print(", ");
      Serial.print(nextHsv[i].s);
      Serial.print(", ");
      Serial.print(nextHsv[i].v);
      Serial.print(")\n");
      prevHsv[i].h = nextHsv[i].h;
      prevHsv[i].s = nextHsv[i].s;
      prevHsv[i].v = nextHsv[i].v;
    }
  }
}

void printAllLedColor() {
  Serial.print("\n********** Current LED Colors **********\n");
  for (int i = 0; i < NUM_LEDS; ++i) {
    Serial.print("LED[");
    Serial.print(i);
    Serial.print("] = hsv(");
    Serial.print(nextHsv[i].h);
    Serial.print(", ");
    Serial.print(nextHsv[i].s);
    Serial.print(", ");
    Serial.print(nextHsv[i].v);
    Serial.print(")\n");
  }
  Serial.print("\n\n");
}

void flashLed() {
  if (currentLed >= 0 && currentLed < NUM_LEDS) {
    if (!showSelectedLed) {
      leds[currentLed] = CHSV(nextHsv[currentLed].h, nextHsv[currentLed].s, nextHsv[currentLed].v);
      FastLED.show();
      showSelectedLed = true;
    } else {
      leds[currentLed] = CHSV(0, 0, 0);
      FastLED.show();
      showSelectedLed = false;
    }
  }
}

void showAllLeds() {
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = CHSV(nextHsv[i].h, nextHsv[i].s, nextHsv[i].v);
    FastLED.show();
  }
}

void selectLed() {
  ++currentLed;
  showAllLeds();
  if (currentLed == NUM_LEDS) {
    printAllLedColor();
  } else if (currentLed > NUM_LEDS) {
    currentLed = 0;
  }
}

void funcKeyCheck() {
  int funcKeyData = analogRead(FUNC_KEY);
  float funcKeyVal = float(funcKeyData) / 1024.0;
  if (funcKeyVal > 0.5) {
    if (funcKeyPress) {
      funcKeyPress = false;
    }
  } else {
    if (!funcKeyPress) {
      selectLed();
      funcKeyPress = true;
    }
  }
}

Ticker printChangeTimer(printChange, 4000);
Ticker knobTimer(knobCheck, 1000 / FPS);
Ticker funcKeyTimer(funcKeyCheck, 1000 / FPS);
Ticker flashTimer(flashLed, 500);


void setup() {
  FastLED.addLeds<WS2812B, STRIP_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(115200);
  showSelectedLed = true;
  knobTimer.start();
  printChangeTimer.start();
  flashTimer.start();
  funcKeyTimer.start();
}

void loop() {
  knobTimer.update();
  printChangeTimer.update();
  flashTimer.update();
  funcKeyTimer.update();
}
