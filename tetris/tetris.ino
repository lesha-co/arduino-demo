#include "init_display.h"
#include <avr/pgmspace.h>

// параметры колодца: высота/ширина в клетках
#define WELL_WIDTH 10
#define WELL_HEIGHT 19
// координаты колодца на экране
#define WELL_DISPLAY_OFFSET 8
// размер клетки
#define WELL_CELL_SIZE 8
// размер массива фигуры
#define SHAPE_SIZE 5
// координаты центра вращения
#define ROTATE_AROUND_I 2
#define ROTATE_AROUND_J 2
// размер массива фигур
#define SHAPES_COUNT 10
// скорость игры
#define DEFAULT_DELAY 1000
// значение пустой клетки в колодце
#define WELL_EMPTY 255
#define BACKGROUND_COLOR ST77XX_BLACK
#define TEXT_COLOR ST77XX_WHITE

// buttons
#define BTN_UP_PIN 6
#define BTN_DOWN_PIN 5
#define BTN_LEFT_PIN 4
#define BTN_RIGHT_PIN 3
#define BTN_MIDDLE_PIN 2

Adafruit_ST7735 *tft;

const uint8_t shape_j_default = (WELL_WIDTH - SHAPE_SIZE) / 2;

static const uint16_t PROGMEM colors[SHAPES_COUNT] = {ST77XX_RED,
                                                      ST77XX_ORANGE,
                                                      ST77XX_GREEN,
                                                      ST77XX_BLUE,
                                                      ST77XX_YELLOW,
                                                      ST77XX_MAGENTA,
                                                      0x601D,
                                                      0x057F,
                                                      0x8888,
                                                      ST77XX_CYAN};

inline uint16_t getColor(uint8_t index)
{
  return pgm_read_word(&colors[index]);
}

/**
 * state
 */
uint8_t global_buttons;
uint8_t global_rotation;
uint8_t global_shape;
int8_t global_shape_i;
int8_t global_shape_j;
uint16_t global_delay;
uint16_t global_score;
size_t global_score_width;
uint32_t last_advance_ms;
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
    // big T
    {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0}},
    //plus
    {
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0}},
    // ZZ
    {
        {0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 1, 0},
        {0, 0, 0, 0, 0}},

};

void initPRNG()
{
  // gimme more of that entropy
  unsigned long seed0 = analogRead(1);
  unsigned long seed1 = analogRead(2);
  unsigned long seed2 = analogRead(3);
  randomSeed(seed1 * seed2 * seed0);
}

/******************************************************************************
 * Генераторы
 */

inline uint8_t getShape()
{
  return random(0, SHAPES_COUNT);
}

inline uint8_t getRotation()
{
  return random(0, 4);
}

/******************************************************************************
 * Утилиты
 */

void rotateCoords(int8_t coords[2], uint8_t rotation)
{
  rotation %= 4;

  if (rotation == 0)
    return;

  uint8_t i = coords[0];
  coords[0] = coords[1];
  coords[1] = SHAPE_SIZE - i - 1;
  rotateCoords(coords, rotation - 1);
}

uint8_t rotate4(uint8_t rotation)
{
  return (rotation + 1) % 4;
}

/******************************************************************************
 * Рисование
 */
void drawCell(uint8_t i, uint8_t j, uint16_t color)
{
  tft->drawRect(
      j * WELL_CELL_SIZE,
      i * WELL_CELL_SIZE + WELL_DISPLAY_OFFSET,
      WELL_CELL_SIZE - 1, WELL_CELL_SIZE - 1,
      color);
  tft->fillRect(
      j * WELL_CELL_SIZE + 2,
      i * WELL_CELL_SIZE + WELL_DISPLAY_OFFSET + 2,
      WELL_CELL_SIZE - 5, WELL_CELL_SIZE - 5,
      color);
}
void drawShape(bool draw)
{
  int8_t tmp_coords[2];
  for (uint8_t coord_i = 0; coord_i < SHAPE_SIZE; coord_i++)
    for (uint8_t coord_j = 0; coord_j < SHAPE_SIZE; coord_j++)
    {
      bool shape_cell = pgm_read_byte(&shapes[global_shape][coord_i][coord_j]);
      if (shape_cell)
      {
        tmp_coords[0] = coord_i;
        tmp_coords[1] = coord_j;
        rotateCoords(tmp_coords, global_rotation);
        drawCell(tmp_coords[0] + global_shape_i, tmp_coords[1] + global_shape_j, draw ? getColor(global_shape) : BACKGROUND_COLOR);
      }
    }
}
void drawWell(bool full)
{
  if (full)
  {
    tft->fillRect(0,
                  WELL_DISPLAY_OFFSET,
                  tft->width(),
                  tft->height() - WELL_DISPLAY_OFFSET,
                  BACKGROUND_COLOR);
  }
  for (size_t i = 0; i < WELL_HEIGHT; i++)
  {
    for (size_t j = 0; j < WELL_WIDTH; j++)
    {
      if (global_well[i][j] != WELL_EMPTY)
        drawCell(i, j, getColor(global_well[i][j]));
    }
  }
}

