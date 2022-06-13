#include "LEDPianoConfig.h"

void initKeys() {
  for (int i = 0; i < NUM_KEYS; ++i) {
    uint8_t currentMidiCode = keyMidiMap[i];
    keyData[i].alpha = 0;
    uint8_t noteNameNum = currentMidiCode % 12;
    uint8_t controlCode = 0x01; // initial color cache is white
    switch (noteNameNum) {
      case 1:
      case 3:
      case 6:
      case 8:
      case 10:
        controlCode |= 0x80; // is black key
        break;
      default: break;
    }
    keyData[i].control = controlCode;
  }
}

CRGB getRainbowColor(int hueCount, int huePeriod, uint8_t sat, uint8_t bri) {
  float hueRatio = float(hueCount % huePeriod) / float(huePeriod);
  uint8_t hue = uint8_t(hueRatio * 255.0 + 0.5);
  return CHSV(hue, sat, bri);
}

CRGB getGradientColor(int hueCount, int huePeriod, uint8_t startHue, uint8_t stopHue, uint8_t sat, uint8_t bri) {
  float hueRatio = float(hueCount % huePeriod) / float(huePeriod);
  if (hueRatio <= 0.5) {
    hueRatio = hueRatio / 0.5;
  } else {
    hueRatio = (1.0 - hueRatio) / 0.5;
  }
  uint8_t hue = uint8_t(float(startHue) + hueRatio * (stopHue - startHue) + 0.5);
  return CHSV(hue, sat, bri);
}

CRGB getColorByCode(uint8_t colorCode, int hueCount, int huePeriod, uint8_t sat, uint8_t bri) {
  // Notice: Remember to add your new color code to bgColorList[] and keyColorList[]
  const static uint8_t red = 0;
  const static uint8_t orange = 17;
  const static uint8_t yellow = 38;
  const static uint8_t yellowGreen = 66;
  const static uint8_t green = 92;
  const static uint8_t cyan = 120;
  const static uint8_t blue = 154;
  const static uint8_t purple = 176;
  const static uint8_t magenta = 224;

  bool isGradient = (colorCode & 0x80) != 0;
  if (isGradient) {
    /*
       Gradient color struct
       1       1 1           1   1 1 1 1
           periodScalar       colorCode (subcode)
       colorCode: from 0x00 to 0x1F (32 colors)
    */
    uint8_t periodScalar = (colorCode & 0x60) >> 5; // totally 4 scalars
    hueCount *= (periodScalar + 1);
    uint8_t subcode = colorCode & 0x1F;
    switch (subcode) {
      case 0: return getRainbowColor(hueCount, huePeriod, sat, bri);
      case 1: return getGradientColor(hueCount, huePeriod, red, yellow, sat, bri);
      case 2: return getGradientColor(hueCount, huePeriod, yellow, green, sat, bri);
      case 3: return getGradientColor(hueCount, huePeriod, green, blue, sat, bri);
      case 4: return getGradientColor(hueCount, huePeriod, blue, magenta, sat, bri);
      case 5: return getGradientColor(hueCount, huePeriod, red, green, sat, bri);
      case 6: return getGradientColor(hueCount, huePeriod, yellow, blue, sat, bri);
      case 7: return getGradientColor(hueCount, huePeriod, green, magenta, sat, bri);
      default: return getRainbowColor(hueCount, huePeriod, sat, bri);
    }
  } else {
    /*
       Pure color struct
       0   1 1 1  1 1 1 1
          colorCode (subcode)
       colorCode: from 0x00 to 0x7F (Max 128 colors)
    */
    uint8_t subcode = colorCode & 0x7F;
    switch (subcode) {
      case 0: return CHSV(0, 0, 0); // turn off
      case 1: return CHSV(0, 0, bri); // white / gray
      case 2: return CHSV(red, sat, bri);
      case 3: return CHSV(orange, sat, bri);
      case 4: return CHSV(yellow, sat, bri);
      case 5: return CHSV(yellowGreen, sat, bri);
      case 6: return CHSV(green, sat, bri);
      case 7: return CHSV(cyan, sat, bri);
      case 8: return CHSV(blue, sat, bri);
      case 9: return CHSV(purple, sat, bri);
      case 10: return CHSV(magenta, sat, bri);
      default: return CHSV(0, 0, 0); // turn off
    }
  }

}

float getPowerRatio() {
  const static float fullPower = 3.0;
  const static float randomRatio = 0.2;
  float currentPower = 0.0;
  for (int i = 0; i < NUM_KEYS; ++i) {
    if (keyData[i].control & 0x40) { // refreshing
      currentPower += float(keyData[i].alpha) / MAX_ALPHA;
    }
  }
  float res = currentPower / 3.0;
  res *= randomRatio * (float(random(0, 256)) - 127.0) / 256.0 + (1.0 - randomRatio);
  if (res > 1.0) {
    return 1.0;
  }
  if (res < 0.0) {
    return 0.0;
  }
  return res;
}

