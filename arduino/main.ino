/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using SPI to communicate
 4 or 5 pins are required to interface.

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

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
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

#define NUMFLAKES 10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16

static const unsigned char PROGMEM image_bits[] = {
    B00000000, B00000000,
    B00000011, B11000000,
    B00000111, B11100000,
    B00000111, B11100000,
    B00000000, B00100000,
    B00000000, B10100000,
    B00000001, B01000000,
    B00000110, B00000000,
    B00000001, B10000000,
    B00000000, B00000000,
    B00000010, B01000000,
    B00000101, B11110000,
    B00011111, B11111000,
    B00011111, B11111000,
    B00011011, B00111000,
    B00011111, B11111000};

#define PHOTO_WIDTH 64
#define PHOTO_HEIGHT 64
static const unsigned char PROGMEM photo64[] =
    {
        B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00000000, B00000000, B00001000, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00000000, B00011110, B00110010, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00000000, B11111110, B11111101, B10000000, B00000000, B00000000,
        B00000000, B00000000, B00000001, B11011001, B11010110, B11000000, B00000000, B00000000,
        B00000000, B00000000, B00000010, B11011111, B01101011, B11100000, B00000000, B00000000,
        B00000000, B00000000, B00000111, B11111111, B11111111, B01100000, B00000000, B00000000,
        B00000000, B00000000, B00000111, B11111110, B01111101, B11110000, B00000000, B00000000,
        B00000000, B00000000, B00001111, B11110111, B00111110, B11111000, B00000000, B00000000,
        B00000000, B00000000, B00001111, B11011101, B11101111, B10111000, B00000000, B00000000,
        B00000000, B00000000, B00011111, B11111111, B11100110, B01111000, B00000000, B00000000,
        B00000000, B00000000, B00011111, B01110011, B11000111, B10101000, B00000000, B00000000,
        B00000000, B00000000, B00000111, B01101111, B10000111, B11111000, B00000000, B00000000,
        B00000000, B00000000, B00101110, B11001111, B00001010, B10111100, B00000000, B00000000,
        B00000000, B00000000, B00111101, B10011110, B00001001, B11111000, B00000000, B00000000,
        B00000000, B00000000, B00111100, B00011010, B00001001, B11111000, B00000000, B00000000,
        B00000000, B00000000, B00111000, B00000100, B00001100, B01110000, B00000000, B00000000,
        B00000000, B00000000, B00111000, B00000000, B00000000, B01110000, B00000000, B00000000,
        B00000000, B00000000, B00111001, B11111000, B00000110, B00100000, B00000000, B00000000,
        B00000000, B00000000, B00011010, B00000000, B01111110, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00011000, B00000000, B00000000, B01110000, B00000000, B00000000,
        B00000000, B00000000, B00001100, B10110000, B00011110, B11001000, B00000000, B00000000,
        B00000000, B00000000, B00000011, B00110100, B00011011, B01111000, B00000000, B00000000,
        B00000000, B00000000, B00100000, B10000111, B00000010, B01101000, B00000000, B00000000,
        B00000000, B00000000, B00111100, B11111100, B00111110, B01100000, B00000000, B00000000,
        B00000000, B00000000, B00110100, B00000000, B00000000, B00010000, B00000000, B00000000,
        B00000000, B00000000, B00011101, B01000000, B00011100, B00100000, B00000000, B00000000,
        B00000000, B00000000, B00001100, B10100000, B00000110, B10000000, B00000000, B00000000,
        B00000000, B00000000, B00000100, B00000011, B10000000, B10000000, B00000000, B00000000,
        B00000000, B00000000, B00000110, B00000000, B00000000, B10000000, B00000000, B00000000,
        B00000000, B00000000, B00000010, B00000000, B00000000, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00000001, B00000000, B01000001, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00000001, B00000010, B00000000, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00000000, B10000000, B00000010, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00000000, B01000000, B00000100, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00000000, B00110000, B00010000, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00000000, B00011100, B01101000, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00000000, B00010000, B00001000, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00000000, B01110000, B00001010, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00000000, B11010000, B00001011, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00000000, B11111000, B00001111, B00000000, B00000000, B00000000,
        B00000000, B00000000, B00000000, B11111111, B11111111, B11000000, B00000000, B00000000,
        B00000000, B00000000, B00000111, B11111111, B11111111, B11110000, B00000000, B00000000,
        B00000000, B00000000, B00011111, B11111111, B11111111, B11111100, B00000000, B00000000,
        B00000000, B00000000, B00111111, B11111111, B11111111, B11111110, B00000000, B00000000,
        B00000000, B00000000, B11111011, B11110011, B11111111, B11111011, B00000000, B00000000,
        B00000000, B00000001, B11111111, B10111011, B01111111, B11111111, B10000000, B00000000,
        B00000000, B00000011, B11111111, B10010111, B11111111, B11111111, B11000000, B00000000,
        B00000000, B00000111, B11111111, B11111111, B11111111, B11111101, B11100000, B00000000,
        B00000000, B00001111, B00011011, B01111111, B01111111, B11111111, B11100000, B00000000,
        B00000000, B00001111, B11111111, B11000101, B10000001, B00100110, B10110000, B00000000,
        B00000000, B00011111, B11111111, B11111111, B11111111, B11111111, B11110000, B00000000,
        B00000000, B00001111, B11111001, B11111111, B11111111, B11111111, B11110000, B00000000,
        B00000000, B00011111, B11111110, B11111011, B01111111, B10111111, B11110000, B00000000,
        B00000000, B00011111, B11111111, B11111000, B01111111, B01111111, B11110000, B00000000,
        B00000000, B00011111, B11111100, B01110111, B11011100, B11111111, B11110000, B00000000,
        B00000000, B00011111, B11100000, B11111011, B11011010, B11111111, B11111000, B00000000,
        B00000000, B00111111, B11011000, B11110100, B11111110, B10001111, B11111000, B00000000,
        B00000000, B00111111, B11111111, B11111101, B01011111, B10011111, B11111000, B00000000,
        B00000000, B00111111, B11111111, B11111111, B11111101, B11111111, B11111000, B00000000,
        B00000000, B00111111, B11111111, B11111111, B11111111, B11111111, B11111000, B00000000,
        B00000000, B00011111, B11111111, B11111111, B11111111, B11111111, B11111000, B00000000};