void drawScore()
{
  tft->fillRect(0, 0, tft->width(), WELL_DISPLAY_OFFSET, BACKGROUND_COLOR);
  tft->setCursor(0, 0);
  global_score_width = tft->println(global_score, 10);
}

/******************************************************************************
 * Игровая логика
 */

// Проверяет, находятся ли координаты в рамках колодца и не задевают ли
// другие фигуры
bool checkWellBounds(uint8_t i, uint8_t j)
{
  if (i < 0 || i >= WELL_HEIGHT || j < 0 || j >= WELL_WIDTH)
    return false;
  if (global_well[i][j] != WELL_EMPTY)
    return false;
  return true;
}

// Проверяет, будет ли легальна ли позиция текущей фигуры на координатах (j,i) и повороте r
bool checkShapeInBounds(uint8_t i, uint8_t j, uint8_t r)
{
  int8_t tmp_coords[2];
  for (size_t coord_i = 0; coord_i < SHAPE_SIZE; coord_i++)
    for (size_t coord_j = 0; coord_j < SHAPE_SIZE; coord_j++)
    {
      bool shape_cell = pgm_read_byte(&shapes[global_shape][coord_i][coord_j]);
      if (!shape_cell)
        continue;

      tmp_coords[0] = coord_i;
      tmp_coords[1] = coord_j;

      rotateCoords(tmp_coords, r);

      if (!checkWellBounds(tmp_coords[0] + i, tmp_coords[1] + j))
      {
        return false;
      }
    }

  return true;
}

// Генерирует новую фигуру
void resetShape()
{
  global_rotation = getRotation();
  global_shape = getShape();
  global_shape_i = 0;
  // global_shape_j = shape_j_default;
  global_shape_j = random(0, WELL_WIDTH - SHAPE_SIZE);
}

// Фигура уперлась во что-то и закончила свое падение.
// Копируем ее клетки в клетки колодца
void commit()
{
  int8_t tmp_coords[2];
  for (size_t coord_i = 0; coord_i < SHAPE_SIZE; coord_i++)
    for (size_t coord_j = 0; coord_j < SHAPE_SIZE; coord_j++)
    {
      bool shape_cell = pgm_read_byte(&shapes[global_shape][coord_i][coord_j]);
      if (!shape_cell)
        continue;

      tmp_coords[0] = coord_i;
      tmp_coords[1] = coord_j;

      rotateCoords(tmp_coords, global_rotation);
      global_well[tmp_coords[0] + global_shape_i][tmp_coords[1] + global_shape_j] = global_shape;
    }
}

void newGame()
{
  global_delay = DEFAULT_DELAY;
  global_score = 0;
  global_score_width = 0;
  tft->fillScreen(BACKGROUND_COLOR);
  resetShape();
  for (size_t i = 0; i < WELL_HEIGHT; i++)
  {
    for (size_t j = 0; j < WELL_WIDTH; j++)
    {
      global_well[i][j] = WELL_EMPTY;
    }
  }
}

void rotate()
{
  if (checkShapeInBounds(global_shape_i, global_shape_j, rotate4(global_rotation)))
  {
    drawShape(false);
    global_rotation = rotate4(global_rotation);
    drawShape(true);
  }
}

void left()
{
  if (checkShapeInBounds(global_shape_i, global_shape_j - 1, global_rotation))
  {
    drawShape(false);
    global_shape_j -= 1;
    drawShape(true);
  }
}

void right()
{
  if (checkShapeInBounds(global_shape_i, global_shape_j + 1, global_rotation))
  {
    drawShape(false);
    global_shape_j += 1;
    drawShape(true);
  }
}

