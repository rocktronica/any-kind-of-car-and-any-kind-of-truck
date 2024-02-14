#include <Arduboy2.h>

enum class GameStatus : uint8_t {
  TitleScreen,
  Play,
};

Arduboy2 arduboy;

GameStatus gameStatus = GameStatus::Play;

struct Body {
  uint8_t x;
  uint8_t y;

  uint8_t width;
  uint8_t height;

  void draw() {
    arduboy.drawRect(x, y, width, height);
  }

  void update() {}

  // TODO: combine left + right
  uint8_t getWheelLeftX(uint8_t radius) {
    return x + radius;
  }

  uint8_t getWheelRightX(uint8_t radius) {
    return x + width - radius;
  }

  uint8_t getWheelY() {
    return y + height;
  }
};

struct Wheel {
  uint8_t x;
  uint8_t y;

  uint8_t radius;

  void setXY(uint8_t _x, uint8_t _y) {
    x = _x;
    y = _y;
  }

  void draw() {
    arduboy.drawCircle(x, y, radius);
  }
};

// TODO: what all really needs XY?
struct Car {
  Body body = {20, 20, 50, 30};

  uint8_t wheelRadius = 10;

  Wheel wheels[2] = {
    {0, 0, wheelRadius},
    {0, 0, wheelRadius},
  };

  void draw() {
    body.draw();

    // TODO: tidy
    uint8_t wheelsX[2] = {
      body.getWheelLeftX(wheels[0].radius),
      body.getWheelRightX(wheels[1].radius)
    };

    uint8_t wheelsY = body.getWheelY();
    for (uint8_t i = 0; i < 2; i++) {
      wheels[i].setXY(wheelsX[i], wheelsY);
      wheels[i].draw();
    }
  }
};

Car car;

void setup() {
  arduboy.boot(); // TODO: use .begin() for Arduboy splash
  arduboy.setFrameRate(15);
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
  car.draw();

  if (arduboy.justPressed(A_BUTTON)) {
    gameStatus = GameStatus::TitleScreen;
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
