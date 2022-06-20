#ifndef SETTING_CONTROL_H
#define SETTING_CONTROL_H

#include "KeyControl.h"

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
      setupKeyAnimation();
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
      setupKeyAnimation();
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

#endif
