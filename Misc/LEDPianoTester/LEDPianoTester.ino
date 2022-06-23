#include "Ticker.h"
#include "MIDIUSB.h"
#include <U8x8lib.h>
// Config for Teensy 2.0

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 12, /* data=*/ 13, /* reset=*/ U8X8_PIN_NONE);

#define TEST_KEY_0 6
#define TEST_KEY_1 7
#define TEST_KEY_2 8
#define TEST_KEY_3 9
#define TEST_KEY_4 10
#define LED_PIN 11

#define START_NOTE 21 // A0
#define STOP_NOTE 108 // C8

#define TRIGGER_LEVEL true
#define IDLE_LEVEL false

uint8_t mode = 0; // 0:mode select, 1:setting, 2:config select, 3:run keys, 4:one key
bool testKeyStatus[5] = {IDLE_LEVEL, IDLE_LEVEL, IDLE_LEVEL, IDLE_LEVEL, IDLE_LEVEL};
bool ledStatus = false;
int ledCounter = 0;

const static uint8_t numChord = 10;
const static uint8_t chordList[numChord] = {0, 4, 7, 10, 12, 16, 19, 22, 24, 28};

uint8_t currentKey = 60; // C4

bool allKeyIdle() {
  for (int i = 0; i < 5; ++i) {
    if (testKeyStatus[i] == TRIGGER_LEVEL) {
      return false;
    }
  }
  return true;
}

void blinkLed(int times) {
  ledCounter = times;
  ledStatus = false;
  digitalWrite(LED_PIN, LOW);
}

