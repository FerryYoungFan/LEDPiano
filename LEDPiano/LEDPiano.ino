/*
   You can modify most of the configurations here: LEDPianoConfig.h
*/

#include "ColorControl.h"
#include "SettingDisplay.h"
#include "SettingControl.h"
#include "ConfigStorage.h"

void updateLeds() {
  blendBgColors();
  blendFgColors();
  updateKeyAlpha();
  if (settingStatus) {
    showConfigAll();
  }
  FastLED.show();
}

void activateKey(KeyData& currentKey, uint8_t velocity) {
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

void deactivateKey(KeyData& currentKey) {
  currentKey.control &= ~0x20; // pressing = false
  currentKey.control |= 0x10; // peaked = true;
}

void settingControl(uint8_t keyIndex) {
  if (keyIndex == settingKeys[0]) {
    prevStyle();
  } else if (keyIndex == settingKeys[1]) {
    nextStyle();
  } else if (keyIndex == settingKeys[2]) {
    prevSetting();
  } else if (keyIndex == settingKeys[3]) {
    nextSetting();
  } else if (keyIndex == confirmKey) {
    saveCurrentConfig(configNum);
    saveConfigNum(configNum);
    settingStatus = 0x00;
  } else {
    for (int i = 0; i < NUM_SAVE_SLOTS; ++i) {
      if (keyIndex == slotKeys[i]) {
        switchToConfig(i);
        break;
      }
    }
  }
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
      for (int i = 0; i <= size; ++i) { // MIDI message size, not buffer size
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
          for (uint8_t i = 0; i < NUM_KEYS; ++i) {
            if (keyMidiMap[i] == pitch) {
              deactivateKey(keyData[i]);
              break;
            }
          }
        } else { // 0x90 note on
          for (uint8_t i = 0; i < NUM_KEYS; ++i) {
            if (keyMidiMap[i] == pitch) {
              activateKey(keyData[i], velocity);
              if (settingStatus) {
                settingControl(i);
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
