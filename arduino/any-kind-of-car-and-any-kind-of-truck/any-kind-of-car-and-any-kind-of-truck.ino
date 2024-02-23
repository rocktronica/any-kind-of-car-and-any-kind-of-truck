#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "images.h"
#include "utils.h"
#include "vehicle.h"

# define INVERT             false
# define DEBUG              false
# define GROUND_Y           HEIGHT - 1
# define JUMP_FRAMES_MAX    2

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

Vehicle vehicle;
int16_t vehicleX = 0;
int8_t jumpFramesElapsed = JUMP_FRAMES_MAX;

bool showTitleOverlay = true;

// https://onlinesequencer.net/3857464
// TODO: split into text sections
const uint16_t themeTones[] PROGMEM = {
 NOTE_G3,34,
 NOTE_C5,136, NOTE_C5,136, NOTE_C5,136, NOTE_C5,136, NOTE_G4,204, NOTE_REST,68, NOTE_G4,136, NOTE_E4,136,
 NOTE_E4,136, NOTE_E4,136, NOTE_E4,136, NOTE_C4,272, NOTE_REST,136, NOTE_C4,136, NOTE_C4,136, NOTE_C4,136,
 NOTE_C4,136, NOTE_C4,136, NOTE_C4,136, NOTE_D4,204, NOTE_REST,68, NOTE_C4,204, NOTE_REST,68, NOTE_C4,204,
 NOTE_REST,204, NOTE_C4,68, NOTE_E4,68, NOTE_C5,136,
 TONES_END
};

const uint16_t changeTones[] PROGMEM = {
 NOTE_G3,34,
 NOTE_C4,68, NOTE_E4,68, NOTE_C5,136,
 TONES_END
};

const uint16_t moveTones[] PROGMEM = {
 NOTE_G3,34,
 NOTE_E3,34,
 TONES_END
};

const uint16_t jumpTones[] PROGMEM = {
 NOTE_E4,34, NOTE_D4,34, NOTE_C4,34,
 TONES_END
};

const uint16_t crouchTones[] PROGMEM = {
 NOTE_G3,34,NOTE_F3,68,
 TONES_END
};

void setup() {
  arduboy.beginDoFirst(); // TODO: use .begin() for Arduboy splash

  arduboy.setFrameRate(15);

  arduboy.initRandomSeed();
  arduboy.invert(INVERT);

  vehicle.baby();
  vehicleX = (WIDTH - vehicle.getWidth()) / 2;

  sound.tones(themeTones);
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
    sound.tones(jumpTones);
  } else if (arduboy.pressed(DOWN_BUTTON)) {
    sound.tones(crouchTones);
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
        sound.tones(themeTones);
      } else {
        sound.noTone();
      }
  }

  if (arduboy.pressed(B_BUTTON) && arduboy.notPressed(A_BUTTON)) {
      uint8_t previousWidth = vehicle.getWidth();
      vehicle.randomize();
      vehicleX = vehicle.getProperlyExposedXAgainstPreviousWidth(vehicleX, previousWidth);

      sound.tones(changeTones);
  }

  if (arduboy.pressed(LEFT_BUTTON)) {
    vehicleX = vehicle.getProperlyExposedX(vehicleX - VEHICLE_TRAVEL);
    sound.tones(moveTones);
  }

  if (arduboy.pressed(RIGHT_BUTTON)) {
    vehicleX = vehicle.getProperlyExposedX(vehicleX + VEHICLE_TRAVEL);
    sound.tones(moveTones);
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
