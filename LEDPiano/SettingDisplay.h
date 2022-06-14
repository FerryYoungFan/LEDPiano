#ifndef SETTING_DISPLAY_H
#define SETTING_DISPLAY_H

#include "LEDPianoConfig.h"

void showStyleNum(uint8_t styleNum) {
  const static CHSV ledOn = CHSV(154, 200, 100); // blue
  const static CHSV ledOff = CHSV(154, 200, 5);
  for (int i = styleNumLedStart; i <= styleNumLedEnd; ++i) {
    bool digit = bool((styleNum >> (i - styleNumLedStart)) & 0x01);
    leds[styleNumLedEnd + styleNumLedStart - i] = digit ? ledOn : ledOff;
  }
}

void showSetting() {
  bool blinkOn = (frameCountSetting % FPS) >= (FPS / 4);
  const static uint8_t defaultH = 0x00; // red
  const static uint8_t defaultH2 = 0x64; // green
  const static uint8_t defaultS = 0xD0;
  const static uint8_t defaultV = 0x80;
  const static uint8_t numSettingLeds = settingLedLeftEnd - settingLedLeftStart;
  float colorRatio = float(frameCountSetting < FPS ? frameCountSetting : (2 * FPS - frameCountSetting)) / FPS;
  uint8_t dynamicColor = uint8_t(colorRatio * 255.0);
  uint8_t selectLed = uint8_t(float(numSettingLeds) * colorRatio + settingLedLeftStart + 0.5);
  switch (settingStatus) {
    case 0x10: // bgAnimation
      for (int i = settingLedLeftStart; i <= settingLedLeftEnd; ++i) {
        leds[i] = blinkOn ? CHSV(defaultH, defaultS, defaultV) : CHSV(0, 0, 0);
      }
      showStyleNum(bgAnimation);
      break;

    case 0x11: // bgColorIdle
      for (int i = settingLedLeftStart; i <= settingLedLeftEnd; ++i) {
        leds[i] = blinkOn ? CHSV(dynamicColor, defaultS, defaultV) : CHSV(0, 0, 0);
      }
      showStyleNum(bgColorIdle);
      break;

    case 0x12: // bgSaturationIdle
      for (int i = settingLedLeftStart; i <= settingLedLeftEnd; ++i) {
        leds[i] = blinkOn ? CHSV(defaultH, dynamicColor, defaultV) : CHSV(0, 0, 0);
      }
      showStyleNum(bgSVIdle >> 4);
      break;

    case 0x13: // bgBrightnessIdle
      for (int i = settingLedLeftStart; i <= settingLedLeftEnd; ++i) {
        leds[i] = blinkOn ? CHSV(defaultH, defaultS, dynamicColor) : CHSV(0, 0, 0);
      }
      showStyleNum(bgSVIdle & 0x0F);
      break;

    case 0x14: // bgColorActivated
      for (int i = settingLedLeftStart; i <= settingLedLeftEnd; ++i) {
        leds[i] = (i <= selectLed) ? CHSV(dynamicColor, defaultS, defaultV) : CHSV(0, 0, 0);
      }
      showStyleNum(bgColorActivated);
      break;

    case 0x15: // bgSaturationActivated
      for (int i = settingLedLeftStart; i <= settingLedLeftEnd; ++i) {
        leds[i] = (i <= selectLed) ? CHSV(defaultH, dynamicColor, defaultV) : CHSV(0, 0, 0);
      }
      showStyleNum(bgSVActivated >> 4);
      break;

    case 0x16: // bgBrightnessActivated
      for (int i = settingLedLeftStart; i <= settingLedLeftEnd; ++i) {
        leds[i] = (i <= selectLed) ? CHSV(defaultH, defaultS, dynamicColor) : CHSV(0, 0, 0);
      }
      showStyleNum(bgSVActivated & 0x0F);
      break;

    case 0x20: // keyAnimation
      for (int i = settingLedLeftStart; i <= settingLedLeftEnd; ++i) {
        leds[i] = CHSV(0, 0, 0);
      }
      for (int i = 0; i < NUM_SETTING_KEYS; ++i) {
        leds[keyLedMap[settingKeys[i]]] = blinkOn ? CHSV(defaultH2, defaultS, defaultV) : CHSV(0, 0, 0);
      }
      showStyleNum(keyAnimation);
      break;

    case 0x21: // whiteKeyColor
      for (int i = settingLedLeftStart; i <= settingLedLeftEnd; ++i) {
        leds[i] = CHSV(0, 0, 0);
      }
      for (int i = 0; i < NUM_SETTING_KEYS; ++i) {
        if (keyData[settingKeys[i]].control & 0x80) { // is black key
          continue;
        }
        leds[keyLedMap[settingKeys[i]]] = blinkOn ? CHSV(dynamicColor, defaultS, defaultV) : CHSV(0, 0, 0);
      }
      showStyleNum(whiteKeyColor);
      break;

    case 0x22: // whiteKeySaturation
      for (int i = settingLedLeftStart; i <= settingLedLeftEnd; ++i) {
        leds[i] = CHSV(0, 0, 0);
      }
      for (int i = 0; i < NUM_SETTING_KEYS; ++i) {
        if (keyData[settingKeys[i]].control & 0x80) { // is black key
          continue;
        }
        leds[keyLedMap[settingKeys[i]]] = blinkOn ? CHSV(defaultH2, dynamicColor, defaultV) : CHSV(0, 0, 0);
      }
      showStyleNum(whiteKeySV >> 4);
      break;

    case 0x23: // whiteKeyBrightness
      for (int i = settingLedLeftStart; i <= settingLedLeftEnd; ++i) {
        leds[i] = CHSV(0, 0, 0);
      }
      for (int i = 0; i < NUM_SETTING_KEYS; ++i) {
        if (keyData[settingKeys[i]].control & 0x80) { // is black key
          continue;
        }
        leds[keyLedMap[settingKeys[i]]] = blinkOn ? CHSV(defaultH2, defaultS, dynamicColor) : CHSV(0, 0, 0);
      }
      showStyleNum(whiteKeySV & 0x0F);
      break;

    case 0x24: // blackKeyColor
      for (int i = settingLedLeftStart; i <= settingLedLeftEnd; ++i) {
        leds[i] = CHSV(0, 0, 0);
      }
      for (int i = 0; i < NUM_SETTING_KEYS; ++i) {
        if (keyData[settingKeys[i]].control & 0x80) { // is black key
          leds[keyLedMap[settingKeys[i]]] = blinkOn ? CHSV(dynamicColor, defaultS, defaultV) : CHSV(0, 0, 0);
        }
      }
      showStyleNum(blackKeyColor);
      break;

    case 0x25: // blackKeySaturation
      for (int i = settingLedLeftStart; i <= settingLedLeftEnd; ++i) {
        leds[i] = CHSV(0, 0, 0);
      }
      for (int i = 0; i < NUM_SETTING_KEYS; ++i) {
        if (keyData[settingKeys[i]].control & 0x80) { // is black key
          leds[keyLedMap[settingKeys[i]]] = blinkOn ? CHSV(defaultH2, dynamicColor, defaultV) : CHSV(0, 0, 0);
        }
      }
      showStyleNum(blackKeySV >> 4);
      break;

    case 0x26: // blackKeyBrightness
      for (int i = settingLedLeftStart; i <= settingLedLeftEnd; ++i) {
        leds[i] = CHSV(0, 0, 0);
      }
      for (int i = 0; i < NUM_SETTING_KEYS; ++i) {
        if (keyData[settingKeys[i]].control & 0x80) { // is black key
          leds[keyLedMap[settingKeys[i]]] = blinkOn ? CHSV(defaultH2, defaultS, dynamicColor) : CHSV(0, 0, 0);
        }
      }
      showStyleNum(blackKeySV & 0x0F);
      break;

    default: break;
  }
}

