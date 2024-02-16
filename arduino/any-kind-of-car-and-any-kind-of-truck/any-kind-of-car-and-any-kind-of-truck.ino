#include <Arduboy2.h>

enum class GameStatus : uint8_t {
  TitleScreen,
  Play,
};

Arduboy2 arduboy;

GameStatus gameStatus = GameStatus::Play;

# define MIN_CAR_WIDTH      20
# define MIN_CAR_HEIGHT     20
# define MAX_CAR_WIDTH      80
# define MAX_CAR_HEIGHT     30
# define MIN_WHEEL_RADIUS   5
# define MAX_WHEEL_RADIUS   10

# define DEBUG              false

class Body {
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
    Body body;
    uint8_t wheelRadius;
    Wheel wheels[2];
    uint8_t wheelsDistance;
    uint8_t wheelsXOffset;

    uint8_t getHeight() {
      return body.height + wheels[0].radius;
    }

    uint8_t getWidth() {
      return body.width;
    }

    void update(
      uint8_t bodyWidth,
      uint8_t bodyHeight,
      uint8_t wheelRadius,
      bool fixValues
    ) {
      body.width = bodyWidth;
      body.height = bodyHeight;

      if (fixValues) {
        wheelRadius = min(
          body.height,
          min(body.width / 4, wheelRadius)
        );
      }

      for (uint8_t i = 0; i < 2; i++) {
        wheels[i].update(wheelRadius);
      }

      minWheelXOffset = 0;
      wheelsXOffset = getWheelsXOffset(false);
      wheelsDistance = getWheelsDistance(false);
    }

    void randomize() {
      update(
        random(MIN_CAR_WIDTH, MAX_CAR_WIDTH),
        random(MIN_CAR_HEIGHT, MAX_CAR_HEIGHT),
        random(MIN_WHEEL_RADIUS, MAX_WHEEL_RADIUS),
        true
      );

      minWheelXOffset = random(0, wheelRadius + 1);
      wheelsXOffset = getWheelsXOffset(true);
      wheelsDistance = getWheelsDistance(true);
    }

    void debug() {
      arduboy.println("bodyWidth: " + String(body.width));
      arduboy.println("bodyHeight: " + String(body.height));
      arduboy.println("wheelRadius: " + String(wheels[0].radius));
    }

    void draw(uint8_t x, uint8_t y) {
      body.draw(x, y);

      uint8_t wheelsX[2] = {
        wheelsXOffset,
        wheelsXOffset + wheelsDistance,
      };
      for (uint8_t i = 0; i < 2; i++) {
        wheels[i].draw(x + wheelsX[i], y + body.height);
      }
    }

  private:
    uint8_t minWheelXOffset;

    uint8_t getWheelsXOffset(bool randomize) {
      uint8_t min = minWheelXOffset;
      uint8_t max = body.width - minWheelXOffset - wheels[0].radius * 2 - 1;

      if (randomize) {
        return random(min, max + 1);
      }

      return min;
    }

    uint8_t getWheelsDistance(bool randomize) {
      uint8_t max = body.width - wheelsXOffset - 1;
      uint8_t min = wheels[0].radius * 2;

      if (randomize) {
        return random(min, max + 1);
      }

      return max;
    };
};

Car car;

void setup() {
  arduboy.boot(); // TODO: use .begin() for Arduboy splash
  arduboy.setFrameRate(15);

  car.randomize();

  arduboy.initRandomSeed();
  arduboy.invert(true);
}

void titleScreen() {
  arduboy.setCursor(12, 20);
  arduboy.print("Any Kind of Car &");

  arduboy.setCursor(12, 30);
  arduboy.print("Any Kind of Truck");

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