void blendBgColors() {
  uint8_t idleSaturation = (bgSVIdle & 0xF0) | bgSIdleOffset;
  uint8_t idleBrightness = ((bgSVIdle & 0x0F) << 4) | bgVIdleOffset;
  uint8_t activatedSaturation = (bgSVActivated & 0xF0) | bgSActivatedOffset;
  uint8_t activatedBrightness = ((bgSVActivated & 0x0F) << 4) | bgVActivatedOffset;

  float powerRatio = getPowerRatio();
  uint8_t activatedLedNum = uint8_t(powerRatio * NUM_LEDS + 0.5);

  const static uint8_t leftLedNum = NUM_LEDS / 2;
  const static uint8_t rightLedNum = NUM_LEDS - NUM_LEDS / 2;
  uint8_t leftActivatedNum = uint8_t(powerRatio * leftLedNum + 0.5);
  uint8_t rightActivatedNum = uint8_t(powerRatio * rightLedNum + 0.5);

  const static int timeScalar = 5;

  int huePeriod = NUM_LEDS;
  int hueCount = 0;

  // Notice: Remember to add your new code to bgAnimationList[]
  switch (bgAnimation) { // set hue period
    case 0x20: // dynamic rainbow left to right
    case 0x22: // dynamic rainbow rigth to left
      if (++frameCount >= huePeriod) {
        frameCount = 0;
      }
      huePeriod = NUM_LEDS;
      break;
    case 0x21: // dynamic rainbow left to right (slow)
    case 0x23: // dynamic rainbow rigth to left (slow)
      huePeriod = NUM_LEDS;
      if (++frameCount >= huePeriod * timeScalar) {
        frameCount = 0;
      }
      break;

    case 0x24: // dynamic rainbow breath
      huePeriod = 255;
      if (++frameCount >= huePeriod) {
        frameCount = 0;
      }
      break;
    case 0x25: // dynamic rainbow breath (slow)
      huePeriod = 255 * timeScalar;
      if (++frameCount >= huePeriod) {
        frameCount = 0;
      }
      break;

    default:
      break;
  }

  if (bgAnimation == 0x14) { // change all brightness
    idleBrightness = uint8_t(float(activatedBrightness - idleBrightness) * powerRatio + 0.5 + float(idleBrightness));
  }

  for (int j = 0; j < NUM_LEDS; ++j) {
    bool activated = false;
    switch (bgAnimation) {
      case 0x01: // no animation
        hueCount = j + frameCount;
        break;

      case 0x10: // jump from left
        activated = j < activatedLedNum;
        hueCount = j + frameCount;
        break;

      case 0x11: // jump from right
        activated = j > NUM_LEDS - 1 - activatedLedNum;
        hueCount = j + frameCount;
        break;

      case 0x12: // jump from both sides
        activated = j < leftActivatedNum || j > (NUM_LEDS - rightActivatedNum);
        hueCount = j + frameCount;
        break;

      case 0x13: // jump from middle
        activated = j > NUM_LEDS / 2 - leftActivatedNum && j < NUM_LEDS / 2 + rightActivatedNum;
        hueCount = j + frameCount;
        break;

      case 0x14: // change all brightness
        hueCount = j + frameCount;
        break;

      case 0x20: // dynamic rainbow left to right
        hueCount = j + huePeriod - frameCount;
        break;
      case 0x21: // dynamic rainbow left to right (slow)
        hueCount = j + (huePeriod - frameCount) / timeScalar;
        break;

      case 0x22: // dynamic rainbow rigth to left
        hueCount = j + frameCount;
        break;
      case 0x23: // dynamic rainbow rigth to left (slow)
        hueCount = j + frameCount / timeScalar;
        break;

      case 0x24:
      case 0x25: // dynamic rainbow breath
        hueCount = frameCount;
        break;

      default: // turn off
        idleBrightness = 0;
        break;
    }

    if (activated) {
      leds[j] = getColorByCode(bgColorActivated, hueCount, huePeriod, activatedSaturation, activatedBrightness);
    } else {
      leds[j] = getColorByCode(bgColorIdle, hueCount, huePeriod, idleSaturation, idleBrightness);
    }

  }
}

