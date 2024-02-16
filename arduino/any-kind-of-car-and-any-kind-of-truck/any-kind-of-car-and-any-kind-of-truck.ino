#include <Arduboy2.h>
#include "Images.h"

enum class GameStatus : uint8_t {
  TitleScreen,
  Play,
};

Arduboy2 arduboy;

GameStatus gameStatus = GameStatus::Play;

// REMEMBER: display is 128x64

# define MIN_CAB_WIDTH      20
# define MIN_CAB_HEIGHT     10
# define MAX_CAB_WIDTH      60
# define MAX_CAB_HEIGHT     22

# define MIN_BOX_WIDTH      20
# define MIN_BOX_HEIGHT     10
# define MAX_BOX_WIDTH      80
# define MAX_BOX_HEIGHT     22

# define MIN_WHEEL_RADIUS   5
# define MAX_WHEEL_RADIUS   10

# define INVERT             false
# define DEBUG              false

class Cab {
  public:
    uint8_t width;
    uint8_t height;

    uint8_t fillet = 5;

    void draw(uint8_t x, uint8_t y) {
      arduboy.drawRoundRect(x, y, width, height, fillet, WHITE);
      arduboy.fillRoundRect(x + 1, y + 1, width - 2, height - 2, fillet - 1, BLACK);
    }
};

class Box {
  public:
    uint8_t width;
    uint8_t height;

    void draw(uint8_t x, uint8_t y) {
      arduboy.drawRoundRect(x, y, width, height, fillet);
    }

  private:
    uint8_t fillet = 5;
};

class Wheel {
  public:
    uint8_t radius;

    void update(uint8_t _radius) {
      radius = _radius;
    }

    void draw(uint8_t x, uint8_t y) {
      arduboy.drawCircle(x, y, radius, WHITE);
      arduboy.fillCircle(x, y, radius - 1, BLACK);

      arduboy.drawCircle(x, y, max(0, radius - tread));
    }

  private:
    uint8_t tread = 5;
};

class Car {
  public:
    Cab cab;
    Box box;
    Wheel wheels[2];

    uint8_t wheelsDistance;
    uint8_t wheelsXOffset;

    uint8_t getHeight() {
      return cab.height + box.height - cabBoxOverlap + wheels[0].radius;
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

      cabXOffset = 0;
    }

    void randomize() {
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
      cabXOffset = min(
        box.width - cab.width,
        max(0, wheelsXOffset + wheelsDistance / 2 - cab.width / 2)
      );
    }

    void debug() {
      arduboy.println("cabWidth: " + String(cab.width));
      arduboy.println("cabWidth: " + String(cab.width));
      arduboy.println("boxWidth: " + String(box.width));
      arduboy.println("boxHeight: " + String(box.height));
      arduboy.println("wheelRadius: " + String(wheels[0].radius));
    }

    // NOTE: order is intentional!
    void draw(uint8_t x, uint8_t y) {
      cab.draw(x + cabXOffset, y);
      box.draw(x, y + cab.height - cabBoxOverlap);
      coverSeam(x + cabXOffset, y);

      drawWindow(x + cabXOffset, y);

      uint8_t wheelsX[2] = {wheelsXOffset, wheelsXOffset + wheelsDistance};
      for (uint8_t i = 0; i < 2; i++) {
        wheels[i].draw(
          x + wheelsX[i],
          y + cab.height + box.height - cabBoxOverlap - 1
        );
      }
    }

  private:
    uint8_t minWheelXOffset = 0;

    uint8_t cabXOffset;
    uint8_t cabBoxOverlap = 5 + 1; // TODO: vs fillet

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
      uint8_t min = wheels[0].radius * 2;

      if (randomize) {
        return random(min, max + 1);
      }

      return max;
    };

    void coverSeam(uint8_t cabX, uint8_t cabY) {
      // Cover inner and lower. Good but incomplete...
      arduboy.fillRect(
        cabX + 1,
        cabY + cab.height - cabBoxOverlap,
        cab.width - 2,
        cabBoxOverlap - 0,
        BLACK
      );
    }

    void drawWindow(uint8_t cabX, uint8_t cabY) {
      uint8_t gutter = 2;

      // TODO: arbitrary dimensions
      uint8_t width = (cab.width - (gutter + 1) * 2) / 2;
      uint8_t height = getHeight() - wheels[0].radius * 2 - (gutter + 1) * 2;

      uint8_t fillet = max(0, cab.fillet - (gutter + 1));

      arduboy.drawRoundRect(
        cabX + (cab.width - width) - (gutter + 1),
        cabY + (gutter + 1),
        width,
        max(fillet * 2, height),
        fillet
      );
    }


};

Car car;

void setup() {
  arduboy.boot(); // TODO: use .begin() for Arduboy splash
  arduboy.setFrameRate(15);

  car.randomize();

  arduboy.initRandomSeed();
  arduboy.invert(INVERT);
}

void titleScreen() {
  Sprites::drawOverwrite(0, 0, title, 0);

  if (arduboy.justPressed(A_BUTTON)) {
    gameStatus = GameStatus::Play;
  }
}

void play() {
  if (DEBUG) {
    car.debug();
  }

  car.draw(
    (WIDTH - car.getWidth()) / 2,
    (HEIGHT - car.getHeight()) / 2
  );

  if (arduboy.pressed(A_BUTTON)) {
      car.randomize();
  }
}

void loop() {
  if (!(arduboy.nextFrame())) {
    return;
  }

  arduboy.pollButtons();
  arduboy.clear();

  switch (gameStatus) {
    case GameStatus::TitleScreen:
      titleScreen();
      break;
    case GameStatus::Play:
      play();
      break;
  }

  arduboy.display();
}