void afterTest(int ms)
{
  delay(ms);
}

void setup()
{
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
    {
      // Don't proceed, loop forever
    }
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  afterTest(2000); // Pause for 2 seconds
}

void loop()
{

  // Clear the buffer
  display.clearDisplay();

  //// Draw a single pixel in white
  // display.drawPixel(10, 10, SSD1306_WHITE);
  //// Show the display buffer on the screen. You MUST call display() after
  //// drawing commands to make them visible on screen!
  //  display.display();
  //  afterTest(2000);
  //// display.display() is NOT necessary after every single drawing command,
  //// unless that's what you want...rather, you can batch up a bunch of
  //// drawing operations and then update the screen all at once by calling
  // display.display(). These examples demonstrate both approaches...

  //  testprint();
  //  testdrawline();      // Draw many lines
  //  testdrawrect();      // Draw rectangles (outlines)
  //  testfillrect();      // Draw rectangles (filled)
  //  testdrawcircle();    // Draw circles (outlines)
  //  testfillcircle();    // Draw circles (filled)
  //  testdrawroundrect(); // Draw rounded rectangles (outlines)
  //  testfillroundrect(); // Draw rounded rectangles (filled)
  //  testdrawtriangle();  // Draw triangles (outlines)
  //  testfilltriangle();  // Draw triangles (filled)
  //  testdrawchar();      // Draw characters of the default font
  //  testdrawstyles();    // Draw 'stylized' characters
  //  testscrolltext();    // Draw scrolling text

  testdrawbitmap(PHOTO_WIDTH, PHOTO_HEIGHT); // Draw a small bitmap image

  //// Invert and restore display, pausing in-between
  //  display.invertDisplay(true);
  //  afterTest(1000);
  //  display.invertDisplay(false);
  //  afterTest(1000);
  //
  //  testanimate(image_bits, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps
  //
  //  randomSquares();
}