CRGB getKeyColor(KeyData& currentKey, int hueCount, uint8_t midiNum) {
  uint8_t note = midiNum % 12;
  uint8_t keyColor;
  uint8_t keySV;
  bool isBlackKey = (currentKey.control & 0x80) != 0;
  if (isBlackKey) {
    keyColor = blackKeyColor;
    keySV = blackKeySV;
  } else {
    keyColor = whiteKeyColor;
    keySV = whiteKeySV;
  }

  uint8_t keySaturation = (keySV & 0xF0) | keySOffset;
  uint8_t keyBrightness = ((keySV & 0x0F) << 4) | keyVOffset;
  int huePeriod = NUM_KEYS;
  bool randomColor = (keyColor == 0x40);

  if (randomColor) {
    keyColor = (currentKey.control & 0x0F); // use random color cached in control data
  }

  bool isGradient = (keyColor & 0x80) != 0;
  if (isGradient) { // Gradient color
    uint8_t periodScalar = (keyColor & 0x60) >> 5; // totally 4 scalars
    keyColor &= ~0x60; // remove period info
    switch (periodScalar) {
      case 1: huePeriod = 36; hueCount = note; break; // three octaves
      case 2: huePeriod = 24; hueCount = note; break; // two octaves
      case 3: huePeriod = 12; hueCount = note; break; // one octaves
      default: break;
    }
    return getColorByCode(keyColor, hueCount, huePeriod, keySaturation, keyBrightness);
  } else {
    return getColorByCode(keyColor, hueCount, huePeriod, keySaturation, keyBrightness);
  }
}

void blendFgColors() {
  // Combine foreground and background color together
  if (keyAnimation == 0x00) {
    return; // turn off key animation
  }
  for (int i = 0; i < NUM_KEYS; ++i) {
    if (keyData[i].alpha > 0) {
      uint8_t ledNum = keyLedMap[i];
      CRGB currentBgColor = leds[ledNum];
      CRGB currentFgColor = getKeyColor(keyData[i], i, keyMidiMap[i]);
      float fgAlpha = float(keyData[i].alpha) / MAX_ALPHA;
      float newColorR = float(currentBgColor.r) * (1.0 - fgAlpha) + float(currentFgColor.r) * fgAlpha + 0.5;
      float newColorG = float(currentBgColor.g) * (1.0 - fgAlpha) + float(currentFgColor.g) * fgAlpha + 0.5;
      float newColorB = float(currentBgColor.b) * (1.0 - fgAlpha) + float(currentFgColor.b) * fgAlpha + 0.5;
      leds[ledNum] = CRGB(uint8_t(newColorR), uint8_t(newColorG), uint8_t(newColorB));
    }
  }
}

void updateKeyAnimation() {
  const static float increaseNone = 0.0;
  const static float increaseSlow = 0.03;
  const static float increaseFast = 0.97;
  const static float fadeSlow = 0.95;
  const static float fadeMedian = 0.7;
  const static float fadeFast = 0.3;
  const static float fadeNone = 1.0;

  // Notice: Remember to add your new code to keyAnimationList[]
  switch (keyAnimation) {
    case 0: // No key press effect
      increaseFactor = 0.0;
      fadeFactorPress = 0.0;
      fadeFactorRelease = 0.0;
      break;

    case 1: // ↑↘↓
      increaseFactor = 0.0;
      fadeFactorPress = fadeSlow;
      fadeFactorRelease = fadeFast;
      break;

    case 2: // ↑→↓
      increaseFactor = 0.0;
      fadeFactorPress = fadeNone;
      fadeFactorRelease = fadeFast;
      break;

    case 3: // ↑→↘
      increaseFactor = 0.0;
      fadeFactorPress = fadeNone;
      fadeFactorRelease = fadeSlow;
      break;

    case 4: // ↑↘↘
      increaseFactor = 0.0;
      fadeFactorPress = fadeSlow;
      fadeFactorRelease = fadeMedian;
      break;

    case 5: // ↗→↘
      increaseFactor = increaseSlow;
      fadeFactorPress = fadeNone;
      fadeFactorRelease = fadeMedian;
      break;

    case 6: // ↗↘↘
      increaseFactor = increaseSlow;
      fadeFactorPress = fadeSlow;
      fadeFactorRelease = fadeMedian;
      break;

    case 7: // ↑→↓ (no velocity)
      increaseFactor = increaseFast;
      fadeFactorPress = fadeNone;
      fadeFactorRelease = fadeFast;
      break;

    case 8: // ↑↘↓ (no velocity)
      increaseFactor = increaseFast;
      fadeFactorPress = fadeSlow;
      fadeFactorRelease = fadeFast;
      break;

    case 9: // ↑↓↓ (fast flash)
      increaseFactor = increaseFast;
      fadeFactorPress = fadeFast;
      fadeFactorRelease = fadeFast;
      break;

    default: break;
  }
}

