/* LED Piano V001
   by @Fanseline, 20220604

   Hardware:
     1. Arduino UNO R3
     2. USB Host Shield 2.0:
        Hardware manual: https://chome.nerpa.tech/usb-host-shield-hardware-manual/
        Soldered as: https://chome.nerpa.tech/wp/wp-content/uploads/2011/02/uhs20s_pin_layout.jpg
     3. WS2812B (Total 175 LEDs, 144 LEDs/m, GRB array, 5V power)

   Software Libraries:
     1. Ticker: https://github.com/sstaub/Ticker
     2. FastLED: https://github.com/FastLED/FastLED
     3. USB Host Shield 2.0: https://github.com/felis/USB_Host_Shield_2.0
     4. MIDIVisualizer (make piano roll video): https://github.com/kosua20/MIDIVisualizer
*/

/* MIDI code vs piano keys (A4 = 440Hz)
   MIDI Code  HEX Code  Piano Key  Note Name  Frequency (Hz)
   127        0x7F      --         G9         12543.85
   126        0x7E      --         F#/Gb9     11839.82
   125        0x7D      --         F9         11175.30
   124        0x7C      --         E9         10548.08
   123        0x7B      --         D#/Eb9     9956.06
   122        0x7A      --         D9         9397.27
   121        0x79      --         C#/Db9     8869.84
   120        0x78      --         C9         8372.02
   119        0x77      --         B8         7902.13
   118        0x76      --         A#/Bb8     7458.62
   117        0x75      --         A8         7040.00
   116        0x74      --         G#/Ab8     6644.88
   115        0x73      --         G8         6271.93
   114        0x72      --         F#/Gb8     5919.91
   113        0x71      --         F8         5587.65
   112        0x70      --         E8         5274.04
   111        0x6F      --         D#/Eb8     4978.03
   110        0x6E      --         D8         4698.64
   109        0x6D      --         C#/Db8     4434.92
   108        0x6C      88         C8         4186.01
   107        0x6B      87         B7         3951.07
   106        0x6A      86         A#/Bb7     3729.31
   105        0x69      85         A7         3520.00
   104        0x68      84         G#/Ab7     3322.44
   103        0x67      83         G7         3135.96
   102        0x66      82         F#/Gb7     2959.96
   101        0x65      81         F7         2793.83
   100        0x64      80         E7         2637.02
   99         0x63      79         D#/Eb7     2489.02
   98         0x62      78         D7         2349.32
   97         0x61      77         C#/Db7     2217.46
   96         0x60      76         C7         2093.00
   95         0x5F      75         B6         1975.53
   94         0x5E      74         A#/Bb6     1864.66
   93         0x5D      73         A6         1760.00
   92         0x5C      72         G#/Ab6     1661.22
   91         0x5B      71         G6         1567.98
   90         0x5A      70         F#/Gb6     1479.98
   89         0x59      69         F6         1396.91
   88         0x58      68         E6         1318.51
   87         0x57      67         D#/Eb6     1244.51
   86         0x56      66         D6         1174.66
   85         0x55      65         C#/Db6     1108.73
   84         0x54      64         C6         1046.50
   83         0x53      63         B5         987.77
   82         0x52      62         A#/Bb5     932.33
   81         0x51      61         A5         880.00
   80         0x50      60         G#/Ab5     830.61
   79         0x4F      59         G5         783.99
   78         0x4E      58         F#/Gb5     739.99
   77         0x4D      57         F5         698.46
   76         0x4C      56         E5         659.26
   75         0x4B      55         D#/Eb5     622.25
   74         0x4A      54         D5         587.33
   73         0x49      53         C#/Db5     554.37
   72         0x48      52         C5         523.25
   71         0x47      51         B4         493.88
   70         0x46      50         A#/Bb4     466.16
   69         0x45      49         A4         440.00
   68         0x44      48         G#/Ab4     415.30
   67         0x43      47         G4         392.00
   66         0x42      46         F#/Gb4     369.99
   65         0x41      45         F4         349.23
   64         0x40      44         E4         329.63
   63         0x3F      43         D#/Eb4     311.13
   62         0x3E      42         D4         293.66
   61         0x3D      41         C#/Db4     277.18
   60         0x3C      40         C4         261.63
   59         0x3B      39         B3         246.94
   58         0x3A      38         A#/Bb3     233.08
   57         0x39      37         A3         220.00
   56         0x38      36         G#/Ab3     207.65
   55         0x37      35         G3         196.00
   54         0x36      34         F#/Gb3     185.00
   53         0x35      33         F3         174.61
   52         0x34      32         E3         164.81
   51         0x33      31         D#/Eb3     155.56
   50         0x32      30         D3         146.83
   49         0x31      29         C#/Db3     138.59
   48         0x30      28         C3         130.81
   47         0x2F      27         B2         123.47
   46         0x2E      26         A#/Bb2     116.54
   45         0x2D      25         A2         110.00
   44         0x2C      24         G#/Ab2     103.83
   43         0x2B      23         G2         98.00
   42         0x2A      22         F#/Gb2     92.50
   41         0x29      21         F2         87.31
   40         0x28      20         E2         82.41
   39         0x27      19         D#/Eb2     77.78
   38         0x26      18         D2         73.42
   37         0x25      17         C#/Db2     69.30
   36         0x24      16         C2         65.41
   35         0x23      15         B1         61.74
   34         0x22      14         A#/Bb1     58.27
   33         0x21      13         A1         55.00
   32         0x20      12         G#/Ab1     51.91
   31         0x1F      11         G1         49.00
   30         0x1E      10         F#/Gb1     46.25
   29         0x1D      9          F1         43.65
   28         0x1C      8          E1         41.20
   27         0x1B      7          D#/Eb1     38.89
   26         0x1A      6          D1         36.71
   25         0x19      5          C#/Db1     34.65
   24         0x18      4          C1         32.70
   23         0x17      3          B0         30.87
   22         0x16      2          A#/Bb0     29.14
   21         0x15      1          A0         27.50
   20         0x14      --         G#/Ab0     25.96
   19         0x13      --         G0         24.50
   18         0x12      --         F#/Gb0     23.12
   17         0x11      --         F0         21.83
   16         0x10      --         E0         20.60
   15         0x0F      --         D#/Eb0     19.45
   14         0x0E      --         D0         18.35
   13         0x0D      --         C#/Db0     17.32
   12         0x0C      --         C0         16.35
   11         0x0B      --         B-1        15.43
   10         0x0A      --         A#/Bb-1    14.57
   9          0x09      --         A-1        13.75
   8          0x08      --         G#/Ab-1    12.98
   7          0x07      --         G-1        12.25
   6          0x06      --         F#/Gb-1    11.56
   5          0x05      --         F-1        10.91
   4          0x04      --         E-1        10.30
   3          0x03      --         D#/Eb-1    9.72
   2          0x02      --         D-1        9.18
   1          0x01      --         C#/Db-1    8.66
   0          0x00      --         C-1        8.18
*/


