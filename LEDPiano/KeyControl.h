#ifndef KEY_CONTROL_H
#define KEY_CONTROL_H

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

void setupKeyAnimation() {
  const static float increaseNone = 0.0;
  const static float increaseSlow = 0.03;
  const static float increaseFast = 0.97;
  const static float fadeSlow = 0.97;
  const static float fadeMedian = 0.7;
  const static float fadeFast = 0.3;
  const static float fadeNone = 1.0;

  // Notice: Remember to add your new code to keyAnimationList[]
  switch (keyAnimation) {
    case 0: // ↑↘↓ without rendering
      increaseFactor = 0.0;
      fadeFactorPress = fadeSlow;
      fadeFactorRelease = fadeFast;
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

void updateKeyAlpha() {
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

#endif
