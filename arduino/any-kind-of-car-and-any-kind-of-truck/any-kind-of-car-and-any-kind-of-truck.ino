#include <Arduboy2.h>

enum class GameStatus : uint8_t {
  TitleScreen,
  Play,
};

Arduboy2 arduboy;

GameStatus gameStatus = GameStatus::TitleScreen;

struct Wheel {
  uint8_t x;
  uint8_t y;

  bool up;

  void draw() {
    arduboy.drawCircle(x, y, 5, WHITE);
  }

  void update() {
    if (up) {
      y -= 1;

      if (y <= 10) {
        up = false;
      }
    } else {
      y += 1;

      if (y >= 40) {
        up = true;
      }
    }
  }
};

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

Wheel wheels[2] = {
  {10, 10},
  {30, 10},
};

void play() {
  for (uint8_t i = 0; i < 2; i++) {
    wheels[i].update();
    wheels[i].draw();
  }

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