void advance()
{
  last_advance_ms = millis();
  // Шаг фигуры вниз. Если фигура может двигаться, двигаем
  if (checkShapeInBounds(global_shape_i + 1, global_shape_j, global_rotation))
  {
    drawShape(false);
    global_shape_i++;
  }
  else
  {
    global_score += 100;
    // Фигура не может двигаться, коммитим ее, и начинаем заново
    commit();
    bool updateFullWell = checkLinesClear();
    drawWell(updateFullWell);
    drawScore();
    resetShape();
    // TODO: вынести куда-нибудь
    if (!checkShapeInBounds(global_shape_i + 1, global_shape_j, global_rotation))
    {
      // даже новая фигура не может уместиться, ресетим
      newGame();
    }
    return;
  }

  drawShape(true);
}

void checkDrop()
{
  while (global_buttons & (1 << BTN_DOWN_PIN))
  {
    if (checkShapeInBounds(global_shape_i + 1, global_shape_j, global_rotation))
    {
      drawShape(false);
      global_shape_i += 1;
      drawShape(true);
      delay(50);
      getPressedButtons();
    }
    else
    {
      break;
    }
  }
}

bool checkLinesClear()
{
  bool update = false;

  for (size_t i = 0; i < WELL_HEIGHT; i++)
  {

    bool line_clear = true;

    for (size_t j = 0; j < WELL_WIDTH; j++)
    {
      if (global_well[WELL_HEIGHT - i - 1][j] == WELL_EMPTY)
      {
        line_clear = false;
      }
    }

    if (line_clear)
    {
      update = true;
      for (size_t subI = i; subI < WELL_HEIGHT; subI++)
      {
        for (size_t j = 0; j < WELL_WIDTH; j++)
        {
          int copyFromLine = WELL_HEIGHT - subI - 2;
          global_well[WELL_HEIGHT - subI - 1][j] = copyFromLine < 0 ? WELL_EMPTY : global_well[copyFromLine][j];
        }
      }

      i -= 1;
    }
  }
  return update;
}
/******************************************************************************
 * Управление
 */

uint8_t getHeldButtons()
{
  return (!digitalRead(BTN_UP_PIN) ? 1 << BTN_UP_PIN : 0) |
         (!digitalRead(BTN_DOWN_PIN) ? 1 << BTN_DOWN_PIN : 0) |
         (!digitalRead(BTN_RIGHT_PIN) ? 1 << BTN_RIGHT_PIN : 0) |
         (!digitalRead(BTN_LEFT_PIN) ? 1 << BTN_LEFT_PIN : 0) |
         (!digitalRead(BTN_MIDDLE_PIN) ? 1 << BTN_MIDDLE_PIN : 0);
}

uint8_t getPressedButtons()
{
  uint8_t heldButtons = getHeldButtons();

  uint8_t pressed_buttons = heldButtons & (heldButtons ^ global_buttons);
  global_buttons = heldButtons;
  return pressed_buttons;
}

/******************************************************************************
 * Arduino code
 */
void setup(void)
{
  Serial.begin(115200);
  tft = initTFT();
  tft->setTextColor(TEXT_COLOR);
  initPRNG();
  newGame();
  pinMode(BTN_DOWN_PIN, INPUT);       // set pin to input
  pinMode(BTN_LEFT_PIN, INPUT);       // set pin to input
  pinMode(BTN_MIDDLE_PIN, INPUT);     // set pin to input
  pinMode(BTN_RIGHT_PIN, INPUT);      // set pin to input
  pinMode(BTN_UP_PIN, INPUT);         // set pin to input
  digitalWrite(BTN_DOWN_PIN, HIGH);   // turn on pullup resistors
  digitalWrite(BTN_LEFT_PIN, HIGH);   // turn on pullup resistors
  digitalWrite(BTN_MIDDLE_PIN, HIGH); // turn on pullup resistors
  digitalWrite(BTN_RIGHT_PIN, HIGH);  // turn on pullup resistors
  digitalWrite(BTN_UP_PIN, HIGH);     // turn on pullup resistors
}

void loop()
{

  uint8_t pressed = getPressedButtons();
  if (pressed & (1 << BTN_MIDDLE_PIN))
  {
    rotate();
  }
  if (pressed & (1 << BTN_LEFT_PIN))
  {
    left();
  }
  if (pressed & (1 << BTN_RIGHT_PIN))
  {
    right();
  }

  checkDrop();

  if (millis() - last_advance_ms < global_delay)
  {
    delay(50);
    return;
  }

  advance();
}
