#include <Arduboy2.h>

enum class GameStatus : uint8_t {
  TitleScreen,
  Play,
};

Arduboy2 arduboy;

GameStatus gameStatus = GameStatus::Play;

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

// TODO: aesthetic radii only
class Wheel {
  public:
    uint8_t radius;

    void update(uint8_t _radius) {
      radius = _radius;
    }

    void draw(uint8_t x, uint8_t y) {
      arduboy.fillCircle(x, y, radius, WHITE);
      arduboy.fillCircle(x, y, radius - tread, BLACK);
    }

  private:
    uint8_t tread = 1;
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
      uint8_t wheelRadius
    ) {
      body.width = bodyWidth;
      body.height = bodyHeight;

      // TODO: extract fixing
      wheelRadius = min(
        body.height,
        min(body.width / 4, wheelRadius)
      );
      minWheelXOffset = random(0, wheelRadius + 1);
      for (uint8_t i = 0; i < 2; i++) {
        wheels[i].update(wheelRadius);
      }

      wheelsXOffset = getWheelsXOffset();
      wheelsDistance = getWheelsDistance();
    }

    void randomize() {
      uint8_t gutter = 10; // TODO: extract/obviate

      update(
        random(10, WIDTH - gutter * 2),
        random(10, HEIGHT - gutter * 2),
        random(4, 30)
      );
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

    uint8_t getWheelsXOffset() {
      uint8_t min = minWheelXOffset;
      uint8_t max = body.width - minWheelXOffset - wheels[0].radius * 2 - 1;

      return random(min, max + 1);
    }

    uint8_t getWheelsDistance() {
      uint8_t max = body.width - wheelsXOffset - 1;
      uint8_t min = wheels[0].radius * 2;

      return random(min, max + 1);
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
  car.draw(
    (WIDTH - car.getWidth()) / 2,
    (HEIGHT - car.getHeight()) / 2
  );
  // car.debug();

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
