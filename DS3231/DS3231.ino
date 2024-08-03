/*
DS3231.ino
Sander de Snaijer
7/24

Turns on led lights based on wired index (index 0 left to right, index 1 right to left, index 2 left to right etc...) and time

Inspired by https://gist.github.com/lean8086 fastLed gist: https://gist.github.com/lean8086/87455e205271c8d83e8e30e5d04b24d4 
*/

#include <DS3231.h>
#include <Wire.h>
#include <FastLED.h>

// DS3231
DS3231 myRTC;
bool h12Flag = false;
bool pmFlag = false;
DateTime myDT;

// FastLed
#define DATA_PIN 17 // A3 on Nano
#define TOTAL_LEDS 156
#define BRIGHTNESS 123
#define LED_TYPE WS2812B
#define POWERUP_SAFETY_DELAY 3000  // Avoid to fry the LEDs
#define MAX_SENTENCE_LENGTH 19
#define MAX_HOURS_LENGTH 6
#define MAX_HOURS_STRING_LENGTH 3

CRGB leds[TOTAL_LEDS];

const int PROGMEM TopNumbers[12][MAX_HOURS_LENGTH] = {
  { 79, 80, 81, 82, 83, 84 },
  { 7, 8, 9, -1, -1, -1 },
  { 22, 23, 24, 25, -1, -1 },
  { 18, 19, 20, 21, -1, -1 },
  { 14, 15, 16, 17, -1, -1 },
  { 26, 27, 28, 29, -1, -1 },
  { 30, 31, 32, -1, -1, -1 },
  { 33, 34, 35, 36, 37, -1 },
  { 52, 53, 54, 55, -1, -1 },
  { 44, 45, 46, 47, 48, -1 },
  { 61, 62, 63, 64, -1, -1 },
  { 70, 71, 72, -1, -1, -1 },
};

const int PROGMEM BottomNumbers[12][MAX_HOURS_LENGTH] = {
  { 147, 148, 149, 150, 151, 152 },
  { 91, 92, 93, -1, -1, -1 },
  { 104, 105, 106, 107, -1, -1 },
  { 108, 109, 110, 111, -1, -1 },
  { 112, 113, 114, 115, -1, -1 },
  { 126, 127, 128, 129, -1, -1 },
  { 123, 124, 125, -1, -1, -1 },
  { 118, 119, 120, 121, 122, -1 },
  { 130, 131, 132, 133, -1, -1 },
  { 134, 135, 136, 137, 138, -1 },
  { 139, 140, 141, 142, -1, -1 },
  { 153, 154, 155, -1, -1, -1 },
};

