#include <Arduboy2.h>

enum class GameStatus : uint8_t {
  TitleScreen,
  Play,
};

Arduboy2 arduboy;

GameStatus gameStatus = GameStatus::Play;

struct Body {
  uint8_t width;
  uint8_t height;

  void draw(uint8_t x, uint8_t y) {
    arduboy.drawRect(x, y, width, height);
  }
};

struct Wheel {
  uint8_t x;
  uint8_t y;

  uint8_t radius;

  void draw(uint8_t x, uint8_t y) {
    arduboy.drawCircle(x, y, radius);
  }
};

struct Car {
  Body body = {50, 30};

  uint8_t wheelRadius = 10;

  Wheel wheels[2] = {
    {0, 0, wheelRadius},
    {0, 0, wheelRadius},
  };

  void draw(uint8_t x, uint8_t y) {
    body.draw(x, y);

    uint8_t wheelsX[2] = {
      wheelRadius,
      body.width - wheelRadius,
    };
    for (uint8_t i = 0; i < 2; i++) {
      wheels[i].draw(x + wheelsX[i], y + body.height);
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
  car.draw(30, 10);

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