void updateFgColors() {
  for (int i = 0; i < NUM_KEYS; ++i) {
    bool refreshing = ((keyData[i].control & 0x40) != 0);
    if (refreshing) {
      bool pressing = ((keyData[i].control & 0x20) != 0);
      bool peaked = ((keyData[i].control & 0x10) != 0);
      if (pressing) { // pressing?
        if (peaked) {
          keyData[i].alpha = uint8_t(float(keyData[i].alpha) * fadeFactorPress);
        } else {
          float nextAlpha = (255.0 - float(keyData[i].alpha)) * increaseFactor + float(keyData[i].alpha) + 0.5;
          if (nextAlpha > MAX_ALPHA) {
            nextAlpha = MAX_ALPHA;
          }
          if (nextAlpha - float(keyData[i].alpha) < 1.0) {
            keyData[i].control |= 0x10; // peaked = true
          }
          keyData[i].alpha = uint8_t(nextAlpha);
        }
      } else {
        keyData[i].alpha = uint8_t(float(keyData[i].alpha) * fadeFactorRelease);
      }
      if (keyData[i].alpha == 0) {
        keyData[i].control &= ~0x40; // refreshing = false
      }
    }
  }
}

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
  const static uint8_t defaultH = 38;
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

uint8_t getNextListData(uint8_t list[], uint8_t listStart, uint8_t listEnd, uint8_t currentData) {
  int settingIndex = -1;
  for (int i = listStart; i < listEnd; ++i) {
    if (list[i] == currentData) {
      settingIndex = i; // get current index
      break;
    }
  }
  if (settingIndex == listEnd - 1 || settingIndex == -1) {
    return list[listStart];
  }
  return list[settingIndex + 1];
}

uint8_t getNextSaturation(uint8_t codeSV) {
  uint8_t sat = codeSV >> 4;
  uint8_t bri = codeSV & 0x0F;
  if (sat >= 0x0F) {
    return bri;
  } else {
    return ((sat + 1) << 4) | bri;
  }
}

uint8_t getNextBrightness(uint8_t codeSV) {
  uint8_t sat = codeSV & 0xF0;
  uint8_t bri = codeSV & 0x0F;
  if (bri >= MAX_BRIGHTNESS) {
    return sat;
  } else {
    return sat | (bri + 1);
  }
}

void nextStyle() {
  switch (settingStatus) {
    case 0x10: // bgAnimation
      bgAnimation = getNextListData(bgAnimationList, 0, bgAnimationNum, bgAnimation);
      frameCount = 0;
      break;

    case 0x11: // bgColorIdle
      bgColorIdle = getNextListData(bgColorList, 0, bgColorNum, bgColorIdle);
      break;

    case 0x12: // bgSaturationIdle
      bgSVIdle = getNextSaturation(bgSVIdle);
      break;

    case 0x13: // bgBrightnessIdle
      bgSVIdle = getNextBrightness(bgSVIdle);
      break;

    case 0x14: // bgColorActivated
      bgColorActivated = getNextListData(bgColorList, 0, bgColorNum, bgColorActivated);
      break;

    case 0x15: // bgSaturationActivated
      bgSVActivated = getNextSaturation(bgSVActivated);
      break;

    case 0x16: // bgBrightnessActivated
      bgSVActivated = getNextBrightness(bgSVActivated);
      break;

    case 0x20: // keyAnimation
      keyAnimation = getNextListData(keyAnimationList, 0, keyAnimationNum, keyAnimation);
      break;

    case 0x21: // whiteKeyColor
      whiteKeyColor = getNextListData(keyColorList, 0, keyColorNum, whiteKeyColor);
      break;

    case 0x22: // whiteKeySaturation
      whiteKeySV = getNextSaturation(whiteKeySV);
      break;

    case 0x23: // whiteKeyBrightness
      whiteKeySV = getNextBrightness(whiteKeySV);
      break;

    case 0x24: // blackKeyColor
      blackKeyColor = getNextListData(keyColorList, 0, keyColorNum, blackKeyColor);
      break;

    case 0x25: // blackKeySaturation
      blackKeySV = getNextSaturation(blackKeySV);
      break;

    case 0x26: // blackKeyBrightness
      blackKeySV = getNextBrightness(blackKeySV);
      break;

    default: break;
  }
}

uint8_t getPrevListData(uint8_t list[], uint8_t listStart, uint8_t listEnd, uint8_t currentData) {
  int settingIndex = -1;
  for (int i = listStart; i < listEnd; ++i) {
    if (list[i] == currentData) {
      settingIndex = i; // get current index
      break;
    }
  }
  if (settingIndex == listStart || settingIndex == -1) {
    return list[listEnd - 1];
  }
  return list[settingIndex - 1];
}

uint8_t getPrevSaturation(uint8_t codeSV) {
  uint8_t sat = codeSV >> 4;
  uint8_t bri = codeSV & 0x0F;
  if (sat == 0x00) {
    return bri | 0xF0;
  } else {
    return ((sat - 1) << 4) | bri;
  }
}

