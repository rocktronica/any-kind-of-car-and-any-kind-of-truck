#pragma once

// https://onlinesequencer.net/3857464

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

