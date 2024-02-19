#ifndef Vehicle_h
#define Vehicle_h

#include "utils.h"

// REMEMBER: display is 128x64

// TODO: really, what's the smallest car?

# define MIN_CAB_WIDTH        20
# define MIN_CAB_HEIGHT       20
# define MAX_CAB_WIDTH        40
# define MAX_CAB_HEIGHT       22

# define MIN_BOX_WIDTH        35
# define MIN_BOX_HEIGHT       10
# define MAX_BOX_WIDTH        80
# define MAX_BOX_HEIGHT       22

# define TRUNK_CHAMFER        3
# define CAB_TOP_CHAMFER      3
# define CAB_JOINT_CHAMFER    2
# define HOOD_CHAMFER         3

// TODO: suspension
# define MIN_WHEEL_RADIUS     5
# define MAX_WHEEL_RADIUS     20

# define BUMPER_HEIGHT        MIN_BOX_HEIGHT - max(TRUNK_CHAMFER, HOOD_CHAMFER)
# define BUMPER_WIDTH         3

# define LIGHTS_WIDTH         2
# define MIN_LIGHTS_HEIGHT    3
# define LIGHTS_BUMPER_GUTTER 1

struct Cab {
  uint8_t width;
  uint8_t height;
  uint8_t xOffset;
};

struct Box {
  uint8_t width;
  uint8_t height;
};

class Wheel {
  public:
    uint8_t radius;

    void update(uint8_t _radius) {
      radius = _radius;
    }

    void draw(uint8_t x, uint8_t y, Arduboy2 arduboy) {
      arduboy.drawCircle(x, y, radius, WHITE);
      arduboy.fillCircle(x, y, radius - 1, BLACK);

      arduboy.drawCircle(x, y, max(0, radius - tread));
    }

  private:
    uint8_t tread = 5;
};

class Vehicle {
  public:
    Cab cab;
    Box box;
    Wheel wheels[2];

    uint8_t wheelsDistance;
    uint8_t wheelsXOffset;

    uint8_t getHeight() {
      return cab.height + box.height + wheels[0].radius + 1;
    }

    uint8_t getWidth() {
      return box.width;
    }

    void update(
      uint8_t cabWidth,
      uint8_t cabHeight,
      uint8_t boxWidth,
      uint8_t boxHeight,
      uint8_t wheelRadius,
      bool fixValues
    ) {
      cab.width = cabWidth;
      cab.height = cabHeight;

      box.width = boxWidth;
      box.height = boxHeight;

      if (fixValues) {
        wheelRadius = min(
          box.height,
          min(box.width / 4, wheelRadius)
        );

        cab.width = min(cab.width, box.width);
      }

      for (uint8_t i = 0; i < 2; i++) {
        wheels[i].update(wheelRadius);
      }

      wheelsXOffset = getWheelsXOffset(false);
      wheelsDistance = getWheelsDistance(false);

      cab.xOffset = 0;
    }

    void randomize() {
      // TODO: prefer "typical" shapes
      update(
        random(MIN_CAB_WIDTH, MAX_CAB_WIDTH),
        random(MIN_CAB_HEIGHT, MAX_CAB_HEIGHT),
        random(MIN_BOX_WIDTH, MAX_BOX_WIDTH),
        random(MIN_BOX_HEIGHT, MAX_BOX_HEIGHT),
        random(MIN_WHEEL_RADIUS, MAX_WHEEL_RADIUS),
        true
      );

      wheelsXOffset = getWheelsXOffset(true);
      wheelsDistance = getWheelsDistance(true);

      // TODO: don't always center against wheels?
      cab.xOffset = min(
        box.width - cab.width,
        max(0, wheelsXOffset + wheelsDistance / 2 - cab.width / 2)
      );
    }

    String getDebugText() {
      return "C:" + String(cab.width) + "x" + String(cab.height) + "\n" +
        "B:" + String(box.width) + "x" + String(box.height) + "\n" +
        "W:" + String(wheels[0].radius);
    }

    void draw(uint8_t x, uint8_t y, Arduboy2 arduboy) {
      uint8_t cabX = x + cab.xOffset;
      uint8_t cabY = y;
      uint8_t boxX = x;
      uint8_t boxY = y + cab.height;
      uint8_t doorX = cabX + cab.width * .4; // TODO: arbitrary

      drawOutline(cabX, cabY, boxX, boxY, arduboy);
      drawWindows(cabX, cabY, doorX, arduboy);
      drawBumpersAndLights(boxX, boxY, arduboy);
      drawDoors(boxY, cabX, doorX, arduboy);

      uint8_t wheelsX[2] = {wheelsXOffset, wheelsXOffset + wheelsDistance};
      for (uint8_t i = 0; i < 2; i++) {
        wheels[i].draw(
          x + wheelsX[i],
          y + cab.height + box.height,
          arduboy
        );
      }
    }

  private:
    uint8_t minWheelXOffset = 0;
    uint8_t gutter = 2;

