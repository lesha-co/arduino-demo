#include "init_display.h"

Adafruit_ST7735 *initTFT()
{
  Adafruit_ST7735 *tft = new Adafruit_ST7735(CS_PIN, DC_PIN, RST_PIN);
  tft->initS(); // Initialize ST7735S screen
  tft->setRotation(2);

  tft->fillScreen(ST77XX_BLACK);
  tft->fillCircle(39, 39, 40, ST77XX_RED);
  tft->fillCircle(39, 79, 40, ST77XX_GREEN);
  tft->fillCircle(39, 119, 40, ST77XX_BLUE);
  tft->setTextColor(ST7735_WHITE);
  tft->setTextSize(2);
  tft->println("Tetris");
  tft->setTextSize(1);
  delay(1000);
  tft->fillScreen(ST77XX_BLACK);
  return tft;
}