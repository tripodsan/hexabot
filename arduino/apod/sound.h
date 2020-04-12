/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef APOD_ARDUINO_SOUND_H
#define APOD_ARDUINO_SOUND_H

#define SOUND_EFFECT_POWER_UP    0
#define SOUND_EFFECT_POWER_DOWN  1
#define SOUND_EFFECT_SPEED_UP    2
#define SOUND_EFFECT_SPEED_DOWN  3

#include "pitches.h"

const int TUNE_SHUTDOWN[] = { NOTE_C5, 200, NOTE_G4, 100, NOTE_G4, 100, NOTE_A4, 200, NOTE_G4, 200, NOTE_OFF, 200, NOTE_B4, 200, NOTE_C5, 200, -1 };
const int TUNE_STARTUP[] = { NOTE_C4, 200, NOTE_E4, 200, NOTE_G4, 200, NOTE_B4, 200, NOTE_OFF, 200, NOTE_B4, 200, NOTE_C5, 200, -1 };

extern void music_update();

extern void music_play(const int notes[]);

extern void music_wait(unsigned long duration);

extern void sound_slide(int from, int to, int duration, int speed);

extern void sound_effect(int nr);

#endif //APOD_ARDUINO_SOUND_H