#include "Ticker.h"
#include <FastLED.h>
#include <usbh_midi.h>
#include <usbhub.h>
#include <EEPROM.h>

// #define DEBUG

#define NUM_LEDS 175 // Max NUM_LEDS = 180
#define NUM_KEYS 88

#define STRIP_PIN 2
// #define STRIP_CLOCK 3
#define START_NOTE 21 // A0, leftmost key on your keyboard
#define MIDI_OFFSET 0

#define FUNC_KEY_PREV 21
#define FUNC_KEY_NEXT 22
#define FUNC_KEY_SET 23
#define FUNC_KEY_CONFIRM 23

#define FPS 60
#define MAX_ALPHA 255

#define CONFIG_SIZE 10 // 10 bytes for each config
#define NUM_SAVE_SLOTS 5
#define NUM_SETTING_KEYS 4

const static char projectTitle[] = "FanLEDPiano V001";
const static uint8_t projectTitleLength = 16;

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

const static uint8_t settingLedLeftStart = 0;
const static uint8_t settingLedLeftEnd = 7;
const static uint8_t styleNumLedStart = 8;
const static uint8_t styleNumLedEnd = 15;
const static uint8_t settingLedRightStart = 163;
const static uint8_t settingLedRightEnd = 174;

const static uint8_t slotKeys[NUM_SAVE_SLOTS] = {82, 83, 84, 85, 86}; // from G7 to B7, index of keyData[]
const static uint8_t confirmKey = 87; // C8, index of keyData[]
const static uint8_t settingKeys[NUM_SETTING_KEYS] = {0, 1, 2, 3}; // from A0 to C1, index of keyData[]

