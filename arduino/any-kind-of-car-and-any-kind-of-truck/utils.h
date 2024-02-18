#ifndef Utils_h
#define Utils_h

#include "Arduino.h"

struct Xy {
  uint8_t x;
  uint8_t y;
};

// TODO: use faster horizontal/vertical methods
// TODO: chamfer
void polygon(Xy points[], uint8_t size, Arduboy2 arduboy) {
  Xy lastPoint = {0,0};

  for (int i = 0; i < size; i++) {
    if (i > 0) {
      arduboy.drawLine(points[i].x, points[i].y, lastPoint.x, lastPoint.y);
    }

    lastPoint = points[i];
  }
};

#endif