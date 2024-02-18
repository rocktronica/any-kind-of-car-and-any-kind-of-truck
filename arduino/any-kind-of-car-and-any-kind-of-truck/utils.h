#ifndef Utils_h
#define Utils_h

#include "Arduino.h"

struct Xy {
  uint8_t x;
  uint8_t y;
};

void drawLine(
  int16_t x0,
  int16_t y0,
  int16_t x1,
  int16_t y1,
  Arduboy2 arduboy
) {
  if (x0 == x1 && y0 == y1) {
    return;
  } else if (x0 == x1) {
    arduboy.drawFastVLine(x0, min(y0, y1), abs(y0 - y1) + 1);
  } else if (y0 == y1) {
    arduboy.drawFastHLine(min(x0, x1), y0, abs(x0 - x1) + 1);
  } else {
    arduboy.drawLine(x0, y0, x1, y1);
  }
};

void drawSegmentedLine(Xy points[], uint8_t size, Arduboy2 arduboy) {
  Xy lastPoint = {0,0};

  for (int i = 0; i < size; i++) {
    if (i > 0) {
      drawLine(points[i].x, points[i].y, lastPoint.x, lastPoint.y, arduboy);
    }

    lastPoint = points[i];
  }
};

void drawPolygon(Xy points[], uint8_t size, Arduboy2 arduboy) {
  drawSegmentedLine(points, size, arduboy);
  drawLine(points[0].x, points[0].y, points[size - 1].x, points[size - 1].y, arduboy);
};

void drawChamferedRectangle(
  int16_t 	x,
  int16_t 	y,
  uint8_t 	w,
  uint8_t 	h,

  // NOTES:
  // * I'm using it as a side instead of the hypotenuse of
  //   the chamfer's triangle. Mathematically incorrect but
  //   practically fine and easier to use in practice.
  // * Chamfers are ordered from top left.
  uint8_t c0,
  uint8_t c1,
  uint8_t c2,
  uint8_t c3,

  Arduboy2 arduboy
) {
  Xy points[8] = {
    {x + c0, y},
    {x + w - 1 - c1, y},

    {x + w - 1, y + c1},
    {x + w - 1, y + h - 1 - c2},

    {x + w - 1 - c2, y + h - 1},
    {x + c3, y + h - 1},

    {x, y + h - 1 - c3},
    {x, y + c0},
  };
  drawPolygon(points, 8, arduboy);
};

#endif