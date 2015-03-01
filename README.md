# MSP430-WS2812
WS2811/WS2812/WS2812B RGB LED driver for MSP430G2553 or similar

Inspired by [RoXXoR's MSP430 WS2812 library](https://github.com/RoXXoR/ws2812), with key improvements in memory usage and overall scalability.

Most notably:
  * MCU runs at full 16MHz
  * Actual encoded data is generated bytewise between SPI transfers to greatly reduce memory consumption (the only data in memory is the actual RGB pixel data)
  * 


Expect frequent cleanups and changes as this project is ongoing.
