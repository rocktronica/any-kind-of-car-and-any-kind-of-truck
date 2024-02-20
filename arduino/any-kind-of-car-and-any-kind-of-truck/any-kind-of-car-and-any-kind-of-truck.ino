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
int16_t vehicleX = 0;

void setup() {
  arduboy.boot(); // TODO: use .begin() for Arduboy splash
  arduboy.setFrameRate(15);

  arduboy.initRandomSeed();
  arduboy.invert(INVERT);
}

// TODO: try as overlay on top of play
void titleScreen() {
  Sprites::drawOverwrite(8, 8, title, 0);

  tinyfont.setCursor(26, 30);
  tinyfont.print("ANY KIND OF ANY");
  tinyfont.setCursor(31, 36);
  tinyfont.print("CAR AND TRUCK");

  tinyfont.setCursor(2, 57);
  tinyfont.print("2024");
  tinyfont.setCursor(88, 52);
  tinyfont.print("for:desi");
  tinyfont.setCursor(88, 57);
  tinyfont.print("luv:dada");

  vehicle.minimize();
  vehicleX = (WIDTH - vehicle.getWidth()) / 2;

  vehicle.draw(
    vehicleX,
    GROUND_Y + 1 - vehicle.getHeight(),
    arduboy
  );

  if (arduboy.justPressed(A_BUTTON | B_BUTTON | UP_BUTTON | RIGHT_BUTTON | DOWN_BUTTON | LEFT_BUTTON)) {
    gameStatus = GameStatus::Play;
  }
}

void play() {
  if (DEBUG) {
    tinyfont.setCursor(1, 1);
    tinyfont.print(vehicle.getDebugText());
  }

  vehicle.setCrouchAndJump(
    arduboy.pressed(DOWN_BUTTON) ? vehicle.getCrouchDistance() : 0,
    arduboy.pressed(UP_BUTTON) ? VEHICLE_JUMP : 0
  );

  vehicle.draw(
    vehicleX,
    GROUND_Y + 1 - vehicle.getHeight(),
    arduboy
  );

  if (arduboy.justPressed(A_BUTTON)) {
      gameStatus = GameStatus::TitleScreen;
  }

  if (arduboy.pressed(B_BUTTON)) {
      uint8_t previousWidth = vehicle.getWidth();

      vehicle.randomize();

      vehicleX = getProperlyExposedX(
        vehicleX + (previousWidth - vehicle.getWidth()) / 2,
        vehicle.getWidth()
      );
  }

  if (arduboy.pressed(LEFT_BUTTON)) {
    vehicleX = getProperlyExposedX(vehicleX - VEHICLE_TRAVEL, vehicle.getWidth());
  }

  if (arduboy.pressed(RIGHT_BUTTON)) {
    vehicleX = getProperlyExposedX(vehicleX + VEHICLE_TRAVEL, vehicle.getWidth());
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
