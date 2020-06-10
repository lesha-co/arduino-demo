#include "init_display.h"

Adafruit_ST7735 *initTFT()
{
  Adafruit_ST7735 *tft = new Adafruit_ST7735(CS_PIN, DC_PIN, RST_PIN);
  tft->initS(); // Initialize ST7735S screen
  tft->setRotation(2);
  tft->fillScreen(ST77XX_RED);
  delay(50);
  tft->fillScreen(ST77XX_GREEN);
  delay(50);
  tft->fillScreen(ST77XX_BLUE);
  delay(50);
  tft->fillScreen(ST77XX_BLACK);
  return tft;
}