uint8_t getPrevBrightness(uint8_t codeSV) {
  uint8_t sat = codeSV & 0xF0;
  uint8_t bri = codeSV & 0x0F;
  if (bri == 0x00) {
    return sat | MAX_BRIGHTNESS;
  } else {
    return sat | (bri - 1);
  }
}

void prevStyle() {
  switch (settingStatus) {
    case 0x10: // bgAnimation
      bgAnimation = getPrevListData(bgAnimationList, 0, bgAnimationNum, bgAnimation);
      frameCount = 0;
      break;

    case 0x11: // bgColorIdle
      bgColorIdle = getPrevListData(bgColorList, 0, bgColorNum, bgColorIdle);
      break;

    case 0x12: // bgSaturationIdle
      bgSVIdle = getPrevSaturation(bgSVIdle);
      break;

    case 0x13: // bgBrightnessIdle
      bgSVIdle = getPrevBrightness(bgSVIdle);
      break;

    case 0x14: // bgColorActivated
      bgColorActivated = getPrevListData(bgColorList, 0, bgColorNum, bgColorActivated);
      break;

    case 0x15: // bgSaturationActivated
      bgSVActivated = getPrevSaturation(bgSVActivated);
      break;

    case 0x16: // bgBrightnessActivated
      bgSVActivated = getPrevBrightness(bgSVActivated);
      break;

    case 0x20: // keyAnimation
      keyAnimation = getPrevListData(keyAnimationList, 0, keyAnimationNum, keyAnimation);
      break;

    case 0x21: // whiteKeyColor
      whiteKeyColor = getPrevListData(keyColorList, 0, keyColorNum, whiteKeyColor);
      break;

    case 0x22: // whiteKeySaturation
      whiteKeySV = getPrevSaturation(whiteKeySV);
      break;

    case 0x23: // whiteKeyBrightness
      whiteKeySV = getPrevBrightness(whiteKeySV);
      break;

    case 0x24: // blackKeyColor
      blackKeyColor = getPrevListData(keyColorList, 0, keyColorNum, blackKeyColor);
      break;

    case 0x25: // blackKeySaturation
      blackKeySV = getPrevSaturation(blackKeySV);
      break;

    case 0x26: // blackKeyBrightness
      blackKeySV = getPrevBrightness(blackKeySV);
      break;

    default: break;
  }
}

void nextSetting() {
  frameCountSetting = 0;
  switch (settingStatus) {
    case 0x10: // bgAnimation
      if (bgAnimation == 0x00) { // bg turned off
        settingStatus = 0x20;
      } else {
        settingStatus = 0x11;
      }
      break;

    case 0x11: // bgColorIdle
      if (bgColorIdle == 0x00) { // turned off
        if (bgAnimation >= 0x10 && bgAnimation < 0x20) { // Has activated effect
          settingStatus = 0x14;
        } else {
          settingStatus = 0x20;
        }
      } else if (bgColorIdle == 0x01) { // white color
        settingStatus = 0x13;
      } else {
        settingStatus = 0x12;
      }
      break;

    case 0x12: // bgSaturationIdle
      settingStatus = 0x13;
      break;

    case 0x13: // bgBrightnessIdle
      if (bgAnimation >= 0x10 && bgAnimation < 0x20) { // Has activated effect
        settingStatus = 0x14;
      } else {
        settingStatus = 0x20;
      }
      break;

    case 0x14: // bgColorActivated
      if (bgColorActivated == 0x00) { // turned off
        settingStatus = 0x20;
      } else if (bgColorActivated == 0x01) { // white color
        settingStatus = 0x16;
      } else {
        settingStatus = 0x15;
      }
      break;

    case 0x15: // bgSaturationActivated
      settingStatus = 0x16;
      break;

    case 0x16: // bgBrightnessActivated
      settingStatus = 0x20;
      break;

    case 0x20: // keyAnimation
      if (keyAnimation == 0x00) { // key turned off
        settingStatus = 0x10;
      } else {
        settingStatus = 0x21;
      }
      break;

    case 0x21: // whiteKeyColor
      if (whiteKeyColor == 0x00) { // turned off
        settingStatus = 0x24;
      } else if (whiteKeyColor == 0x01) { // white color
        settingStatus = 0x23;
      } else {
        settingStatus = 0x22;
      }
      break;

    case 0x22: // whiteKeySaturation
      settingStatus = 0x23;
      break;

    case 0x23: // whiteKeyBrightness
      settingStatus = 0x24;
      break;

    case 0x24: // blackKeyColor
      if (blackKeyColor == 0x00) { // turned off
        settingStatus = 0x10;
      } else if (blackKeyColor == 0x01) { // white color
        settingStatus = 0x26;
      } else {
        settingStatus = 0x25;
      }
      break;

    case 0x25: // blackKeySaturation
      settingStatus = 0x26;
      break;

    case 0x26: // blackKeyBrightness
      settingStatus = 0x10;
      break;

    default: break;
  }
}

