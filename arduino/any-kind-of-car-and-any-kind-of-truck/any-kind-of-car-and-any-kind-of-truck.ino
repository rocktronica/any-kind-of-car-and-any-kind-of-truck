#include <Arduboy2.h>
#include <Tinyfont.h>
#include "images.h"
#include "utils.h"
#include "vehicle.h"

# define INVERT             false
# define DEBUG              false
# define GROUND_Y           HEIGHT - 1
# define JUMP_FRAMES_MAX    2

Arduboy2 arduboy;
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

Vehicle vehicle;
int16_t vehicleX = 0;
int8_t jumpFramesElapsed = JUMP_FRAMES_MAX;

bool showTitleOverlay = true;

void setup() {
  arduboy.boot(); // TODO: use .begin() for Arduboy splash
  arduboy.setFrameRate(15);

  arduboy.initRandomSeed();
  arduboy.invert(INVERT);

  vehicle.baby();
  vehicleX = (WIDTH - vehicle.getWidth()) / 2;
}

void titleOverlay() {
  Sprites::drawSelfMasked(8, 8, title, 0);

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
}

void play() {
  if (DEBUG) {
    tinyfont.setCursor(1, 1);
    tinyfont.print(vehicle.getDebugText());
  }

  if (jumpFramesElapsed < JUMP_FRAMES_MAX) {
    jumpFramesElapsed = jumpFramesElapsed + 1;
  } else if (arduboy.pressed(UP_BUTTON)) {
    jumpFramesElapsed = 0;
  }

  vehicle.setCrouch(
    arduboy.pressed(DOWN_BUTTON) ? vehicle.getCrouchDistance() : 0
  );

  vehicle.draw(
    vehicleX,
    GROUND_Y + 1 - vehicle.getHeight()
      - (jumpFramesElapsed < JUMP_FRAMES_MAX ? VEHICLE_JUMP : 0),
    arduboy
  );

  if (arduboy.justPressed(A_BUTTON)) {
      showTitleOverlay = !showTitleOverlay;
  }

  if (arduboy.pressed(B_BUTTON) && arduboy.notPressed(A_BUTTON)) {
      uint8_t previousWidth = vehicle.getWidth();
      vehicle.randomize();
      vehicleX = vehicle.getProperlyExposedXAgainstPreviousWidth(vehicleX, previousWidth);
  }

  if (arduboy.pressed(LEFT_BUTTON)) {
    vehicleX = vehicle.getProperlyExposedX(vehicleX - VEHICLE_TRAVEL);
  }

  if (arduboy.pressed(RIGHT_BUTTON)) {
    vehicleX = vehicle.getProperlyExposedX(vehicleX + VEHICLE_TRAVEL);
  }

  if (arduboy.pressed(A_BUTTON | B_BUTTON)) {
    if (arduboy.justPressed(DOWN_BUTTON)) {
      uint8_t previousWidth = vehicle.getWidth();
      vehicle.baby();
      vehicleX = vehicle.getProperlyExposedXAgainstPreviousWidth(vehicleX, previousWidth);
    }

    if (arduboy.justPressed(UP_BUTTON)) {
      uint8_t previousWidth = vehicle.getWidth();
      vehicle.bigBoy();
      vehicleX = vehicle.getProperlyExposedXAgainstPreviousWidth(vehicleX, previousWidth);
    }
  }
}

void loop() {
  if (!(arduboy.nextFrame())) {
    return;
  }

  arduboy.pollButtons();
  arduboy.clear();

  arduboy.drawFastHLine(0, GROUND_Y, WIDTH);

  play();

  if (showTitleOverlay) {
    titleOverlay();
  }

  arduboy.display();
}
