/* LED Piano V002
   by @Fanseline, 20220622

   Hardware:
     1. Arduino UNO R3 / Leonardo R3
     2. USB Host Shield 2.0:
        Hardware manual: https://chome.nerpa.tech/usb-host-shield-hardware-manual/
        Soldered as: https://chome.nerpa.tech/wp/wp-content/uploads/2011/02/uhs20s_pin_layout.jpg
     3. WS2812B (Total 175 LEDs, 144 LEDs/m, GRB array, 5V power)

   Software Libraries:
     1. Ticker: https://github.com/sstaub/Ticker
     2. FastLED: https://github.com/FastLED/FastLED
     3. USB Host Shield 2.0: https://github.com/felis/USB_Host_Shield_2.0
     4. MIDIUSB (for Leonardo MIDI Loopback) https://github.com/arduino-libraries/MIDIUSB
*/

#ifndef LED_PIANO_CONFIG_H
#define LED_PIANO_CONFIG_H

// #define DEBUG // Print MIDI packet via serial
// #define TEST_STYLE // Test default style
// #define MIDI_LOOPBACK // Leonardo R3 can use this feature

#include "Ticker.h"
#include <FastLED.h>
#include <usbh_midi.h>
#include <usbhub.h>
#include <EEPROM.h>

#ifdef MIDI_LOOPBACK
#include "MIDIUSB.h"
#endif

/* ****************** Static Configurations ****************** */

#define NUM_LEDS 175 // Number of LEDs on your strip, max = 180
#define NUM_KEYS 88

#define STRIP_PIN A0
// #define STRIP_CLOCK A1 // Please check FastLED library

#define START_NOTE 21 // A0, leftmost key on your midi keyboard
#define MIDI_OFFSET 0

#define FPS 60
#define MAX_ALPHA 255

/*
   Brightness limit (power limit)
   Consider external power supply for LED strip.
   If you directly use the on-board 5V pin for LED strip, please set this value
   lower than 0x08 to avoid harmful voltage drop.

   min=0x01, max=0x0F, default=0x08
*/
#define MAX_BRIGHTNESS 0x08

#define CONFIG_SIZE 10 // 10 bytes for each config slot
#define NUM_SAVE_SLOTS 5
#define NUM_SETTING_KEYS 4 // Leftmost 4 keys for setting

const static uint8_t projectTitleLength = 16;
const static char projectTitle[projectTitleLength + 1] = "FanLEDPiano V002"; // Modify title will reset EEPROM!

/*
   MIDI keyboard - LED mapping (LED number starts from left)
   ●: LED mapped to key
   ○: LED not mapped to key

   0  2  4  6  8                                                                  174
   ●○●○●○●○●○●○●○●○●○●○●○●○●   ... ... ●○●○●○●○●○●○●○●○●○●○●○●
   ______________________________________ ... ... __________________________________
   |  | |  |  | | | |  |  | | | | | |  |           |  | | | |  |  | | | | | |  |   |
   |  | |  |  | | | |  |  | | | | | |  |           |  | | | |  |  | | | | | |  |   |
   |  | |  |  | | | |  |  | | | | | |  |           |  | | | |  |  | | | | | |  |   |
   |  |_|  |  |_| |_|  |  |_| |_| |_|  |  ... ...  |  |_| |_|  |  |_| |_| |_|  |   |
   |   |   |   |   |   |   |   |   |   |           |   |   |   |   |   |   |   |   |
   |   |   |   |   |   |   |   |   |   |           |   |   |   |   |   |   |   |   |
   |___|___|___|___|___|___|___|___|___|           |___|___|___|___|___|___|___|___|
*/
const static uint8_t keyLedMap[NUM_KEYS] =
{ 0, 2, 4, // A0 -> B0
  6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, // C1 -> B1
  30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, // C2 -> B2
  54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, // C3 -> B3
  78, 80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100, // C4 -> B4
  102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, // C5 -> B5
  126, 128, 130, 132, 134, 136, 138, 140, 142, 144, 146, 148, // C6 -> B6
  150, 152, 154, 156, 158, 160, 162, 164, 166, 168, 170, 172, // C7 -> B7
  174 // C8
};