void testprint()
{
  int incomingByte = 0;                // for incoming serial data
  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.cp437(true);                 // Use full 256 char 'Code Page 437' font

  while (true)
  {
    if (Serial.available() > 0)
    {
      // read the incoming byte:
      incomingByte = Serial.read();
      display.write(incomingByte);
      display.display();
    }
  }
}

void testdrawline()
{
  int16_t i;

  display.clearDisplay(); // Clear display buffer

  for (i = 0; i < display.width(); i += 4)
  {
    display.drawLine(0, 0, i, display.height() - 1, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn line
    afterTest(1);
  }
  for (i = 0; i < display.height(); i += 4)
  {
    display.drawLine(0, 0, display.width() - 1, i, SSD1306_WHITE);
    display.display();
    afterTest(1);
  }
  afterTest(250);

  display.clearDisplay();

  for (i = 0; i < display.width(); i += 4)
  {
    display.drawLine(0, display.height() - 1, i, 0, SSD1306_WHITE);
    display.display();
    afterTest(1);
  }
  for (i = display.height() - 1; i >= 0; i -= 4)
  {
    display.drawLine(0, display.height() - 1, display.width() - 1, i, SSD1306_WHITE);
    display.display();
    afterTest(1);
  }
  afterTest(250);

  display.clearDisplay();

  for (i = display.width() - 1; i >= 0; i -= 4)
  {
    display.drawLine(display.width() - 1, display.height() - 1, i, 0, SSD1306_WHITE);
    display.display();
    afterTest(1);
  }
  for (i = display.height() - 1; i >= 0; i -= 4)
  {
    display.drawLine(display.width() - 1, display.height() - 1, 0, i, SSD1306_WHITE);
    display.display();
    afterTest(1);
  }
  afterTest(250);

  display.clearDisplay();

  for (i = 0; i < display.height(); i += 4)
  {
    display.drawLine(display.width() - 1, 0, 0, i, SSD1306_WHITE);
    display.display();
    afterTest(1);
  }
  for (i = 0; i < display.width(); i += 4)
  {
    display.drawLine(display.width() - 1, 0, i, display.height() - 1, SSD1306_WHITE);
    display.display();
    afterTest(1);
  }

  afterTest(2000); // Pause for 2 seconds
}

void testdrawrect(void)
{
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2; i += 2)
  {
    display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    afterTest(1);
  }

  afterTest(2000);
}

void testfillrect(void)
{
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2; i += 3)
  {
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(i, i, display.width() - i * 2, display.height() - i * 2, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn rectangle
    afterTest(1);
  }

  afterTest(2000);
}

void randomSquares(void)
{
  display.clearDisplay();
  for (int16_t i = 0; i < 2000; i++)
  {
    int x = random(0, display.width());
    int y = random(0, display.height());
    int w = random(0, display.width() - x);
    int h = random(0, display.height() - y);
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(x, y, w, h, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn rectangle
    afterTest(1);
  }
}

void testdrawcircle(void)
{
  display.clearDisplay();

  for (int16_t i = 0; i < max(display.width(), display.height()) / 2; i += 2)
  {
    display.drawCircle(display.width() / 2, display.height() / 2, i, SSD1306_WHITE);
    display.display();
    afterTest(1);
  }

  afterTest(2000);
}

void testfillcircle(void)
{
  display.clearDisplay();

  for (int16_t i = max(display.width(), display.height()) / 2; i > 0; i -= 3)
  {
    // The INVERSE color is used so circles alternate white/black
    display.fillCircle(display.width() / 2, display.height() / 2, i, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn circle
    afterTest(1);
  }

  afterTest(2000);
}

void testdrawroundrect(void)
{
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2)
  {
    display.drawRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i,
                          display.height() / 4, SSD1306_WHITE);
    display.display();
    afterTest(1);
  }

  afterTest(2000);
}

