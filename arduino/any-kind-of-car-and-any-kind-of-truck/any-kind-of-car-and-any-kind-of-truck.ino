#include <Arduboy2.h>
#include <Tinyfont.h>
#include "images.h"
#include "utils.h"
#include "vehicle.h"

# define INVERT             false
# define DEBUG              false
# define GROUND_Y           HEIGHT - 1

enum class GameStatus : uint8_t {
  TitleScreen,
  Play,
};

Arduboy2 arduboy;
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

GameStatus gameStatus = GameStatus::TitleScreen;

Vehicle vehicle;

void setup() {
  arduboy.boot(); // TODO: use .begin() for Arduboy splash
  arduboy.setFrameRate(15);

  arduboy.initRandomSeed();
  arduboy.invert(INVERT);
}

void titleScreen() {
  Sprites::drawOverwrite(8, 8, title, 0);

  tinyfont.setCursor(26, 30);
  tinyfont.print("ANY KIND OF ANY");
  tinyfont.setCursor(31, 36);
  tinyfont.print("CAR AND TRUCK");

  tinyfont.setCursor(3, 57);
  tinyfont.print("2024");
  tinyfont.setCursor(83, 52);
  tinyfont.print(" for desi");
  tinyfont.setCursor(83, 57);
  tinyfont.print("from dada");

  vehicle.minimize();
  vehicle.draw(
    (WIDTH - vehicle.getWidth()) / 2,
    GROUND_Y + 1 - vehicle.getHeight(),
    arduboy
  );

  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) {
    gameStatus = GameStatus::Play;
  }
}

void play() {
  if (DEBUG) {
    tinyfont.setCursor(1, 1);
    tinyfont.print(vehicle.getDebugText());
  }

  vehicle.draw(
    (WIDTH - vehicle.getWidth()) / 2,
    GROUND_Y + 1 - vehicle.getHeight(),
    arduboy
  );

  if (arduboy.justPressed(A_BUTTON)) {
      gameStatus = GameStatus::TitleScreen;
  }

  if (arduboy.pressed(B_BUTTON)) {
      vehicle.randomize();
  }
}

void loop() {
  if (!(arduboy.nextFrame())) {
    return;
  }

  arduboy.pollButtons();
  arduboy.clear();

  arduboy.drawFastHLine(0, GROUND_Y, WIDTH);

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