/*
      MIDI code vs piano keys (A4 = 440Hz)
   MIDI Code  HEX Code  Piano Key  Note Name  Frequency (Hz)
   108        0x6C      88         C8         4186.01
   107        0x6B      87         B7         3951.07
   106        0x6A      86         A#/Bb7     3729.31
   105        0x69      85         A7         3520.00
   104        0x68      84         G#/Ab7     3322.44
   103        0x67      83         G7         3135.96
                           ... ...
    (Check full table: /Misc/MIDI_Code_vs_Piano_Key.txt)
                           ... ...
   25         0x19      5          C#/Db1     34.65
   24         0x18      4          C1         32.70
   23         0x17      3          B0         30.87
   22         0x16      2          A#/Bb0     29.14
   21         0x15      1          A0         27.50
*/
const static uint8_t keyMidiMap[NUM_KEYS] =
{ 21, 22, 23, // A0 -> B0
  24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, // C1 -> B1
  36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, // C2 -> B2
  48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, // C3 -> B3
  60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, // C4 -> B4
  72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, // C5 -> B5
  84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, // C6 -> B6
  96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, // C7 -> B7
  108 // C8
};

/* Setting Demo LEDs config
   settingLedLeft: show which stytle is under adjusting
   styleNumLed: show current style number using 8 LEDs (display uint8_t number)
   settingLedRight: turn off these LEDs to emphasize config slots

     ○○○○○○○○  ●●●●●●●●  ◑◑◑◑◑◑◑◑◑◑◑ ... ◑◑◑◑◑◑◑◑◑◑◑◑◑◑  ○○○○○○○○○○○○
   | SettingLeft |  StyleNum  |               StyleDemo                   | SettingRight(Slots) |
*/
const static uint8_t settingLedLeftStart = 0;
const static uint8_t settingLedLeftEnd = 7;
const static uint8_t styleNumLedStart = 8;
const static uint8_t styleNumLedEnd = 15;
const static uint8_t settingLedRightStart = 163;
const static uint8_t settingLedRightEnd = 174;

/*
   Setting configuration on MIDI keyboard
   ______________________________________ ... ... __________________________________
   |  | |  |  | | | |  |  | | | | | |  |           |  | | | |  |  | | | | | |  |   |
   |  | |  |  | | | |  |  | | | | | |  |           |  | | | |  |  | | | | | |  |   |
   |  | |  |  | | | |  |  | | | | | |  |           |  | | | |  |  | | | | | |  |   |
   |  |_|  |  |_| |_|  |  |_| |_| |_|  |  ... ...  |  |_| |_|  |  |_| |_| |_|  |   |
   |   |   |   |   |   |   |   |   |   |           |   |   |   |   |   |   |   |   |
   |   |   |   |   |   |   |   |   |   |           |   |   |   |   |   |   |   |   |
   |___|___|___|___|___|___|___|___|___|           |___|___|___|___|___|___|___|___|
     | | |   |                                                       | | | | |   |
     | | |   |__nextSetting(C1)                           slot0(G7)__| | | | |   |
     | | |                                                             | | | |   |
     | | |__prevSetting(B0)                                slot1(G#7)__| | | |   |
     | |                                                                 | | |   |
     | |__nextStyle(A#0)                                      slot2(A7)__| | |   |
     |                                                                     | |   |
     |__prevStyle(A0)                                          slot3(A#7)__| |   |
                                                                             |   |
                                                                  slot4(B7)__|   |
                                                                                 |
                                                           confirm & save(C8)____|
*/
const static uint8_t slotKeys[NUM_SAVE_SLOTS] = {82, 83, 84, 85, 86}; // from G7 to B7, index of keyData[]: slot0 - slot4
const static uint8_t settingKeys[NUM_SETTING_KEYS] = {0, 1, 2, 3}; // from A0 to C1, index of keyData[]: prevStyle, nextStyle, prevSetting, nextSetting
const static uint8_t confirmKey = 87; // C8, index of keyData[]

const static uint8_t bgAnimationNum = 13;
const static uint8_t bgAnimationList[bgAnimationNum] =
{ 0x00, 0x01,
  0x10, 0x11, 0x12, 0x13, 0x14,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25
}; // List for blendFgColors()

