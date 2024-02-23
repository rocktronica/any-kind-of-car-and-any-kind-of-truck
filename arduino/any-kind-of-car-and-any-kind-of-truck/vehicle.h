#ifndef Vehicle_h
#define Vehicle_h

#include "utils.h"

// REMEMBER: display is 128x64

# define MIN_CAB_WIDTH        13
# define MIN_CAB_HEIGHT       6
# define MAX_CAB_WIDTH        40
# define MAX_CAB_HEIGHT       25

# define MIN_BOX_WIDTH        18
# define MIN_BOX_HEIGHT       8
# define MAX_BOX_WIDTH        100
# define MAX_BOX_HEIGHT       35

# define TRUNK_CHAMFER        3
# define CAB_TOP_CHAMFER      3
# define CAB_JOINT_CHAMFER    1
# define HOOD_CHAMFER         3

# define WHEEL_TREAD          5
# define MIN_WHEEL_RADIUS     3
# define MAX_WHEEL_RADIUS     25

# define MAX_BUMPER_HEIGHT    8
# define MIN_BUMPER_HEIGHT    5
# define BUMPER_WIDTH         3

# define LIGHTS_WIDTH         2
# define MIN_LIGHTS_HEIGHT    4
# define LIGHTS_BUMPER_GUTTER 1

# define GUTTER               2

# define VEHICLE_TRAVEL       5
# define VEHICLE_JUMP         10
# define MIN_X_EXPOSURE       10

struct Cab {
  uint8_t width;
  uint8_t height;
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

    // TODO: cool hub caps
    void draw(int16_t x, int16_t y, Arduboy2 arduboy) {
      arduboy.drawCircle(x, y, radius, WHITE);
      arduboy.fillCircle(x, y, radius - 1, BLACK);

      arduboy.drawCircle(x, y, max(0, radius - WHEEL_TREAD));
    }
};

class Vehicle {
  public:
    Cab cab;
    Box box;
    Wheel wheels[2];

    void setSuspension(int8_t _suspension) {
      suspension = _suspension;
 }

    uint8_t getHeight() {
      return cab.height + box.height + suspension + wheels[0].radius + 1;
    }

    uint8_t getWidth() {
      return box.width;
    }

    void update(
      uint8_t cabWidth,
      uint8_t cabHeight,
      uint8_t boxWidth,
      uint8_t boxHeight,
      uint8_t wheelRadius
    ) {
      cab.width = min(cabWidth, boxWidth);
      cab.height = cabHeight;

      box.width = boxWidth;
      box.height = boxHeight;

      wheelRadius = min((box.width - GUTTER) / 2, wheelRadius);

      for (uint8_t i = 0; i < 2; i++) {
        wheels[i].update(wheelRadius);
      }

      trunkToHood = (255 / 2);
      wheelsDistance = getMaxWheelsDistance();
    }

    void baby() {
      update(
        MIN_CAB_WIDTH,
        MIN_CAB_HEIGHT,
        MIN_BOX_WIDTH,
        MIN_BOX_HEIGHT,
        MIN_WHEEL_RADIUS
      );

      trunkToHood = (255 / 2);
      wheelsDistance = getMinWheelsDistance();
    }

    void bigBoy() {
      update(
        MAX_CAB_WIDTH,
        MAX_CAB_HEIGHT,
        MAX_BOX_WIDTH,
        MAX_BOX_HEIGHT,
        MAX_WHEEL_RADIUS
      );
    }

    void randomize() {
      // TODO: prefer "typical" shapes
      update(
        random(MIN_CAB_WIDTH, MAX_CAB_WIDTH),
        random(MIN_CAB_HEIGHT, MAX_CAB_HEIGHT),
        random(MIN_BOX_WIDTH, MAX_BOX_WIDTH),
        random(MIN_BOX_HEIGHT, MAX_BOX_HEIGHT),
        random(MIN_WHEEL_RADIUS, MAX_WHEEL_RADIUS)
      );

      trunkToHood = random(0, 255);
      wheelsDistance = getRandomWheelsDistance();
    }

    uint8_t getWheelRadius() {
      return wheels[0].radius;
    }

    void draw(int16_t x, int16_t y, Arduboy2 arduboy) {
      int16_t cabX = x + getCabXFromBox();
      int16_t cabY = y + hum;
      int16_t boxX = x;
      int16_t boxY = y + hum + cab.height;
      int16_t doorX = cabX + cab.width * .4; // TODO: arbitrary

      drawOutline(cabX, cabY, boxX, boxY, arduboy);
      drawWindows(cabX, cabY, doorX, arduboy);
      drawBumpersAndLights(boxX, boxY, arduboy);
      drawDoors(boxY, cabX, doorX, arduboy);

      int16_t wheelsX[2] = {
        getWheelsXFromBox(),
        getWheelsXFromBox() + wheelsDistance
      };
      for (uint8_t i = 0; i < 2; i++) {
        wheels[i].draw(
          x + wheelsX[i],
          y + suspension + cab.height - 1 + box.height,
          arduboy
        );
      }

      hum = (hum - 1) % 2;
    }

    int16_t getProperlyExposedX(int16_t x) {
      return min(
        max(x, -getWidth() + MIN_X_EXPOSURE),
        WIDTH - MIN_X_EXPOSURE
      );
    }

  int16_t getProperlyExposedXAgainstPreviousWidth(
      int16_t x,
      uint8_t previousWidth
    ) {
      return min(
        max(
          x + (previousWidth - getWidth()) / 2,
          -previousWidth + MIN_X_EXPOSURE
        ),
        WIDTH - MIN_X_EXPOSURE
      );
    }

