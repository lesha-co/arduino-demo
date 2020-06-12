#ifndef _INIT_DISPLAY_H_
#define _INIT_DISPLAY_H_

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define CS_PIN 10
#define DC_PIN 9
#define RST_PIN 8
// SPI scl 13
// SPI sda 11

Adafruit_ST7735 *initTFT();

#endif