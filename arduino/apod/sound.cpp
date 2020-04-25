#include <Arduino.h>
#include "config.h"
#include "pitches.h"
#include "sound.h"

long noteStart = 0;
long noteDur = 0;
const int* noteTrack = nullptr;

void music_update() {
  if (!noteTrack) {
    return;
  }
  long now = millis();
  long delta = now - noteStart;
  if (delta < noteDur) {
    return;
  }
  int freq = *(noteTrack++);
  if (freq < 0) {
    noteTrack = nullptr;
    return;
  }
  noteStart = now;
  noteDur = *(noteTrack++);
  tone(SOUND_PIN, freq, noteDur);
}

void music_play(const int notes[]) {
  noteTrack = notes;
  noteStart = 0;
  noteDur = 0;
  music_update();
}

void music_wait(unsigned long duration) {
  unsigned long start = millis();
  do {
    music_update();
    delay(5);
  } while (millis() - start < duration);
}

void sound_slide(int from, int to, int duration, int speed) {
  int steps = (duration / speed) + 1;
  int delta = (to - from) / steps;
  if (from < to) {
    while (from < to) {
      tone(SOUND_PIN, from, speed);
      from += delta;
      delay(speed);
    }
  } else {
    while (from > to) {
      tone(SOUND_PIN, from, speed);
      from += delta;
      delay(speed);
    }
  }
}

void sound_effect(int nr) {
  switch (nr) {
    case SOUND_EFFECT_POWER_UP:
      sound_slide(440, 880, 400, 10);
      break;
    case SOUND_EFFECT_POWER_DOWN:
      sound_slide(880, 440, 400, 10);
      break;
    case SOUND_EFFECT_VOLTAGE_LOW:
      sound_slide(220, 100, 400, 10);
      break;
    case SOUND_EFFECT_VOLTAGE_OK:
      sound_slide(100, 220, 400, 10);
      break;
    case SOUND_EFFECT_SPEED_UP:
      sound_slide(400, 500, 300, 10);
      break;
    case SOUND_EFFECT_SPEED_DOWN:
      sound_slide(400, 300, 300, 10);
      break;
    default:
      ;
  }
}