const static uint8_t keyAnimationNum = 10;
const static uint8_t keyAnimationList[keyAnimationNum] =
{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // List for updateKeyAnimation()

const static uint8_t bgColorNum = 27;
const static uint8_t bgColorList[bgColorNum] =
{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, // Pure color
  0x80 | 0, 0x80 | 1, 0x80 | 2, 0x80 | 3, 0x80 | 4, 0x80 | 5, 0x80 | 6, 0x80 | 7, // Gradient one cycle
  0xE0 | 0, 0xE0 | 1, 0xE0 | 2, 0xE0 | 3, 0xE0 | 4, 0xE0 | 5, 0xE0 | 6, 0xE0 | 7, // Gradient multi-cycle
}; // List for getColorByCode()

const static uint8_t keyColorNum = 28;
const static uint8_t keyColorList[keyColorNum] =
{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, // Pure color
  0x80 | 0, 0x80 | 1, 0x80 | 2, 0x80 | 3, 0x80 | 4, 0x80 | 5, 0x80 | 6, 0x80 | 7, // Gradient one cycle
  0xE0 | 0, 0xE0 | 1, 0xE0 | 2, 0xE0 | 3, 0xE0 | 4, 0xE0 | 5, 0xE0 | 6, 0xE0 | 7, // Gradient multi-cycle
  0x40 | 0, // random color
}; // List for getColorByCode()

const static uint8_t bgSIdleOffset = 0x04; // Saturation offset
const static uint8_t bgVIdleOffset = 0x01; // Brightness offset
const static uint8_t bgSActivatedOffset = 0x0D;
const static uint8_t bgVActivatedOffset = 0x0A;
const static uint8_t keySOffset = 0x0F;
const static uint8_t keyVOffset = 0x0F;

const static uint8_t defaultConfig[NUM_SAVE_SLOTS][CONFIG_SIZE] =
{
  /* Data order for each row:
    0.bgAnimation
    1.bgColorIdle
    2.bgSVIdle
    3.bgColorActivated
    4.bgSVActivated
    5.keyAnimation
    6.whiteKeyColor
    7.whiteKeySV
    8.blackKeyColor
    9.blackKeySV
  */
  {0x01, 0x87, 0xC2, 0x01, 0xB8, 0x01, 0x07, 0xA8, 0x09, 0xF8},
  {0x01, 0x07, 0x61, 0xE4, 0xB5, 0x03, 0x07, 0x98, 0x07, 0x98},
  {0x12, 0x87, 0x92, 0xE6, 0xF6, 0x00, 0x01, 0xA8, 0x01, 0xA8},
  {0x00, 0xE0, 0xB3, 0xE4, 0xB5, 0x02, 0x05, 0xA8, 0x05, 0xA8},
  {0x23, 0xE0, 0x92, 0xE4, 0xB5, 0x08, 0x40, 0xF8, 0x40, 0xF8},
};


/* ****************** Global Variables ****************** */

/* systemStatus & settingStatus definition
   systemStatus: 0x0?:none, 0x1?:start up, 0x2?:seeking midi, 0x3?:main/setting, 0x4?: usb error
   settingStatus: 0x00:none (idle)
                  0x10:bgAnimation, 0x11:bgColorIdle, 0x12:bgSaturationIdle, 0x13:bgBrightnessIdle
                                    0x14:bgColorActivated, 0x15:bgSaturationActivated, 0x16:bgBrightnessActivated
                  0x20:keyAnimation, 0x21:whiteKeyColor, 0x22:whiteKeySaturation, 0x23:whiteKeyBrightness
                                     0x24:blackKeyColor, 0x25:blackKeySaturation, 0x26:blackKeyBrightness
*/
uint8_t systemStatus = 0x00;
uint8_t settingStatus = 0x00;

uint8_t configNum = 0; // Load from or save to which slot

// Notice: you can test the following style when TEST_STYLE is defined
uint8_t bgAnimation = 0x01;
uint8_t bgColorIdle = 0x01; // Hue or gradient color
uint8_t bgSVIdle = 0x00; // 0x0? - 0xF?: Saturation,  0x?0 - 0x?F: Brightness
uint8_t bgColorActivated = 0x01;
uint8_t bgSVActivated = 0x00;

uint8_t keyAnimation = 0x01;
uint8_t whiteKeyColor = 0x07;
uint8_t whiteKeySV = 0xAD;
uint8_t blackKeyColor = 0x01;
uint8_t blackKeySV = 0xAD;

int16_t frameCount = 0; // Used for background frame counting
int16_t frameCountSetting = 0; // Used for system setting status and error status

float increaseFactor = 0;
float fadeFactorPress = 0.97;
float fadeFactorRelease = 0.3;

CRGBArray<NUM_LEDS> leds;
USB Usb;
USBH_MIDI Midi(&Usb);

struct KeyData {
  uint8_t alpha;
  // 0x80:blackKey, 0x40:refreshing, 0x20:pressing, 0x10:peaked
  // 0x?0 - 0x?F: random color cache
  uint8_t control;
};
KeyData keyData[NUM_KEYS];

#endif
