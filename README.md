# MSP430-WS2812
WS2811/WS2812/WS2812B RGB LED driver for MSP430G2553 or similar

Inspired by [RoXXoR's MSP430 WS2812 library](https://github.com/RoXXoR/ws2812), with key improvements in memory usage and overall scalability for use on smaller, more easily embedded devices.

Most notably:
  * MCU runs at full 16MHz
  * Encoded data is generated bytewise between SPI transfers to greatly reduce memory consumption.
    * LED Patterns can be written to flash and decoded in software without the use of a frame buffer, allowing *A LOT* of LEDs to be addressed more effectively
  * Uses RoXXoR's 4bit encoding to reduce number of SPI transfers (compared to others' 8 bit encodings)
  * Added ability to control global brightness of the pixels on-the-fly

Future improvements:
  * Add more pattern options
  * Allow upconversion from 565 encoding
  * Better separate LED code from rotary encoder code

Expect frequent cleanups and changes as this project is ongoing.