void noteOn(uint8_t pitch, uint8_t velocity, uint8_t channel) {
  midiEventPacket_t noteOn = {0x09, channel | 0x90, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(uint8_t pitch, uint8_t velocity, uint8_t channel) {
  midiEventPacket_t noteOff = {0x08, channel | 0x80, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void key0Press() {
  mode = 0;
  blinkLed(1);
  showOnScreen();
}

void key0Release() {
  // Do nothing
}

void key1Press() {
  int randVelocity = random(10, 127);
  switch (mode) {
    case 0:
      mode = 1;
      blinkLed(1);
      showOnScreen();
      break;

    case 1:
      noteOn(21, 100, 1);
      break;

    case 2:
      noteOn(103, 100, 1);
      break;

    case 3:
      if (currentKey - 1 < START_NOTE) {
        currentKey = STOP_NOTE;
      } else {
        currentKey -= 1;
      }
      noteOn(currentKey, randVelocity, 1);
      showCurrentNote();
      break;

    case 4:
      if (currentKey - 1 < START_NOTE) {
        currentKey = STOP_NOTE;
      } else {
        currentKey -= 1;
      }
      showCurrentNote();
      break;

    default: break;
  }
}

void key1Release() {
  switch (mode) {
    case 0:
      break;

    case 1:
      noteOff(21, 0, 1);
      break;

    case 2:
      noteOff(103, 0, 1);
      break;

    case 3:
      noteOff(currentKey, 0, 1);
      break;

    case 4:
      break;

    default: break;
  }
}

void key2Press() {
  int randVelocity = random(10, 127);
  switch (mode) {
    case 0:
      mode = 2;
      blinkLed(2);
      showOnScreen();
      break;

    case 1:
      noteOn(22, 100, 1);
      break;

    case 2:
      noteOn(104, 100, 1);
      break;

    case 3:
    case 4:
      noteOn(currentKey, randVelocity, 1);
      break;

    default: break;
  }
}

void key2Release() {
  switch (mode) {
    case 0:
      break;

    case 1:
      noteOff(22, 0, 1);
      break;

    case 2:
      noteOff(104, 0, 1);
      break;

    case 3:
    case 4:
      noteOff(currentKey, 0, 1);
      break;

    default: break;
  }
}

void key3Press() {
  int randVelocity = random(10, 127);
  switch (mode) {
    case 0:
      mode = 3;
      blinkLed(3);
      showOnScreen();
      break;

    case 1:
      noteOn(23, 100, 1);
      break;

    case 2:
      noteOn(105, 100, 1);
      break;

    case 3:
      if (currentKey + 1 > STOP_NOTE) {
        currentKey = START_NOTE;
      } else {
        currentKey += 1;
      }
      noteOn(currentKey, randVelocity, 1);
      showCurrentNote();
      break;

    case 4:
      if (currentKey + 1 > STOP_NOTE) {
        currentKey = START_NOTE;
      } else {
        currentKey += 1;
      }
      showCurrentNote();
      break;

    default: break;
  }
}

void key3Release() {
  switch (mode) {
    case 0:
      break;

    case 1:
      noteOff(23, 0, 1);
      break;

    case 2:
      noteOff(105, 0, 1);
      break;

    case 3:
      noteOff(currentKey, 0, 1);
      break;

    case 4:
      break;

    default: break;
  }
}

void key4Press() {
  int randVelocity = random(10, 127);
  switch (mode) {
    case 0:
      mode = 4;
      blinkLed(4);
      showOnScreen();
      break;

    case 1:
      noteOn(24, 100, 1);
      break;

    case 2:
      noteOn(108, 100, 1);
      break;

    case 3:
    case 4:
      for (int i = 0; i < numChord; ++i) {
        if (currentKey + chordList[i] < 128) {
          noteOn(currentKey + chordList[i], randVelocity, 1);
        }
      }
      break;

    default: break;
  }
}

void key4Release() {
  switch (mode) {
    case 0:
      break;

    case 1:
      noteOff(24, 0, 1);
      break;

    case 2:
      noteOff(108, 0, 1);
      mode = 3;
      showOnScreen();
      blinkLed(3);
      break;

    case 3:
    case 4:
      for (int i = 0; i < numChord; ++i) {
        if (currentKey + chordList[i] < 128) {
          noteOff(currentKey + chordList[i], 0, 1);
        }
      }
    default: break;
  }
}

void keyPressCheck() {
  for (int k = 0; k < 5; ++k) {
    int testKeyPin = TEST_KEY_0 + k;
    bool val = digitalRead(testKeyPin);
    if (val != testKeyStatus[k]) {
      if (val == IDLE_LEVEL) {
        switch (k) {
          case 0: key0Release(); break;
          case 1: key1Release(); break;
          case 2: key2Release(); break;
          case 3: key3Release(); break;
          case 4: key4Release(); break;
          default: break;
        }
      } else {
        if (allKeyIdle()) {
          switch (k) {
            case 0: key0Press(); break;
            case 1: key1Press(); break;
            case 2: key2Press(); break;
            case 3: key3Press(); break;
            case 4: key4Press(); break;
            default: break;
          }
        }
      }
      testKeyStatus[k] = val;
    }
  }
}

String getNoteName(uint8_t note) {
  uint8_t noteName = note % 12;
  int octave = int(note) / 12 - 1;
  String res = "Key: ";
  switch (noteName) {
    case 0: res += "C"; break;
    case 1: res += "C#/Db"; break;
    case 2: res += "D"; break;
    case 3: res += "D#/Eb"; break;
    case 4: res += "E"; break;
    case 5: res += "F"; break;
    case 6: res += "F#/Gb"; break;
    case 7: res += "G"; break;
    case 8: res += "G#/Ab"; break;
    case 9: res += "A"; break;
    case 10: res += "A#/Bb"; break;
    case 11: res += "B"; break;
    default: res += ""; break;
  }
  res += String(octave) + "     ";
  return res;
}

void showCurrentNote() {
  u8x8.drawString(0, 6, getNoteName(currentKey).c_str());
}

void showOnScreen() {
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.clear();
  switch (mode) {
    case 0:
      u8x8.drawString(0, 0, "Select Mode:");
      u8x8.drawString(0, 2, "1-Setting");
      u8x8.drawString(0, 3, "2-Slot select");
      u8x8.drawString(0, 4, "3-Play note 1");
      u8x8.drawString(0, 5, "4-Play note 2");
      break;

    case 1:
      u8x8.drawString(0, 0, "Setting");
      u8x8.drawString(0, 2, "0-Mode");
      u8x8.drawString(0, 3, "1-Prev Style");
      u8x8.drawString(0, 4, "2-Next Style");
      u8x8.drawString(0, 5, "3-Prev Item");
      u8x8.drawString(0, 6, "4-Next Item");
      break;

    case 2:
      u8x8.drawString(0, 0, "Slot select");
      u8x8.drawString(0, 2, "0-Mode");
      u8x8.drawString(0, 3, "1-Slot 1");
      u8x8.drawString(0, 4, "2-Slot 2");
      u8x8.drawString(0, 5, "3-Slot 3");
      u8x8.drawString(0, 6, "4-Save & Quit");
      break;

    case 3:
    case 4:
      u8x8.drawString(0, 0, "Play Note");
      u8x8.drawString(0, 1, "0-Mode");
      u8x8.drawString(0, 2, "1-Prev Note");
      u8x8.drawString(0, 3, "2-Play Note");
      u8x8.drawString(0, 4, "3-Next Note");
      u8x8.drawString(0, 5, "4-Play Chord");
      showCurrentNote();
      break;

    default: break;
  }
}

void ledCheck() {
  if (ledStatus) {
    ledStatus = false;
    digitalWrite(LED_PIN, LOW);
  } else {
    if (ledCounter > 0) {
      ledStatus = true;
      digitalWrite(LED_PIN, HIGH);
      --ledCounter;
    }
  }
}

Ticker keyTimer(keyPressCheck, 1000 / 144);
Ticker ledTimer(ledCheck, 200);

void setup() {
  pinMode(TEST_KEY_0, INPUT);
  pinMode(TEST_KEY_1, INPUT);
  pinMode(TEST_KEY_2, INPUT);
  pinMode(TEST_KEY_3, INPUT);
  pinMode(TEST_KEY_4, INPUT);
  pinMode(LED_PIN, OUTPUT);
  keyTimer.start();
  ledTimer.start();

  delay(200);
  u8x8.begin();
  u8x8.setPowerSave(0);
  showOnScreen();
}

midiEventPacket_t rx;

void loop() {
  keyTimer.update();
  ledTimer.update();
  do {
    rx = MidiUSB.read();
    if (rx.header != 0) {
      blinkLed(1);
    }
  } while (rx.header != 0);
}