    uint8_t getHoodChamfer() {
      return min(HOOD_CHAMFER, box.width - cab.xOffset - cab.width);
    }

    uint8_t getBumperHeight() {
      return min(BUMPER_HEIGHT, MIN_BOX_HEIGHT - getHoodChamfer());
    }

    uint8_t getLightsHeight() {
      // TODO: fix this sometimes being 255 somehow
      return max(0, box.height - getHoodChamfer() - getBumperHeight() - LIGHTS_BUMPER_GUTTER);
    }

    void drawOutline(
      uint8_t cabX,
      uint8_t cabY,
      uint8_t boxX,
      uint8_t boxY,

      Arduboy2 arduboy
    ) {
      uint8_t trunk_chamfer = min(TRUNK_CHAMFER, cab.xOffset);
      uint8_t hood_chamfer = getHoodChamfer();

      uint8_t cab_back_joint_chamfer = min(CAB_JOINT_CHAMFER, cab.xOffset - trunk_chamfer);
      uint8_t cab_front_joint_chamfer =
        min(CAB_JOINT_CHAMFER, box.width - cab.xOffset - cab.width - hood_chamfer);

      // Clockwise from top left of box
      Xy points[14] = {
        {boxX + trunk_chamfer, boxY},
        {cabX - cab_back_joint_chamfer, boxY},
        {cabX, boxY - cab_back_joint_chamfer},
        {cabX, cabY + CAB_TOP_CHAMFER},
        {cabX + CAB_TOP_CHAMFER, cabY},
        {cabX + cab.width - 1 - CAB_TOP_CHAMFER, cabY},
        {cabX + cab.width - 1, cabY + CAB_TOP_CHAMFER},
        {cabX + cab.width - 1, boxY - cab_front_joint_chamfer},
        {cabX + cab.width - 1 + cab_front_joint_chamfer, boxY},
        {boxX + box.width - 1 - hood_chamfer, boxY},
        {boxX + box.width - 1, boxY + hood_chamfer},
        {boxX + box.width - 1, boxY + box.height - 1},
        {boxX, boxY + box.height - 1},
        {boxX, boxY + trunk_chamfer},
      };
      drawPolygon(points, 14, arduboy);
    }

    uint8_t getWheelsXOffset(bool randomize) {
      uint8_t min = minWheelXOffset;
      uint8_t max = box.width - minWheelXOffset - wheels[0].radius * 2 - 1;

      if (randomize) {
        return random(min, max + 1);
      }

      return min;
    }

    uint8_t getWheelsDistance(bool randomize) {
      uint8_t max = box.width - wheelsXOffset - 1;
      uint8_t min = wheels[0].radius * 2 + gutter;

      if (randomize) {
        return random(min, max + 1);
      }

      return max;
    };

    void drawWindows(uint8_t cabX, uint8_t cabY, uint8_t doorX, Arduboy2 arduboy) {
      uint8_t height = cab.height - gutter;

      drawChamferedRectangle(
        cabX + (gutter + 1),
        cabY + (gutter + 1),
        cab.width - (gutter + 1) * 2,
        height,
        CAB_TOP_CHAMFER - gutter, CAB_TOP_CHAMFER - gutter, 0, 0,
        arduboy
      );

      arduboy.drawFastVLine(
        doorX,
        cabY + (gutter + 1),
        cab.height - 2
      );
    }

    void drawBumpersAndLights(int8_t boxX, uint8_t boxY, Arduboy2 arduboy) {
      uint8_t bumper_height = getBumperHeight();
      uint8_t lights_y = boxY + getHoodChamfer();
      uint8_t lights_height = getLightsHeight();

      // Left bumper
      drawChamferedRectangle(
        boxX - BUMPER_WIDTH,
        boxY + box.height - bumper_height,
        BUMPER_WIDTH + 1,
        bumper_height,
        2, 0, 0, 2,
        arduboy
      );

      // Right bumper
      drawChamferedRectangle(
        boxX + box.width - 1,
        boxY + box.height - bumper_height,
        BUMPER_WIDTH + 1,
        bumper_height,
        0, 2, 2, 0,
        arduboy
      );

      // Front lights
      if (lights_height >= MIN_LIGHTS_HEIGHT) {
        drawChamferedRectangle(
          boxX + box.width - 1,
          lights_y,
          LIGHTS_WIDTH + 1,
          lights_height,
          0, 1, 1, 0,
          arduboy
        );
      }
    }

    void drawDoors(uint8_t boxY, uint8_t cabX, int8_t doorX, Arduboy2 arduboy) {
      uint8_t y = boxY + ((gutter * 2) + 1);
      uint8_t height = box.height - ((gutter * 2) + 1) * 2;

      arduboy.drawFastVLine(cabX, y, height);
      arduboy.drawFastVLine(doorX, y, height);
      arduboy.drawFastVLine(cabX + cab.width - 1, y, height);
    }

    // TODO: side mirrors
};

#endif