void prevSetting() {
  frameCountSetting = 0;
  switch (settingStatus) {
    case 0x10: // bgAnimation
      if (keyAnimation == 0x00) { // key turned off
        settingStatus = 0x20;
      } else {
        if (blackKeyColor == 0x00) { // turned off {
          settingStatus = 0x24;
        } else {
          settingStatus = 0x26;
        }
      }
      break;

    case 0x11: // bgColorIdle
      settingStatus = 0x10;
      break;

    case 0x12: // bgSaturationIdle
      settingStatus = 0x11;
      break;

    case 0x13: // bgBrightnessIdle
      if (bgColorIdle == 0x01) { // white color
        settingStatus = 0x11;
      } else {
        settingStatus = 0x12;
      }
      break;

    case 0x14: // bgColorActivated
      if (bgColorIdle == 0x00) { // turned off
        settingStatus = 0x11;
      } else {
        settingStatus = 0x13;
      }
      break;

    case 0x15: // bgSaturationActivated
      settingStatus = 0x14;
      break;

    case 0x16: // bgBrightnessActivated
      if (bgColorActivated == 0x01) { // white color
        settingStatus = 0x14;
      } else {
        settingStatus = 0x15;
      }
      break;

    case 0x20: // keyAnimation
      if (bgAnimation == 0x00) { // turned off
        settingStatus = 0x10;
      } else if (bgAnimation >= 0x10 && bgAnimation < 0x20) { // Has activated effect
        if (bgColorActivated == 0x00) { // turned off
          settingStatus = 0x14;
        } else {
          settingStatus = 0x16;
        }
      } else {
        if (bgColorIdle == 0x00) { // turned off
          settingStatus = 0x11;
        } else {
          settingStatus = 0x13;
        }
      }
      break;

    case 0x21: // whiteKeyColor
      settingStatus = 0x20;
      break;

    case 0x22: // whiteKeySaturation
      settingStatus = 0x21;
      break;

    case 0x23: // whiteKeyBrightness
      if (whiteKeyColor == 0x01) { // white color
        settingStatus = 0x21;
      } else {
        settingStatus = 0x22;
      }
      break;

    case 0x24: // blackKeyColor
      if (whiteKeyColor == 0x00) { // turned off
        settingStatus = 0x21;
      } else {
        settingStatus = 0x23;
      }
      break;

    case 0x25: // blackKeySaturation
      settingStatus = 0x24;
      break;

    case 0x26: // blackKeyBrightness
      if (blackKeyColor == 0x01) { // white color
        settingStatus = 0x24;
      } else {
        settingStatus = 0x25;
      }
      break;

    default: break;
  }
}

void updateLeds() {
  blendBgColors();
  blendFgColors();
  updateFgColors();
  if (settingStatus) {
    showConfigAll();
  }
  FastLED.show();
}

void initSaveSlots() {
  int eepromPointer = 0;
  for (int i = 0; i < projectTitleLength; ++i) { // write project title (as identicator)
    EEPROM.update(eepromPointer++, projectTitle[i]);
  }
  configNum = 0;
  EEPROM.update(eepromPointer++, configNum);
  for (int i = 0; i < NUM_SAVE_SLOTS; ++i) {
    EEPROM.update(eepromPointer++, defaultConfig[i][0]); // bgAnimation
    EEPROM.update(eepromPointer++, defaultConfig[i][1]); // bgColorIdle
    EEPROM.update(eepromPointer++, defaultConfig[i][2]); // bgSVIdle
    EEPROM.update(eepromPointer++, defaultConfig[i][3]); // bgColorActivated
    EEPROM.update(eepromPointer++, defaultConfig[i][4]); // bgSVActivated

    EEPROM.update(eepromPointer++, defaultConfig[i][5]); // keyAnimation
    EEPROM.update(eepromPointer++, defaultConfig[i][6]); // whiteKeyColor
    EEPROM.update(eepromPointer++, defaultConfig[i][7]); // whiteKeySV
    EEPROM.update(eepromPointer++, defaultConfig[i][8]); // blackKeyColor
    EEPROM.update(eepromPointer++, defaultConfig[i][9]); // blackKeySV
  }
}

bool isSaveValid() {
  // read project title (as identicator)
  for (int i = 0; i < projectTitleLength; ++i) {
    if (EEPROM.read(i) != projectTitle[i]) {
      return false;
    }
  }
  return true;
}

uint8_t readConfigNum() {
  int eepromPointer = projectTitleLength;
  uint8_t rawData = EEPROM.read(eepromPointer);
  return (rawData < NUM_SAVE_SLOTS) ? rawData : 0;
}