const PROGMEM int SentenceLetters[60][MAX_SENTENCE_LENGTH] = {
  { 0, 1, 2, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 7, 8, 9, 100, 101, 102, 103, -1, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 22, 23, 24, 25, 100, 101, 102, 103, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 18, 19, 20, 21, 100, 101, 102, 103, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 14, 15, 16, 17, 100, 101, 102, 103, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 26, 27, 28, 29, 100, 101, 102, 103, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 30, 31, 32, 100, 101, 102, 103, -1, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 33, 34, 35, 36, 37, 100, 101, 102, 103, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 52, 53, 54, 55, 100, 101, 102, 103, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 44, 45, 46, 47, 48, 100, 101, 102, 103, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 61, 62, 63, 64, 100, 101, 102, 103, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 70, 71, 72, 100, 101, 102, 103, -1, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 79, 80, 81, 82, 83, 84, 100, 101, 102, 103, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 49, 50, 51, 61, 62, 63, 64, 60, 100, 101, 102, 103, -1, -1 },
  { 0, 1, 2, 4, 5, 56, 57, 58, 59, 61, 62, 63, 64, 60, 100, 101, 102, 103, -1 },
  { 0, 1, 2, 4, 5, 73, 74, 75, 76, 77, 100, 101, 102, 103, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 30, 31, 32, 60, 61, 62, 63, 64, 100, 101, 102, 103, -1, -1 },
  { 0, 1, 2, 4, 5, 33, 34, 35, 36, 37, 60, 61, 62, 63, 64, 100, 101, 102, 103 },
  { 0, 1, 2, 4, 5, 52, 53, 54, 55, 60, 61, 62, 63, 64, 100, 101, 102, 103, -1 },
  { 0, 1, 2, 4, 5, 44, 45, 46, 47, 48, 60, 61, 62, 63, 64, 100, 101, 102, 103 },
  { 0, 1, 2, 4, 5, 61, 62, 63, 64, 86, 87, 88, 89, 95, 96, 97, 98, -1, -1 },
  { 0, 1, 2, 4, 5, 44, 45, 46, 47, 48, 86, 87, 88, 89, 95, 96, 97, 98, -1 },
  { 0, 1, 2, 4, 5, 52, 53, 54, 55, 86, 87, 88, 89, 95, 96, 97, 98, -1, -1 },
  { 0, 1, 2, 4, 5, 33, 34, 35, 36, 37, 86, 87, 88, 89, 95, 96, 97, 98, -1 },
  { 0, 1, 2, 4, 5, 30, 31, 32, 86, 87, 88, 89, 95, 96, 97, 98, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 26, 27, 28, 29, 86, 87, 88, 89, 95, 96, 97, 98, -1, -1 },
  { 0, 1, 2, 4, 5, 14, 15, 16, 17, 86, 87, 88, 89, 95, 96, 97, 98, -1, -1 },
  { 0, 1, 2, 4, 5, 18, 19, 20, 21, 86, 87, 88, 89, 95, 96, 97, 98, -1, -1 },
  { 0, 1, 2, 4, 5, 22, 23, 24, 25, 86, 87, 88, 89, 95, 96, 97, 98, -1, -1 },
  { 0, 1, 2, 4, 5, 7, 8, 9, 86, 87, 88, 89, 95, 96, 97, 98, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 95, 96, 97, 98, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 7, 8, 9, 100, 101, 102, 103, 95, 96, 97, 98, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 22, 23, 24, 25, 100, 101, 102, 103, 95, 96, 97, 98, -1, -1 },
  { 0, 1, 2, 4, 5, 18, 19, 20, 21, 100, 101, 102, 103, 95, 96, 97, 98, -1, -1 },
  { 0, 1, 2, 4, 5, 14, 15, 16, 17, 100, 101, 102, 103, 95, 96, 97, 98, -1, -1 },
  { 0, 1, 2, 4, 5, 26, 27, 28, 29, 100, 101, 102, 103, 95, 96, 97, 98, -1, -1 },
  { 0, 1, 2, 4, 5, 30, 31, 32, 100, 101, 102, 103, 95, 96, 97, 98, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 33, 34, 35, 36, 37, 100, 101, 102, 103, 95, 96, 97, 98, -1 },
  { 0, 1, 2, 4, 5, 52, 53, 54, 55, 100, 101, 102, 103, 95, 96, 97, 98, -1, -1 },
  { 0, 1, 2, 4, 5, 44, 45, 46, 47, 48, 100, 101, 102, 103, 95, 96, 97, 98, -1 },
  { 0, 1, 2, 4, 5, 61, 62, 63, 64, 100, 101, 102, 103, 95, 96, 97, 98, -1, -1 },
  { 0, 1, 2, 4, 5, 44, 45, 46, 47, 48, 60, 61, 62, 63, 64, 86, 87, 88, 89 },
  { 0, 1, 2, 4, 5, 52, 53, 54, 55, 60, 61, 62, 63, 64, 86, 87, 88, 89, -1 },
  { 0, 1, 2, 4, 5, 33, 34, 35, 36, 37, 60, 61, 62, 63, 64, 86, 87, 88, 89 },
  { 0, 1, 2, 4, 5, 30, 31, 32, 60, 61, 62, 63, 64, 86, 87, 88, 89, -1, -1 },
  { 0, 1, 2, 4, 5, 73, 74, 75, 76, 77, 86, 87, 88, 89, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 56, 57, 58, 59, 61, 62, 63, 64, 60, 86, 87, 88, 89, -1 },
  { 0, 1, 2, 4, 5, 49, 50, 51, 61, 62, 63, 64, 60, 86, 87, 88, 89, -1, -1 },
  { 0, 1, 2, 4, 5, 79, 80, 81, 82, 83, 84, 86, 87, 88, 89, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 70, 71, 72, 86, 87, 88, 89, -1, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 61, 62, 63, 64, 86, 87, 88, 89, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 44, 45, 46, 47, 48, 86, 87, 88, 89, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 52, 53, 54, 55, 86, 87, 88, 89, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 33, 34, 35, 36, 37, 86, 87, 88, 89, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 30, 31, 32, 86, 87, 88, 89, -1, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 26, 27, 28, 29, 86, 87, 88, 89, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 14, 15, 16, 17, 86, 87, 88, 89, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 18, 19, 20, 21, 86, 87, 88, 89, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 22, 23, 24, 25, 86, 87, 88, 89, -1, -1, -1, -1, -1, -1 },
  { 0, 1, 2, 4, 5, 7, 8, 9, 86, 87, 88, 89, -1, -1, -1, -1, -1, -1, -1 },
};

