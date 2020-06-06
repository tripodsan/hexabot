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

#include "GaitSequencer.h"
#include <cmath>
#include <cstdio>
#include <cstring>

#define movementThreshold 4.0f // threshold above which a movement is required.

Gait::Gait(const char *name, const int len, float *const seq, const int (&phase)[6], uint16_t stepTime) {
  this->name = name;
  this->len = len;
  this->stepTime = stepTime;
  this->seq = (Vec3f *) malloc(sizeof(Vec3f) * len);
  float *x = seq;
  float *y = x + len;
  float *z = y + len;

  for (int i = 0; i < 6; i++) {
    this->phase[i] = phase[i];
  }

  for (size_t i = 0; i < len; i++) {
    this->seq[i].x = *(x++);
    this->seq[i].y = *(y++);
    this->seq[i].z = *(z++);
  }
}

Gait::Gait(const char *name, const int rs, const int ps, const int (&phase)[6], uint16_t stepTime) {
  this->len = rs + ps;
  this->name = name;
  this->stepTime = stepTime;
  this->seq = (Vec3f *) calloc(len, sizeof(Vec3f));
  for (int i = 0; i < 6; i++) {
    this->phase[i] = phase[i];
  }
  if (rs < 2) {
    printf("ERROR: rs needs to be at least 2. (%s gait)\n", name);
    return;
  }
  // return stroke
  float dx = 1.0f / (float) rs;
  float dy = 1.0f / (float) rs;
  float dz = M_PI / (float) rs;
  for (int i = 0 ; i < rs; i++) {
    this->seq[i].x = -0.5f + ((float) i)*dx;
    this->seq[i].y = -0.5f + ((float) i)*dy;

    this->seq[i].z = sinf(((float) i) * dz);
  }
  // power stroke
  dx = 1.0f / (float) ps;
  dy = 1.0f / (float) ps;
  for (int i = 0 ; i < ps; i++) {
    this->seq[i + rs].x = 0.5f - ((float) i)*dx;
    this->seq[i + rs].y = 0.5f - ((float) i)*dy;
    this->seq[i + rs].z = 0.0f;
  }
  printf("Gait: %s\n", name);
  for (int i = 0; i < len; i++) {
    printf("%5.2f ", this->seq[i].y);
  }
  printf("\n");
  for (int i = 0; i < len; i++) {
    printf("%5.2f ", this->seq[i].z);
  }
  printf("\n");
}

Gait::~Gait() {
  free(this->seq);
  this->seq = nullptr;
}

/**
 * Wave Gait:
 * RF: **----------
 * RM: --**--------
 * RR: ----**------
 * LF: ------**----
 * LM: --------**--
 * LR: ----------**
 */
static Gait WAVE = Gait("wave", 2, 10, {0, 2, 4, 6, 8, 10}, 100);

/**
 * Slow Wave Gait:
 */
static Gait WAVE_SLOW = Gait("wave slow", 4, 20, {0, 4, 8, 12, 16, 20}, 100);

/**
 * Half Wave Gait:
 * RF: ****--------
 * RM: ----****----
 * RR: --------****
 * LF: ****--------
 * LM: ----****----
 * LR: --------****
 */
static Gait WAVE_SYNC = Gait("wave sync", 4, 8, {0, 4, 8, 0, 4, 8}, 200);


/**
 * Ripple Gait:
 * RF: **----
 * RM: --**--
 * RR: ----**
 * LF: -**---
 * LM: ---**-
 * LR: *----*
 */
static Gait RIPPLE = Gait("ripple", 2, 4, {0, 2, 4, 1, 3, 5}, 100);

/**
 * Slow Ripple Gait:
 * RF: ****--------
 * RM: ----****----
 * RR: --------****
 * LF: --****------
 * LM: ------****--
 * LR: **--------**
 */
static Gait RIPPLE_SLOW = Gait("ripple slow", 4, 8, {0, 4, 8, 2, 6, 10}, 100);

/**
 * Fast Tripod Gait:
 * RF: **--
 * RM: --**
 * RR: **--
 * LF: --**
 * LM: **--
 * LR: --**
 */
static Gait TRIPOD = Gait("tripod", 2, 2, {0, 2, 0, 2, 0, 2}, 100);

/**
 * Slow Tripod Gait:
 * RF: **********----------
 * RM: ----------**********
 * RR: **********----------
 * LF: ----------**********
 * LM: **********----------
 * LR: ----------**********
 */
static Gait TRIPOD_SLOW = Gait("tripod slow", 10, 10, {0, 10, 0, 10, 0, 10}, 50);


GaitSequencer::GaitSequencer() {
  numGaits = 7;
  gaits = (Gait**) calloc(numGaits, sizeof(void*));
  gaits[0] = &WAVE;
  gaits[1] = &WAVE_SLOW;
  gaits[2] = &WAVE_SYNC;
  gaits[3] = &TRIPOD;
  gaits[4] = &TRIPOD_SLOW;
  gaits[5] = &RIPPLE;
  gaits[6] = &RIPPLE_SLOW;
  gait = gaits[0];
  curGait = 0;
}

void GaitSequencer::Select(int nr) {
  if (nr < 0 || nr >= numGaits) {
    printf("Error: invalid gait type: %d. Choose a value from 0-%d", nr, numGaits - 1);
    return;
  }
  curGait = nr;
  gait = gaits[nr];
  step = 0;
  printf("Gait [%d]: %s (%d steps)\n", curGait, gait->name, gait->len);
}

void GaitSequencer::Step(Vec3f *v) {
  bool moveCheck = ((std::abs(v->x) > movementThreshold) ||
                    (std::abs(v->y) > movementThreshold) ||
                    (std::abs(v->z) > movementThreshold));

  //Clear values under the movements threshold
  if (!moveCheck) {
    return;
  }

  printf("Gait step: %d. tx:%.2f ty:%.2f tz:%.2f\n", step, v->x, v->y, v->z);
  for (int i = 0; i < gait->len; i++) {
    printf("%f ", gait->seq[(step + i) % gait->len].z);
  }
  printf("\n");

  for (int i = 0; i < 6; i++) {
    Vec3f *p = &gait->seq[(gait->phase[i] + step) % gait->len];
    pos[i].x = v->x * p->x;
    pos[i].y = v->y * p->y;
    pos[i].z = legLiftHeight * p->z;
    rot[i] = v->z * p->x; // we modulate the z-rotation with the same as the x/y gait
    printf("  leg %d. p:%d x:%.2f y:%.2f z:%.2f, r:%.2f\n", i, gait->phase[i], pos[i].x, pos[i].y, pos[i].z, rot[i]);
  }
  step = (step + 1) % gait->len;
}