void saveConfigNum(uint8_t _configNum) {
  int eepromPointer = projectTitleLength;
  EEPROM.update(eepromPointer, _configNum < NUM_SAVE_SLOTS ? _configNum : 0);
}

bool checkDataInList(uint8_t list[], uint8_t listLen, int& readPointer, uint8_t& writeBack) {
  uint8_t rawData = EEPROM.read(readPointer++);
#ifdef DEBUG
  Serial.print("In List: ");
  Serial.println(rawData, HEX);
#endif
  for (int i = 0; i < listLen; ++i) {
    if (list[i] == rawData) {
      writeBack = rawData;
      return true;
    }
  }
#ifdef DEBUG
  Serial.println("^not in list");
#endif
  writeBack = list[0];
  return false;
}

bool readSVEEPROM(int& readPointer, uint8_t& writeBack) {
  uint8_t rawData = EEPROM.read(readPointer++);
#ifdef DEBUG
  Serial.print("ReadSV: ");
  Serial.println(rawData, HEX);
#endif
  if ((rawData & 0x0F) > MAX_BRIGHTNESS) { // Brigthness limit check
    rawData = (rawData & 0xF0) | MAX_BRIGHTNESS;
    writeBack = rawData;
#ifdef DEBUG
    Serial.println("^brigthness limit error");
#endif
    return false;
  }
  writeBack = rawData;
  return true;
}

bool loadSetting(uint8_t _configNum) {
  int eepromPointer = projectTitleLength + 1 + _configNum * CONFIG_SIZE;
  bool noError = true;

  noError &= checkDataInList(bgAnimationList, bgAnimationNum, eepromPointer, bgAnimation);
  noError &= checkDataInList(bgColorList, bgColorNum, eepromPointer, bgColorIdle);
  noError &= readSVEEPROM(eepromPointer, bgSVIdle);
  noError &= checkDataInList(bgColorList, bgColorNum, eepromPointer, bgColorActivated);
  noError &= readSVEEPROM(eepromPointer, bgSVActivated);

  noError &= checkDataInList(keyAnimationList, keyAnimationNum, eepromPointer, keyAnimation);
  noError &= checkDataInList(keyColorList, keyColorNum, eepromPointer, whiteKeyColor);
  noError &= readSVEEPROM(eepromPointer, whiteKeySV);
  noError &= checkDataInList(keyColorList, keyColorNum, eepromPointer, blackKeyColor);
  noError &= readSVEEPROM(eepromPointer, blackKeySV);

  updateKeyAnimation();
  return noError;
}

void checkSavedConfig() {
  if (!isSaveValid()) {
#ifdef DEBUG
    Serial.println("init EEPROM");
#endif
    initSaveSlots();
  }
  configNum = readConfigNum();
  loadSetting(configNum);
}

void saveCurrentConfig(uint8_t _configNum) {
  int eepromPointer = projectTitleLength + 1 + _configNum * CONFIG_SIZE;
  EEPROM.update(eepromPointer++, bgAnimation);
  EEPROM.update(eepromPointer++, bgColorIdle);
  EEPROM.update(eepromPointer++, bgSVIdle);
  EEPROM.update(eepromPointer++, bgColorActivated);
  EEPROM.update(eepromPointer++, bgSVActivated);

  EEPROM.update(eepromPointer++, keyAnimation);
  EEPROM.update(eepromPointer++, whiteKeyColor);
  EEPROM.update(eepromPointer++, whiteKeySV);
  EEPROM.update(eepromPointer++, blackKeyColor);
  EEPROM.update(eepromPointer++, blackKeySV);
}

void switchToConfig(uint8_t _configNum) {
  if (_configNum != configNum) {
    saveCurrentConfig(configNum);
    configNum = _configNum;
    loadSetting(configNum);
  }
}

inline void activateKey(KeyData& currentKey, uint8_t velocity) {
  currentKey.control |= 0x60; // pressing = true; refreshing = true

  bool isBlackKey = (currentKey.control & 0x80) != 0;
  bool randomColor = isBlackKey ? (blackKeyColor == 0x40) : (whiteKeyColor == 0x40);
  if (randomColor) {
    currentKey.control = (currentKey.control & 0xF0) | uint8_t(random(1, 10 + 1));
  }

  if (increaseFactor <= 0) {
    currentKey.control |= 0x10; // peaked = true;
    currentKey.alpha = (velocity << 1) | 1;
  } else {
    currentKey.control &= ~0x10; // peaked = false;
    currentKey.alpha = 0;
  }
}

inline void deactivateKey(KeyData& currentKey) {
  currentKey.control &= ~0x20; // pressing = false
  currentKey.control |= 0x10; // peaked = true;
}

