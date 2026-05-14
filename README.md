# Arduino Dutch Word Clock

Arduino firmware for a 3D-printed Dutch word clock that displays the current time in words using individually addressable LEDs.

- **Blog post:** [Building a 3D-Printed Arduino Word Clock](https://sanderdesnaijer.com/blog/3d-printed-arduino-word-clock)
- **Project:** [sanderdesnaijer.com/projects/arduino-3d-printed-dutch-word-clock](https://sanderdesnaijer.com/projects/arduino-3d-printed-dutch-word-clock)
- **TypeScript prototype:** [typescript-dutch-wordclock](https://github.com/sanderdesnaijer/typescript-dutch-wordclock) / browser-based prototype used to develop the letter layout and timing logic
- **Live demo (prototype):** [sanderdesnaijer.github.io/typescript-dutch-wordclock](https://sanderdesnaijer.github.io/typescript-dutch-wordclock/)

## How it works

The clock reads the current time from a DS3231 real-time clock module and lights up the corresponding Dutch words on a grid of WS2812b LEDs. The letter layout and word mappings are defined in the `.ino` file.

## Getting started

Open the `.ino` file in the Arduino IDE, install the required libraries, and upload to your board. Set `isDebug` to `true` if you want to test without a clock module connected.

## Requirements

- Arduino board
- DS3231 real-time clock module
- WS2812b individually addressable LED strip
- 3D-printed clock housing (see the [blog post](https://sanderdesnaijer.com/blog/3d-printed-arduino-word-clock) for details), or build the enclosure in your preferred material. The letter layout is defined inside the `.ino` file

## Libraries

- [DS3231](https://github.com/NorthernWidget/DS3231) / real-time clock
- [Wire](https://www.arduino.cc/en/reference/wire) / I2C communication
- [FastLED](https://github.com/FastLED/FastLED) / LED control

## Related

- [sanderdesnaijer.com](https://sanderdesnaijer.com) / portfolio and blog
- [Building a 3D-Printed Arduino Word Clock](https://sanderdesnaijer.com/blog/3d-printed-arduino-word-clock) / full build guide
- [typescript-dutch-wordclock](https://github.com/sanderdesnaijer/typescript-dutch-wordclock) / TypeScript prototype with live demo
