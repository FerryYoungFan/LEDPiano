#ifndef COLOR_CONTROL_H
#define COLOR_CONTROL_H

#include "KeyControl.h"

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

#endif