void midiInputCheck() {
  uint8_t outBuf[4];
  uint8_t size;

  do {
    if ( (size = Midi.RecvRawData(outBuf)) > 0 ) {

#ifdef DEBUG
      Serial.print("MIDI[");
      Serial.print(size);
      Serial.print("] = [ ");
      for (int i = 0; i <= size; ++i) { // size is the midi message size!
        Serial.print("0x");
        Serial.print(outBuf[i], HEX);
        Serial.print(" ");
      }
      Serial.println("]");
#endif

#ifdef MIDI_LOOPBACK
      midiEventPacket_t event = {outBuf[0], outBuf[1], outBuf[2], outBuf[3]};
      MidiUSB.sendMIDI(event);
      MidiUSB.flush();
#endif

      uint8_t statusCode = outBuf[1] & 0xF0;
      if (statusCode == 0x80 || statusCode == 0x90) {
        uint8_t pitch = outBuf[2] + MIDI_OFFSET;
        uint8_t velocity = outBuf[3];
        if (statusCode == 0x80 || velocity == 0) { // 0x80 note off
          for (int i = 0; i < NUM_KEYS; ++i) {
            if (keyMidiMap[i] == pitch) {
              deactivateKey(keyData[i]);
              break;
            }
          }
        } else { // 0x90 note on
          for (int i = 0; i < NUM_KEYS; ++i) {
            if (keyMidiMap[i] == pitch) {
              activateKey(keyData[i], velocity);
              if (settingStatus) {
                switch (i) {
                  case 0: prevStyle(); break; // A0
                  case 1: nextStyle(); break; // A#0
                  case 2: prevSetting(); break; // B0
                  case 3: nextSetting(); break; // C1

                  case 82: switchToConfig(0); break; // g7
                  case 83: switchToConfig(1); break; // G#7
                  case 84: switchToConfig(2); break; // A7
                  case 85: switchToConfig(3); break; // A#7
                  case 86: switchToConfig(4); break; // B7

                  case 87: // C8
                    saveCurrentConfig(configNum);
                    saveConfigNum(configNum);
                    settingStatus = 0x00;
                    break;

                  default: break;
                }
              }
              break;
            }
          }
        }
      }

    }
  } while (size > 0);
}

void showError() {
  if (frameCountSetting == 0) {
    if ((systemStatus & 0xF0) == 0x20) { // seeking midi
      for (int j = 0; j < NUM_LEDS; ++j) {
        leds[j] = CRGB(0x01, 0x01, 0x00); // Yellow for midi warning
      }
    } else {
      for (int j = 0; j < NUM_LEDS; ++j) {
        leds[j] = CRGB(0x02, 0x00, 0x00); // Red for error
      }
    }
    frameCountSetting = 1;
  } else {
    for (int j = 0; j < NUM_LEDS; ++j) {
      leds[j] = CRGB(0x00, 0x00, 0x00);
    }
    frameCountSetting = 0;
  }
  FastLED.show();
}

Ticker ledTimer(updateLeds, 1000 / FPS);
Ticker errorFlashTimer(showError, 500);

void midiCheckLoop() {
  Usb.Task();
  uint8_t codeHeader = systemStatus & 0xF0;

  if (Midi) {
    midiInputCheck();
    if (codeHeader != 0x30) { // previously not main or setting status
      systemStatus = (systemStatus & 0x0F) | 0x30;
      ledTimer.start();
      errorFlashTimer.stop();

#ifdef DEBUG
      Serial.println("MIDI connected");
#endif
    }

  } else { // !Midi
    if (codeHeader == 0x30) { // previously main or setting status
      systemStatus = (systemStatus & 0x0F) | 0x20; // seeking midi
      ledTimer.stop();
      errorFlashTimer.start();

#ifdef DEBUG
      Serial.println("MIDI disconnected");
#endif
    }
  }
}

void setup() {
  systemStatus = 0x10; // system start up
  FastLED.addLeds<WS2812B, STRIP_PIN, GRB>(leds, NUM_LEDS); // Remider: here RGB order is "GRB" for WS2812B
  initKeys();

#ifdef DEBUG
  Serial.begin(115200);
#endif

  errorFlashTimer.start();
  if (Usb.Init() == -1) {
    systemStatus = 0x40; // usb error
    while (1) {
      errorFlashTimer.update();
    }
  }

  systemStatus = 0x20; // seeking midi

#ifndef TEST_STYLE
  settingStatus = 0x10;
  checkSavedConfig();
#else
  settingStatus = 0x00;
#endif
}

void loop() {
  uint8_t codeHeader = systemStatus & 0xF0;
  switch (codeHeader) {
    case 0x30: // main or setting
      midiCheckLoop();
      ledTimer.update();
      break;
    case 0x20: // seeking midi
      midiCheckLoop();
      errorFlashTimer.update();
    default:
      break;
  }
}
