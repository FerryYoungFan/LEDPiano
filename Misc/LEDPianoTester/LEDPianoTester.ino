#include "Ticker.h"
// Config for Teensy 2.0

#define TEST_KEY_0 6
#define TEST_KEY_1 7
#define TEST_KEY_2 8
#define TEST_KEY_3 9
#define TEST_KEY_4 10
#define LED_PIN 11

#define START_NOTE 21 // A0
#define STOP_NOTE 108 // C8

uint8_t mode = 0; // 0:mode select, 1:setting, 2:config select, 3:run keys, 4:one key
bool testKeyStatus[5] = {true, true, true, true, true};

uint8_t currentKey = 60; // C4

void keyPressCheck() {
  for (int k = 0; k < 5; ++k) {
    int testKeyPin = TEST_KEY_0 + k;
    bool val = digitalRead(testKeyPin);
    if (val != testKeyStatus[k]) {
      if (val) {
        switch(k){
          case 0: key0Release(); break;
          case 1: key1Release(); break;
          case 2: key2Release(); break;
          case 3: key3Release(); break;
          case 4: key4Release(); break;
          default: break;
        }
        
      } else {
        switch(k){
          case 0: key0Press(); break;
          case 1: key1Press(); break;
          case 2: key2Press(); break;
          case 3: key3Press(); break;
          case 4: key4Press(); break;
          default: break;
        }
      }
      testKeyStatus[k] = val;
    }
  }
}

Ticker keyTimer(keyPressCheck, 1000 / 144);

void blinkLed(int times){
  for (int i = 0; i<times; ++i){
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}

void key0Press(){
  mode = 0;
  blinkLed(1);
}

void key0Release(){
  
}

void key1Press(){
  switch(mode){
    case 0:
    mode = 1;
    blinkLed(1);
    break;

    case 1:
    usbMIDI.sendNoteOn(21, 100, 1);
    break;

    case 2:
    usbMIDI.sendNoteOn(103, 100, 1);
    break;

    case 3:
    if (currentKey - 1 < START_NOTE){
      currentKey = STOP_NOTE;
    } else {
      currentKey -= 1;
    }
    int randVelocity = random(10, 127);
    usbMIDI.sendNoteOn(currentKey, randVelocity, 1);
    break;

    case 4:
    if (currentKey - 1 < START_NOTE){
      currentKey = STOP_NOTE;
    } else {
      currentKey -= 1;
    }
    break;
    
    default: break;
  }
}

void key1Release(){
  switch(mode){
    case 0:
    break;

    case 1:
    usbMIDI.sendNoteOff(21, 0, 1);
    break;

    case 2:
    usbMIDI.sendNoteOff(103, 0, 1);
    break;

    case 3:
    usbMIDI.sendNoteOff(currentKey, 0, 1);
    break;

    case 4:
    break;
    
    default: break;
  }
}

void key2Press(){
  switch(mode){
    case 0:
    mode = 2;
    blinkLed(2);
    break;

    case 1:
    usbMIDI.sendNoteOn(22, 100, 1);
    break;

    case 2:
    usbMIDI.sendNoteOn(104, 100, 1);
    break;

    case 3:
    case 4:
    int randVelocity = random(10, 127);
    usbMIDI.sendNoteOn(currentKey, randVelocity, 1);
    break;
    
    default: break;
  }
}

void key2Release(){
  switch(mode){
    case 0:
    break;

    case 1:
    usbMIDI.sendNoteOff(22, 0, 1);
    break;

    case 2:
    usbMIDI.sendNoteOff(104, 0, 1);
    break;

    case 3:
    case 4:
    usbMIDI.sendNoteOff(currentKey, 0, 1);
    break;
    
    default: break;
  }
}

void key3Press(){
  switch(mode){
    case 0:
    mode = 3;
    blinkLed(3);
    break;

    case 1:
    usbMIDI.sendNoteOn(23, 100, 1);
    break;

    case 2:
    usbMIDI.sendNoteOn(105, 100, 1);
    break;

    case 3:
    if (currentKey + 1 > STOP_NOTE){
      currentKey = START_NOTE;
    } else {
      currentKey += 1;
    }
    int randVelocity = random(10, 127);
    usbMIDI.sendNoteOn(currentKey, randVelocity, 1);
    break;

    case 4:
    if (currentKey + 1 > STOP_NOTE){
      currentKey = START_NOTE;
    } else {
      currentKey += 1;
    }
    break;
    
    default: break;
  }
}

void key3Release(){
  switch(mode){
    case 0:
    break;

    case 1:
    usbMIDI.sendNoteOff(23, 0, 1);
    break;

    case 2:
    usbMIDI.sendNoteOff(105, 0, 1);
    break;

    case 3:
    usbMIDI.sendNoteOff(currentKey, 0, 1);
    break;

    case 4:
    break;
    
    default: break;
  }
}

void key4Press(){
  switch(mode){
    case 0:
    mode = 4;
    blinkLed(4);
    break;

    case 1:
    usbMIDI.sendNoteOn(24, 100, 1);
    break;

    case 2:
    usbMIDI.sendNoteOn(108, 100, 1);
    break;

    case 3:
    case 4:
    int randVelocity = random(10, 127);
    usbMIDI.sendNoteOn(currentKey, randVelocity, 1);
    usbMIDI.sendNoteOn(currentKey + 4, randVelocity, 1);
    usbMIDI.sendNoteOn(currentKey + 7, randVelocity, 1);
    break;
    
    default: break;
  }
}

void key4Release(){
  switch(mode){
    case 0:
    break;

    case 1:
    usbMIDI.sendNoteOff(24, 0, 1);
    break;

    case 2:
    usbMIDI.sendNoteOff(108, 0, 1);
    mode = 3;
    blinkLed(3);
    break;

    case 3:
    case 4:
    int randVelocity = random(10, 127);
    usbMIDI.sendNoteOff(currentKey, 0, 1);
    usbMIDI.sendNoteOff(currentKey + 4, 0, 1);
    usbMIDI.sendNoteOff(currentKey + 7, 0, 1);
    
    default: break;
  }
}


void setup() {
  pinMode(TEST_KEY_1, INPUT);
  pinMode(TEST_KEY_2, INPUT);
  pinMode(TEST_KEY_3, INPUT);
  pinMode(TEST_KEY_4, INPUT);
  pinMode(LED_PIN, OUTPUT);
  keyTimer.start();
}

void loop() {
  keyTimer.update(); 
}