const PROGMEM int Hours[MAX_HOURS_STRING_LENGTH] = { 143, 144, 145 };

int lastMinutes = -1;
bool isDebug = false;

void setupLeds() {
  delay(POWERUP_SAFETY_DELAY);
  FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, TOTAL_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  // clear all leds first
  FastLED.clear();  // clear all pixel data
  FastLED.show();
}

void setupClock() {
  Wire.begin();  // Start the I2C interface
}

void setup() {
  setupLeds();
  setupClock();
  Serial.begin(57600);  // Start the serial interface
  Serial.print("initialise clock");
  Serial.println();
}

byte getNextHour() {
  uint32_t timeStamp = DateTime(RTClib::now()).unixtime();
  timeStamp += (1 * (60 * 60));  // add 1 hours
  DateTime myDT2 = DateTime(timeStamp);
  int nextHours = myDT2.hour() % 12;
  return nextHours;
}

void rainbow_wave(uint8_t thisSpeed, uint8_t deltaHue) {  // The fill_rainbow call doesn't support brightness levels.
  uint8_t thisHue = beat8(thisSpeed, 255);                // A simple rainbow march.
  fill_rainbow(leds, TOTAL_LEDS, thisHue, deltaHue);      // Use FastLED's fill_rainbow routine.
}

void turnOnLed(int ledIndex) {
  leds[ledIndex] = CRGB::White;
  if (isDebug) {
    Serial.print(ledIndex);
    Serial.print(" ");
  }
}


void loop() {
  int currentHours = myRTC.getHour(h12Flag, pmFlag) % 12;
  int currentMinutes = myRTC.getMinute();

  if (isDebug) {
    Serial.print(F("current time "));
    Serial.print(currentHours);
    Serial.print(F(":"));
    Serial.print(currentMinutes);
    Serial.println();
  }

  // intro
  if (lastMinutes == -1) {
    Serial.print("start intro show");
    Serial.println();

    rainbow_wave(10, 10);  // Speed, delta hue values.
    FastLED.show();
    delay(5000);
  }


  if (lastMinutes != currentMinutes) {
    // clear all leds first
    FastLED.clear();  // clear all pixel data
    FastLED.show();   // updated leds with cleared data

    // first sentence (it is ...)
    for (int i = 0; i < MAX_SENTENCE_LENGTH; i++) {
      int letter = pgm_read_byte(&SentenceLetters[currentMinutes][i]);
      if (letter >= 0 && letter < TOTAL_LEDS) {
        turnOnLed(letter);
      }
    }

    // full hour
    if (currentMinutes == 0) {
      // the hours (on top on grid)
      for (int i = 0; i < MAX_HOURS_LENGTH; i++) {
        int letter = pgm_read_byte(&TopNumbers[currentHours][i]);
        if (letter >= 0 && letter < TOTAL_LEDS) {
          turnOnLed(letter);
        }
      }
      // ...hour
      for (int i = 0; i < MAX_HOURS_STRING_LENGTH; i++) {
        int letter = Hours[i];
        turnOnLed(letter);
      }

    }
    // all other times
    else {
      int nextHour = currentMinutes < 20 ? currentHours : getNextHour();
      // the hours (on bottom on grid)
      for (int i = 0; i < MAX_HOURS_LENGTH; i++) {
        int letter = pgm_read_byte(&BottomNumbers[nextHour][i]);
        if (letter >= 0 && letter < TOTAL_LEDS) {
          turnOnLed(letter);
        }
      }
    }
    // show current active leds
    FastLED.show();
  }

  lastMinutes = currentMinutes;

  if (isDebug) {
    Serial.println();
  }
  delay(1000);
}
