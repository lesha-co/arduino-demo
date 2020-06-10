#include "init_display.h"
#include "photo.h"
#include "after_test.h"

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

  display.clearDisplay();

  testdrawbitmap(PHOTO_WIDTH, PHOTO_HEIGHT); // Draw a small bitmap image
}

void testdrawbitmap(int16_t width, int16_t height)
{
  display.clearDisplay();

  display.drawBitmap((display.width() - width) / 2, (display.height() - height) / 2, photo64, width, height, 1);
  display.display();
}