void showConfigNum() {
  bool blinkOn = (frameCountSetting % FPS) >= (FPS / 2);
  const static uint8_t defaultH = 0x26; // yellow
  const static uint8_t defaultH2 = 0x64; // green
  const static uint8_t defaultS = 0xD0;
  const static uint8_t defaultV = 0x20;
  const static uint8_t defaultV2 = 0x80;
  for (int i = settingLedRightStart; i <= settingLedRightEnd; ++i) {
    leds[i] = CHSV(0, 0, 0);
  }
  for (int i = 0; i < NUM_SAVE_SLOTS; ++i) {
    uint8_t tempBrightness;
    if (i == configNum) {
      tempBrightness = blinkOn ? defaultV2 : 0;
    } else {
      tempBrightness = defaultV;
    }
    leds[keyLedMap[slotKeys[i]]] = CHSV(defaultH, defaultS, tempBrightness);
  }
  leds[keyLedMap[confirmKey]] = CHSV(defaultH2, defaultS, blinkOn ? defaultV : 0); // confirm key
}

void showConfigKeyPress() {
  const static CHSV ledOn = CHSV(0, 0, 0x90);
  for (int i = 0; i < NUM_SAVE_SLOTS; ++i) {
    if (keyData[slotKeys[i]].control & 0x20) { // pressing
      leds[keyLedMap[slotKeys[i]]] = ledOn;
    }
  }
  for (int i = 0; i < NUM_SETTING_KEYS; ++i) {
    if (keyData[settingKeys[i]].control & 0x20) { // pressing
      leds[keyLedMap[settingKeys[i]]] = ledOn;
    }
  }
  if (keyData[confirmKey].control & 0x20) { // pressing
    leds[keyLedMap[confirmKey]] = ledOn;
  }
}

void showConfigAll() {
  showSetting();
  showConfigNum();
  showConfigKeyPress();
  if (++frameCountSetting >= 2 * FPS) {
    frameCountSetting = 0;
  }
}

#endif
