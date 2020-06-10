#ifndef INIT_DISPLAY
#define INIT_DISPLAY

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

/*
  GND - GND
  VCC - 3.3v (or VCC)
  D0 - D13 (SCLK)
  D1 - D11 (MOSI)
  RES - D8
  DC - D9
  CS - D10*/

// Comment out above, uncomment this block to use hardware SPI
#define OLED_DC 9
#define OLED_CS 10
#define OLED_RESET 8
inline Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

#endif