const static uint8_t bgAnimationNum = 10;
const static uint8_t bgAnimationList[bgAnimationNum] =
{ 0x00, 0x01,
  0x10, 0x11, 0x12, 0x13, 0x14,
  0x20, 0x21, 0x22
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

uint8_t bgAnimation = 0x01;
uint8_t bgColorIdle = 0x01; // Hue or gradient color
uint8_t bgSVIdle = 0x00; // Saturation & Brightness
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

  int huePeriod = 0;
  int hueCount = 0;

  // Notice: Remember to add your new code to bgAnimationList[]
  switch (bgAnimation) { // set hue period
    case 0x20: // dynamic rainbow left to right
    case 0x21: // dynamic rainbow rigth to left
      huePeriod = NUM_LEDS;
      ++frameCount;
      if (frameCount >= huePeriod) {
        frameCount = 0;
      }
      break;

    case 0x22: // dynamic rainbow breath
      huePeriod = 255;
      ++frameCount;
      if (frameCount >= huePeriod) {
        frameCount = 0;
      }
      break;

    default:
      huePeriod = NUM_LEDS;
      break;
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
        idleBrightness = uint8_t(float(activatedBrightness - idleBrightness) * powerRatio + 0.5 + float(idleBrightness));
        hueCount = j + frameCount;
        break;

      case 0x20: // dynamic rainbow left to right
        hueCount = j + huePeriod - frameCount;
        break;

      case 0x21: // dynamic rainbow rigth to left
        hueCount = j + frameCount;
        break;

      case 0x22: // dynamic rainbow breath
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
  const static float fadeSlow = 0.97;
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
  const static uint8_t defaultV2 = 0xD0;
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
  const static CHSV ledOn = CHSV(0, 0, 0xC0);
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
  if (bri >= 0x0F) {
    return sat;
  } else {
    return sat | (bri + 1);
  }
}

void nextStyle() {
  switch (settingStatus) {
    case 0x10: // bgAnimation
      bgAnimation = getNextListData(bgAnimationList, 0, bgAnimationNum, bgAnimation);
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
    return sat | 0x0F;
  } else {
    return sat | (bri - 1);
  }
}

void prevStyle() {
  switch (settingStatus) {
    case 0x10: // bgAnimation
      bgAnimation = getPrevListData(bgAnimationList, 0, bgAnimationNum, bgAnimation);
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
    EEPROM.update(eepromPointer++, 0x01); // bgAnimation
    EEPROM.update(eepromPointer++, 0x01); // bgColorIdle
    EEPROM.update(eepromPointer++, 0x82); // bgSVIdle
    EEPROM.update(eepromPointer++, 0x04); // bgColorActivated
    EEPROM.update(eepromPointer++, 0xE8); // bgSVActivated

    EEPROM.update(eepromPointer++, 0x01); // keyAnimation
    EEPROM.update(eepromPointer++, 0x07); // whiteKeyColor
    EEPROM.update(eepromPointer++, 0xAD); // whiteKeySV
    EEPROM.update(eepromPointer++, 0x01); // blackKeyColor
    EEPROM.update(eepromPointer++, 0xAD); // blackKeySV
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

bool loadSetting(uint8_t _configNum) {
  int eepromPointer = projectTitleLength + 1 + _configNum * CONFIG_SIZE;
  uint8_t rawData;
  bool noError = true;
  bool dataValid;

  // bgAnimation
  rawData = EEPROM.read(eepromPointer++); dataValid = false;
#ifdef DEBUG
  Serial.println(rawData, HEX);
#endif
  for (int i = 0; i < bgAnimationNum; ++i) {
    if (bgAnimationList[i] == rawData) {
      bgAnimation = rawData;
      dataValid = true;
      break;
    }
  }
  noError &= dataValid;

  // bgColorIdle
  rawData = EEPROM.read(eepromPointer++); dataValid = false;
#ifdef DEBUG
  Serial.println(rawData, HEX);
#endif
  for (int i = 0; i < bgColorNum; ++i) {
    if (bgColorList[i] == rawData) {
      bgColorIdle = rawData;
      dataValid = true;
      break;
    }
  }
  noError &= dataValid;

  // bgSVIdle
  rawData = EEPROM.read(eepromPointer++);
#ifdef DEBUG
  Serial.println(rawData, HEX);
#endif
  bgSVIdle = rawData;

  // bgColorActivated
  rawData = EEPROM.read(eepromPointer++); dataValid = false;
#ifdef DEBUG
  Serial.println(rawData, HEX);
#endif
  for (int i = 0; i < bgColorNum; ++i) {
    if (bgColorList[i] == rawData) {
      bgColorActivated = rawData;
      dataValid = true;
      break;
    }
  }
  noError &= dataValid;

  // bgSVActivated
  rawData = EEPROM.read(eepromPointer++);
#ifdef DEBUG
  Serial.println(rawData, HEX);
#endif
  bgSVActivated = rawData;


  // keyAnimation
  rawData = EEPROM.read(eepromPointer++); dataValid = false;
#ifdef DEBUG
  Serial.println(rawData, HEX);
#endif
  for (int i = 0; i < keyAnimationNum; ++i) {
    if (keyAnimationList[i] == rawData) {
      keyAnimation = rawData;
      dataValid = true;
      break;
    }
  }
  noError &= dataValid;

  // whiteKeyColor
  rawData = EEPROM.read(eepromPointer++); dataValid = false;
#ifdef DEBUG
  Serial.println(rawData, HEX);
#endif
  for (int i = 0; i < keyColorNum; ++i) {
    if (keyColorList[i] == rawData) {
      whiteKeyColor = rawData;
      dataValid = true;
      break;
    }
  }
  noError &= dataValid;

  // whiteKeySV
  rawData = EEPROM.read(eepromPointer++);
#ifdef DEBUG
  Serial.println(rawData, HEX);
#endif
  whiteKeySV = rawData;

  // blackKeyColor
  rawData = EEPROM.read(eepromPointer++); dataValid = false;
#ifdef DEBUG
  Serial.println(rawData, HEX);
#endif
  for (int i = 0; i < keyColorNum; ++i) {
    if (keyColorList[i] == rawData) {
      blackKeyColor = rawData;
      dataValid = true;
      break;
    }
  }
  noError &= dataValid;

  // blackKeySV
  rawData = EEPROM.read(eepromPointer++);
#ifdef DEBUG
  Serial.println(rawData, HEX);
#endif
  blackKeySV = rawData;

#ifdef DEBUG
  if (!noError) {
    Serial.print("Error in config ");
    Serial.println(_configNum);
  }
#endif

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
  uint8_t outBuf[3];
  uint8_t size;

  do {
    if ( (size = Midi.RecvData(outBuf)) > 0 ) {

#ifdef DEBUG
      Serial.print("midi_in[");
      Serial.print(size);
      Serial.print("] = [ ");
      for (int i = 0; i < size; ++i) {
        Serial.print("0x");
        Serial.print(outBuf[i], HEX);
        Serial.print(" ");
      }
      Serial.println("]");
#endif

      uint8_t statusCode = outBuf[0] & 0xF0;
      if (statusCode == 0x80 || statusCode == 0x90) {
        uint8_t pitch = outBuf[1] + MIDI_OFFSET;
        uint8_t velocity = outBuf[2];
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
                  case 0: prevStyle(); break;
                  case 1: nextStyle(); break;
                  case 2: prevSetting(); break;
                  case 3: nextSetting(); break;

                  case 82: switchToConfig(0); break;
                  case 83: switchToConfig(1); break;
                  case 84: switchToConfig(2); break;
                  case 85: switchToConfig(3); break;
                  case 86: switchToConfig(4); break;

                  case 87:
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
  delay(200);
  checkSavedConfig();
  settingStatus = 0x10;
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
