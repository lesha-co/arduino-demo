#ifndef DEMO
#define DEMO
#include <stdint.h>
#include "flake.h"
#include "init_display.h"
#include "after_test.h"

void testprint();
void testdrawrect(void);
void testfillrect(void);
void randomSquares(void);
void testdrawcircle(void);
void testfillcircle(void);
void testdrawroundrect(void);
void testfillroundrect(void);
void testdrawtriangle(void);
void testfilltriangle(void);
void testdrawchar(void);
void testdrawstyles(void);
void testscrolltext(void);
void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h);

#endif