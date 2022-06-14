#ifndef CONFIG_STORAGE_H
#define CONFIG_STORAGE_H

#include "LEDPianoConfig.h"

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

  setupKeyAnimation();
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

#endif
