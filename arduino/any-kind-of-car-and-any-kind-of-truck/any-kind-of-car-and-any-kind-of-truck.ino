#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "images.h"
#include "noise.h"
#include "utils.h"
#include "vehicle.h"

# define INVERT             false
# define GROUND_Y           HEIGHT - 1
# define JUMP_FRAMES_MAX    2

# define THEME_1_FRAMES     29
# define THEME_2_FRAMES     55

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

Vehicle vehicle;
int16_t vehicleX = 0;
int8_t jumpFramesElapsed = JUMP_FRAMES_MAX;
int16_t themePlayingFramesElapsed = 0;
int16_t score = 0;

bool showTitleOverlay = true;
bool showAllTitleParts = false;

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();

  arduboy.setFrameRate(15);

  arduboy.invert(INVERT);

  vehicle.baby();
  vehicleX = (WIDTH - vehicle.getWidth()) / 2;

  sound.tones(THEME_TONES);
}

void titleOverlay() {
  Sprites::drawSelfMasked(9, 2, title, 0);

  if (themePlayingFramesElapsed >= THEME_1_FRAMES || showAllTitleParts) {
    tinyfont.setCursor(26, 33);
    tinyfont.print("ANY KIND OF ANY");
    tinyfont.setCursor(31, 38);
    tinyfont.print("CAR AND TRUCK");
  }

  if (themePlayingFramesElapsed >= THEME_2_FRAMES || showAllTitleParts) {
    tinyfont.setCursor(2, 57);
    tinyfont.print("2024");
    tinyfont.setCursor(88, 52);
    tinyfont.print("for:desi");
    tinyfont.setCursor(88, 57);
    tinyfont.print("luv:dada");
  }

  if (themePlayingFramesElapsed < THEME_2_FRAMES) {
    themePlayingFramesElapsed += 1;
  }
}

void play() {
  if (jumpFramesElapsed < JUMP_FRAMES_MAX) {
    jumpFramesElapsed = jumpFramesElapsed + 1;
  } else if (arduboy.pressed(UP_BUTTON)) {
    jumpFramesElapsed = 0;
    sound.tones(JUMP_TONES);
    showAllTitleParts = true;
  } else if (arduboy.pressed(DOWN_BUTTON)) {
    sound.tones(CROUCH_TONES);
    showAllTitleParts = true;
  }

  if (arduboy.pressed(DOWN_BUTTON)) {
    vehicle.setSuspension(-vehicle.getWheelRadius());
  } else if (jumpFramesElapsed < JUMP_FRAMES_MAX) {
    vehicle.setSuspension(vehicle.getWheelRadius() / 2);
  } else {
    vehicle.setSuspension(0);
  }

  vehicle.draw(
    vehicleX,
    GROUND_Y + 1 - vehicle.getHeight()
      - (jumpFramesElapsed < JUMP_FRAMES_MAX ? VEHICLE_JUMP : 0),
    arduboy
  );

  if (arduboy.justPressed(A_BUTTON)) {
      showTitleOverlay = !showTitleOverlay;

      if (showTitleOverlay) {
        sound.tones(THEME_TONES);
        themePlayingFramesElapsed = 0;
        showAllTitleParts = false;
      } else {
        sound.noTone();
      }
  }

  if (arduboy.pressed(B_BUTTON) && arduboy.notPressed(A_BUTTON)) {
      uint8_t previousWidth = vehicle.getWidth();
      vehicle.randomize();
      vehicleX = vehicle.getProperlyExposedXAgainstPreviousWidth(vehicleX, previousWidth);

      sound.tones(CHANGE_TONES);
      showAllTitleParts = true;

      score += 1;
  }

  if (arduboy.pressed(LEFT_BUTTON)) {
    vehicleX = vehicle.getProperlyExposedX(vehicleX - VEHICLE_TRAVEL);
    sound.tones(MOVE_TONES);
    showAllTitleParts = true;
  }

  if (arduboy.pressed(RIGHT_BUTTON)) {
    vehicleX = vehicle.getProperlyExposedX(vehicleX + VEHICLE_TRAVEL);
    sound.tones(MOVE_TONES);
    showAllTitleParts = true;
  }

  if (arduboy.pressed(A_BUTTON | B_BUTTON)) {
    if (arduboy.justPressed(DOWN_BUTTON)) {
      uint8_t previousWidth = vehicle.getWidth();
      vehicle.baby();
      vehicleX = vehicle.getProperlyExposedXAgainstPreviousWidth(vehicleX, previousWidth);
      showAllTitleParts = true;
    }

    if (arduboy.justPressed(UP_BUTTON)) {
      uint8_t previousWidth = vehicle.getWidth();
      vehicle.bigBoy();
      vehicleX = vehicle.getProperlyExposedXAgainstPreviousWidth(vehicleX, previousWidth);
      showAllTitleParts = true;
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
  } else {
    tinyfont.setCursor(1, 1);
    tinyfont.print("SCORE:" + String(score));
  }

  arduboy.display();
}