  private:
    // THINK: Is there a more obvious way to express/name this?
    uint8_t trunkToHood; // Avoiding a float. 0 = 0%; 255 = 100%
    uint8_t wheelsDistance;

    int8_t hum = 0;
    int8_t suspension = 0;

    uint8_t getCabXFromBox() {
      return (box.width - cab.width) * (trunkToHood / 255.0);
    }

    uint8_t getWheelsXFromBox() {
      return (box.width - wheelsDistance) * (trunkToHood / 255.0);
    }

    uint8_t getHoodChamfer() {
      return min(HOOD_CHAMFER, box.width - getCabXFromBox() - cab.width);
    }

    uint8_t getBumperHeight() {
      return min(
        max(0, box.height - getHoodChamfer() - LIGHTS_BUMPER_GUTTER - MIN_LIGHTS_HEIGHT),
        MAX_BUMPER_HEIGHT
      );
    }

    uint8_t getLightsHeight() {
      // TODO: fix this sometimes being 255 somehow
      return max(0, box.height - getHoodChamfer() - getBumperHeight() - LIGHTS_BUMPER_GUTTER);
    }

    void drawOutline(
      int16_t cabX,
      int16_t cabY,
      int16_t boxX,
      int16_t boxY,

      Arduboy2 arduboy
    ) {
      uint8_t trunkChamfer = min(TRUNK_CHAMFER, getCabXFromBox());
      uint8_t hoodChamfer = getHoodChamfer();

      uint8_t cabBackJointChamfer = min(
        CAB_JOINT_CHAMFER,
        getCabXFromBox() - trunkChamfer
      );
      uint8_t cabFrontJointChamfer = min(
        CAB_JOINT_CHAMFER,
        box.width - getCabXFromBox() - cab.width - hoodChamfer
      );

      // Clockwise from top left of box
      Xy points[14] = {
        {boxX + trunkChamfer, boxY},
        {cabX - cabBackJointChamfer, boxY},
        {cabX, boxY - cabBackJointChamfer},
        {cabX, cabY + CAB_TOP_CHAMFER},
        {cabX + CAB_TOP_CHAMFER, cabY},
        {cabX + cab.width - 1 - CAB_TOP_CHAMFER, cabY},
        {cabX + cab.width - 1, cabY + CAB_TOP_CHAMFER},
        {cabX + cab.width - 1, boxY - cabFrontJointChamfer},
        {cabX + cab.width - 1 + cabFrontJointChamfer, boxY},
        {boxX + box.width - 1 - hoodChamfer, boxY},
        {boxX + box.width - 1, boxY + hoodChamfer},
        {boxX + box.width - 1, boxY + box.height - 1},
        {boxX, boxY + box.height - 1},
        {boxX, boxY + trunkChamfer},
      };
      drawPolygon(points, 14, arduboy);
    }

    uint8_t getMinWheelsDistance() {
      return wheels[0].radius * 2 + GUTTER;
    };

    uint8_t getMaxWheelsDistance() {
      return box.width;
    };

    uint8_t getRandomWheelsDistance() {
      return random(getMinWheelsDistance(), getMaxWheelsDistance());
    };

    void drawWindows(int16_t cabX, int16_t cabY, int16_t doorX, Arduboy2 arduboy) {
      uint8_t height = cab.height - GUTTER;

      drawChamferedRectangle(
        cabX + (GUTTER + 1),
        cabY + (GUTTER + 1),
        cab.width - (GUTTER + 1) * 2,
        height,
        max(0, CAB_TOP_CHAMFER - GUTTER), max(0, CAB_TOP_CHAMFER - GUTTER), 0, 0,
        arduboy
      );

      arduboy.drawFastVLine(
        doorX,
        cabY + (GUTTER + 1),
        cab.height - 2
      );
    }

    void drawBumpersAndLights(int16_t boxX, int16_t boxY, Arduboy2 arduboy) {
      uint8_t bumperHeight = getBumperHeight();
      uint8_t lightsHeight = getLightsHeight();

      if (bumperHeight >= MIN_BUMPER_HEIGHT) {
        // Left bumper
        drawChamferedRectangle(
          boxX - BUMPER_WIDTH,
          boxY + box.height - bumperHeight,
          BUMPER_WIDTH + 1,
          bumperHeight,
          2, 0, 0, 2,
          arduboy
        );

        // Right bumper
        drawChamferedRectangle(
          boxX + box.width - 1,
          boxY + box.height - bumperHeight,
          BUMPER_WIDTH + 1,
          bumperHeight,
          0, 2, 2, 0,
          arduboy
        );
      }

      // Front lights
      if (lightsHeight >= MIN_LIGHTS_HEIGHT) {
        drawChamferedRectangle(
          boxX + box.width - 1,
          boxY + getHoodChamfer(),
          LIGHTS_WIDTH + 1,
          lightsHeight,
          0, 1, 1, 0,
          arduboy
        );
      }
    }

    void drawDoors(int16_t boxY, int16_t cabX, int16_t doorX, Arduboy2 arduboy) {
      int16_t y = boxY + (GUTTER + 1);
      uint8_t height = max(0, box.height - (GUTTER + 1) * 2);

      if (height > 1) {
        arduboy.drawFastVLine(cabX, y, height);
        arduboy.drawFastVLine(doorX, y, height);
        arduboy.drawFastVLine(cabX + cab.width - 1, y, height);
      }
    }

    // TODO: side mirrors
};

#endif