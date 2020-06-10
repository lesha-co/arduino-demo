#include "init_display.h"
#include <avr/pgmspace.h>

#define WELL_WIDTH 10
#define WELL_HEIGHT 20
#define SHAPE_SIZE 5
#define WELL_CELL_SIZE 8
#define ROTATE_AROUND_I 2
#define ROTATE_AROUND_J 2
#define SHAPES_COUNT 7

Adafruit_ST7735 *tft;

const uint8_t shape_j_default = (WELL_WIDTH - SHAPE_SIZE) / 2;
const uint8_t shape_rotation_default = 0;

static const uint16_t PROGMEM colors[SHAPES_COUNT] = {ST77XX_RED,
                                                      ST77XX_ORANGE,
                                                      ST77XX_GREEN,
                                                      ST77XX_BLUE,
                                                      ST77XX_YELLOW,
                                                      ST77XX_MAGENTA,
                                                      ST77XX_CYAN};

/**
 * state
 */
uint8_t global_rotation;
uint8_t global_shape;
uint8_t global_shape_i;
uint8_t global_shape_j;
uint8_t global_well[WELL_HEIGHT][WELL_WIDTH];

static const bool PROGMEM shapes[SHAPES_COUNT][SHAPE_SIZE][SHAPE_SIZE] = {
    // L
    {
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 1, 0},
        {0, 0, 0, 0, 0}},
    // L2
    {
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 0, 0, 0}},
    // T
    {
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0}},
    // I
    {
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0}},
    // Z
    {
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0}},
    // Z2
    {
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0}},
    // square
    {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}},

};

uint8_t getShape()
{
  return random(0, SHAPES_COUNT);
}

void rotateCoords(uint8_t coords[2], uint8_t rotation)
{
  rotation %= 4;

  if (rotation == 0)
    return;

  uint8_t i = coords[0];
  coords[0] = coords[1];
  coords[1] = SHAPE_SIZE - i - 1;
  rotateCoords(coords, rotation - 1);
}

void drawShape(uint8_t shapeID, uint8_t rotation, uint8_t i, uint8_t j)
{
  uint8_t tmp_coords[2];
  for (uint8_t coord_i = 0; coord_i < SHAPE_SIZE; coord_i++)
    for (uint8_t coord_j = 0; coord_j < SHAPE_SIZE; coord_j++)
    {
      bool shape_cell = pgm_read_byte(&shapes[shapeID][coord_i][coord_j]);
      if (shape_cell)
      {
        tmp_coords[0] = coord_i;
        tmp_coords[1] = coord_j;
        rotateCoords(tmp_coords, rotation);
        tft->fillRoundRect(
            (tmp_coords[0] + j) * WELL_CELL_SIZE,
            (tmp_coords[1] + i) * WELL_CELL_SIZE,
            WELL_CELL_SIZE, WELL_CELL_SIZE,
            2, colors[shapeID]);
      }
    }
}

void rotate(uint8_t *rotation)
{
  (*rotation += 1) %= 4;
}

void initPRNG()
{
  // gimme more of that entropy
  unsigned long seed0 = analogRead(1);
  unsigned long seed1 = analogRead(2);
  unsigned long seed2 = analogRead(3);
  randomSeed(seed1 * seed2 * seed0);
}

bool checkWellBounds(uint8_t i, uint8_t j)
{
  return i >= 0 && i < WELL_HEIGHT && j >= 0 && j < WELL_WIDTH;
}

bool checkCanAdvance()
{
  uint8_t tmp_coords[2];
  for (size_t coord_i = 0; coord_i < SHAPE_SIZE; coord_i++)
    for (size_t coord_j = 0; coord_j < SHAPE_SIZE; coord_j++)
    {
      bool shape_cell = pgm_read_byte(&shapes[global_shape][coord_i][coord_j]);
      if (shape_cell)
      {
        tmp_coords[0] = coord_i;
        tmp_coords[1] = coord_j;
        rotateCoords(tmp_coords, global_rotation);
        if (!checkWellBounds(tmp_coords[0] + global_shape_i, tmp_coords[1] + global_shape_j))
        {
          return false;
        }
      }
    }
  return true;
}

void resetShape()
{
  global_rotation = shape_rotation_default;
  global_shape = getShape();
  global_shape_i = 0;
  global_shape_j = shape_j_default;
}

void setup(void)
{
  Serial.begin(115200);
  initPRNG();
  tft = initTFT();
  resetShape();
}

void loop()
{
  tft->fillScreen(ST7735_BLACK);

  drawShape(global_shape, global_rotation, global_shape_i, global_shape_j);
  // rotate(&global_rotation);
  if (checkCanAdvance())
  {
    global_shape_i++;
  }
  else
  {
    resetShape();
  }

  delay(50);
}