void testfillroundrect(void)
{
  display.clearDisplay();

  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2)
  {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i,
                          display.height() / 4, SSD1306_INVERSE);
    display.display();
    afterTest(1);
  }

  afterTest(2000);
}

void testdrawtriangle(void)
{
  display.clearDisplay();

  for (int16_t i = 0; i < max(display.width(), display.height()) / 2; i += 5)
  {
    display.drawTriangle(
        display.width() / 2, display.height() / 2 - i,
        display.width() / 2 - i, display.height() / 2 + i,
        display.width() / 2 + i, display.height() / 2 + i, SSD1306_WHITE);
    display.display();
    afterTest(1);
  }

  afterTest(2000);
}

void testfilltriangle(void)
{
  display.clearDisplay();

  for (int16_t i = max(display.width(), display.height()) / 2; i > 0; i -= 5)
  {
    // The INVERSE color is used so triangles alternate white/black
    display.fillTriangle(
        display.width() / 2, display.height() / 2 - i,
        display.width() / 2 - i, display.height() / 2 + i,
        display.width() / 2 + i, display.height() / 2 + i, SSD1306_INVERSE);
    display.display();
    afterTest(1);
  }

  afterTest(2000);
}

void testdrawchar(void)
{
  display.clearDisplay();

  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.cp437(true);                 // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for (int16_t i = 0; i < 256; i++)
  {
    if (i == '\n')
      display.write(' ');
    else
      display.write(i);
  }

  display.display();
  afterTest(2000);
}

void testdrawstyles(void)
{
  display.clearDisplay();

  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.println(F("Hello, world!"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x"));
  display.println(0xDEADBEEF, HEX);

  display.display();
  afterTest(2000);
}

void testscrolltext(void)
{
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display(); // Show initial text
  afterTest(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  afterTest(2000);
  display.stopscroll();
  afterTest(1000);
  display.startscrollleft(0x00, 0x0F);
  afterTest(2000);
  display.stopscroll();
  afterTest(1000);
  display.startscrolldiagright(0x00, 0x07);
  afterTest(2000);
  display.startscrolldiagleft(0x00, 0x07);
  afterTest(2000);
  display.stopscroll();
  afterTest(1000);
}

void testdrawbitmap(uint8_t width, uint8_t height)
{
  display.clearDisplay();

  display.drawBitmap((display.width() - width) / 2, (display.height() - height) / 2, photo64, width, height, 1);
  display.display();
}

#define XPOS 0 // Indexes into the 'icons' array in function below
#define YPOS 1
#define DELTAY 2

void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h)
{
  int8_t f, icons[NUMFLAKES][3];

  // Initialize 'snowflake' positions
  for (f = 0; f < NUMFLAKES; f++)
  {
    icons[f][XPOS] = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS] = -LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
    Serial.print(F("x: "));
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(F(" y: "));
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(F(" dy: "));
    Serial.println(icons[f][DELTAY], DEC);
  }
  int i = 0;
  for (; i < 100;)
  {                         // Loop forever...
    display.clearDisplay(); // Clear the display buffer

    // Draw each snowflake:
    for (f = 0; f < NUMFLAKES; f++)
    {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
    }

    display.display(); // Show the display buffer on the screen
    afterTest(200);    // Pause for 1/10 second

    // Then update coordinates of each flake...
    for (f = 0; f < NUMFLAKES; f++)
    {
      icons[f][YPOS] += icons[f][DELTAY];
      icons[f][XPOS] += random(1, 3) * icons[f][DELTAY] - icons[f][DELTAY] * 2;
      // If snowflake is off the bottom of the screen...
      if (icons[f][YPOS] >= display.height())
      {
        i++;
        // Reinitialize to a random position, just off the top
        icons[f][XPOS] = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS] = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
      }
    }
  }
}