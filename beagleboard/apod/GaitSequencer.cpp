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

#define  cRR      0
#define  cRM      1
#define  cRF      2
#define  cLR      3
#define  cLM      4
#define  cLF      5

#define cTravelDeadZone 4.0f // threshold above which a movement is required.

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
  float dy = 2.0f / (float) rs;
  for (int i = 0 ; i < rs; i++) {
    this->seq[i].x = 0.0f;
    this->seq[i].y = -1.0f + (float) i*dy;
    this->seq[i].z = 0.0f;
  }
  // power stroke
  dy = 2.0f / (float) ps;
  for (int i = rs ; i < rs; i++) {
    this->seq[i].x = 0.0f;
    this->seq[i].y = -1.0f + (float) i*dy;
    this->seq[i].z = 0.0f;
  }

}

Gait::~Gait() {
  free(this->seq);
  this->seq = nullptr;
}

/**
 * Fast Wave Gait:
 * RF: **----
 * RM: -**---
 * RR: --**--
 * LF: ---**-
 * LM: ----**
 * LR: *----*
 */
static Gait WAVE_6 =
    Gait("fast wave", 6,
         (float[]) {
             0,   0,   0,   0,    0,  0.0,
             0, 1.0, 0.5, 0.0, -0.5, -1.0,
             1,   0,   0,   0,    0,    0
         },
         {
             0, 1, 2, 3, 4, 5
         },
         200);

/**
 * Slow Wave Gait:
 * RF: **----------
 * RM: --**--------
 * RR: ----**------
 * LF: ------**----
 * LM: --------**--
 * LR: ----------**
 */
static Gait WAVE_12 =
    Gait("slow wave", 12,
         (float[]) {
               0, 0, 0.0, 0.0, 0.0, 0.0, 0.0,  0.0, 0.0,   0.0, 0.0, 0.0,
               0, 1, 0.8, 0.6, 0.4, 0.2, 0.0, -0.2, -0.4, -0.6, -0.8, -1,
               1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         },
         {
             0, 2, 4, 6, 8, 10
         },
         200);

/**
 * Ripple Gait:
 * RF: **----
 * RM: --**--
 * RR: ----**
 * LF: -**---
 * LM: --**--
 * LR: ---**-
 */
static Gait RIPPLE =
    Gait("ripple", 6,
         (float[]) {
               0.0, 0.0, 0.0, 0.0,  0.0,  0.0,
               0.0, 1.0, 0.5, 0.0, -0.5, -1.0,
                 1,   0,   0,   0,    0,    0,
         },
         {
             0, 2, 4, 1, 3, 5
         },
         200);

/**
 * Slow Tripod Gait:
 * RF: ****----
 * RM: ----****
 * RR: ****----
 * LF: ----****
 * LM: ****----
 * LR: ----****
 */
static Gait TRIPOD =
    Gait("slow tripod", 8,
         (float[]) {
              0.0,  0.0, 0.0, 0.0,  0.0, 0.0,  0.0,  0.0,
             -1.0, -0.5, 0.0, 0.5,  1.0, 0.5,  0.0, -0.5,
              0.0,  0.5, 1.0, 0.5,  0.0, 0.0,  0.0,  0.0
         },
         {
             0, 4, 0, 4, 0, 4
         },
         200);

/**
 * Fast Tripod Gait:
 * RF: **--
 * RM: --**
 * RR: **--
 * LF: --**
 * LM: **--
 * LR: --**
 */
static Gait TRIPOD_FAST =
    Gait("fast tripod", 4,
         (float[]) {
              0.0, 0.0, 0.0, 0.0,
              0.0, 1.0, 0.0,-1.0,
              1.0, 0.0, 0.0, 0.0,
         },
         {
             0, 2, 0, 2, 0, 2
         },
         200);


/*
 ╭───────╮
 │5     2│
 │4     1│
 │3     0│
 ╰───────╯
 */
GaitSequencer::GaitSequencer() {
  numGaits = 5;
  gaits = (Gait**) calloc(numGaits, sizeof(void*));
  gaits[0] = &WAVE_6;
  gaits[1] = &WAVE_12;
  gaits[2] = &TRIPOD_FAST;
  gaits[3] = &TRIPOD;
  gaits[4] = &RIPPLE;
  gait = gaits[0];
}

void GaitSequencer::Select(int nr) {
  if (nr < 0 || nr >= numGaits) {
    printf("Error: invalid gait type: %d. Choose a value from 0-%d", nr, numGaits - 1);
    return;
  }
  gait = gaits[nr];
  step = 0;
  printf("Gait: %s (%d steps)\n", gait->name, gait->len);
  return;

  //Gait selector
  switch (nr) {
    case 0:
      // ripple Gait 12 steps
      printf("Gait: ripple 12\n");
      GaitLegNr[cLR] = 1;
      GaitLegNr[cRF] = 3;
      GaitLegNr[cLM] = 5;
      GaitLegNr[cRR] = 7;
      GaitLegNr[cLF] = 9;
      GaitLegNr[cRM] = 11;

      NrLiftedPos = 3;
      HalfLiftHeight = 3;
      TLDivFactor = 8;
      StepsInGait = 12;
      NomGaitSpeed = 70;
      break;
    case 1:
      printf("Gait: tripod 8\n");
      // Tripod 8 steps
      GaitLegNr[cLR] = 5;
      GaitLegNr[cRF] = 1;
      GaitLegNr[cLM] = 1;
      GaitLegNr[cRR] = 1;
      GaitLegNr[cLF] = 5;
      GaitLegNr[cRM] = 5;

      NrLiftedPos = 3;
      HalfLiftHeight = 3;
      TLDivFactor = 4;
      StepsInGait = 8;
      NomGaitSpeed = 70;
      break;
    case 2:
      printf("Gait: tripod 12\n");
      // triple Tripod 12 step
      GaitLegNr[cRF] = 3;
      GaitLegNr[cLM] = 4;
      GaitLegNr[cRR] = 5;
      GaitLegNr[cLF] = 9;
      GaitLegNr[cRM] = 10;
      GaitLegNr[cLR] = 11;

      NrLiftedPos = 3;
      HalfLiftHeight = 3;
      TLDivFactor = 8;
      StepsInGait = 12;
      NomGaitSpeed = 60;
      break;
    case 3:
      printf("Gait: tripod 16/5\n");
      // Triple Tripod 16 steps, use 5 lifted positions
      GaitLegNr[cRF] = 4;
      GaitLegNr[cLM] = 5;
      GaitLegNr[cRR] = 6;
      GaitLegNr[cLF] = 12;
      GaitLegNr[cRM] = 13;
      GaitLegNr[cLR] = 14;

      NrLiftedPos = 5;
      HalfLiftHeight = 1;
      TLDivFactor = 10;
      StepsInGait = 16;
      NomGaitSpeed = 60;
      break;
    case 4:
      printf("Gait: wave\n");
      //Wave 24 steps
      GaitLegNr[cLR] = 1;
      GaitLegNr[cRF] = 21;
      GaitLegNr[cLM] = 5;

      GaitLegNr[cRR] = 13;
      GaitLegNr[cLF] = 9;
      GaitLegNr[cRM] = 17;

      NrLiftedPos = 3;
      HalfLiftHeight = 3;
      TLDivFactor = 20;
      StepsInGait = 24;
      NomGaitSpeed = 70;
      break;
    case 5:
      printf("Gait: test\n");
      // test 12 steps
      GaitLegNr[cRR] = 1;
      GaitLegNr[cRM] = 2;
      GaitLegNr[cRF] = 3;

      GaitLegNr[cLF] = 4;
      GaitLegNr[cLM] = 5;
      GaitLegNr[cLR] = 6;

      NrLiftedPos = 1;
      HalfLiftHeight = 0;
      TLDivFactor = 20;
      StepsInGait = 6;
      NomGaitSpeed = 70;

      StepsInGait = 8;

      break;
  }
  step = 0;
}

void GaitSequencer::Seq(Vec3f *travel) {
  //Check if the Gait is in motion
  TravelRequest = ((std::abs(travel->x) > cTravelDeadZone) ||
                   (std::abs(travel->y) > cTravelDeadZone) ||
                   (std::abs(travel->z) > cTravelDeadZone));

  //Clear values under the cTravelDeadZone
  if (!TravelRequest) {
    travel->x = 0;
    travel->y = 0;
    travel->z = 0;
  }

  printf("Gait step: %d. tx:%.2f ty:%.2f tz:%.2f\n", step, travel->x, travel->y, travel->z);
  for (int i = 0; i < gait->len; i++) {
    printf("%f ", gait->seq[(step + i) % gait->len].z);
  }
  printf("\n");

  if (NrLiftedPos == 5) {
    LiftDivFactor = 4;
  } else {
    LiftDivFactor = 2;
  }

  //Calculate Gait sequence
  for (int idx = 0; idx < 6; idx++) {
    Step2(travel, idx);
    printf("  leg %d. p:%d x:%.2f y:%.2f z:%.2f, r:%.2f\n", idx, gait->phase[idx], GaitPosX[idx], GaitPosY[idx], GaitPosZ[idx], GaitRotZ[idx]);

  }
  step = (step + 1) % gait->len;
}

void GaitSequencer::Step2(Vec3f *travel, int idx) {
  Vec3f *p = &gait->seq[(gait->phase[idx] + step) % gait->len];
  GaitPosX[idx] = travel->x * p->x;
  GaitPosZ[idx] = LegLiftHeight * p->z;
  GaitPosY[idx] = travel->y * p->y;
  GaitRotZ[idx] = 0;
}

void GaitSequencer::Step(Vec3f *travel, int idx) {
  //Leg middle up position
  //Gait in motion														  									Gait NOT in motion, return to home position
  if ((TravelRequest && (NrLiftedPos == 1 || NrLiftedPos == 3 || NrLiftedPos == 5) &&
       step == GaitLegNr[idx]) ||
      (!TravelRequest && step == GaitLegNr[idx] && ((std::abs(GaitPosX[idx]) > 2) ||
                                                    (std::abs(GaitPosY[idx]) > 2) ||
                                                    (std::abs(GaitPosZ[idx]) > 2)))) { //Up
    GaitPosX[idx] = 0;
    GaitPosZ[idx] = LegLiftHeight;
    GaitPosY[idx] = 0;
    GaitRotZ[idx] = 0;
  }
    //Optional Half height Rear (2, 3, 5 lifted positions)
  else if (((NrLiftedPos == 2 && step == GaitLegNr[idx]) || (NrLiftedPos >= 3 &&
                                                             (step ==
                                                                  GaitLegNr[idx] - 1 ||
                                                              step == GaitLegNr[idx] +
                                                                      (StepsInGait - 1))))
           && TravelRequest) {
    GaitPosX[idx] = -travel->x / LiftDivFactor;
    GaitPosZ[idx] = 3 * LegLiftHeight / (3 + HalfLiftHeight);     //Easier to shift between div factor: /1 (3/3), /2 (3/6) and 3/4
    GaitPosY[idx] = -travel->y / LiftDivFactor;
    GaitRotZ[idx] = -travel->z / LiftDivFactor;
  }

    // Optional Half height front (2, 3, 5 lifted positions)
  else if ((NrLiftedPos >= 2) && (step == GaitLegNr[idx] + 1 ||
                                  step == GaitLegNr[idx] - (StepsInGait - 1)) && TravelRequest) {
    GaitPosX[idx] = travel->x / LiftDivFactor;
    GaitPosZ[idx] = 3 * LegLiftHeight / (3 + HalfLiftHeight); // Easier to shift between div factor: /1 (3/3), /2 (3/6) and 3/4
    GaitPosY[idx] = travel->y / LiftDivFactor;
    GaitRotZ[idx] = travel->z / LiftDivFactor;
  }

    //Optional Half height Rear 5 LiftedPos (5 lifted positions)
  else if (((NrLiftedPos == 5 && (step == GaitLegNr[idx] - 2))) && TravelRequest) {
    GaitPosX[idx] = -travel->x / 2;
    GaitPosZ[idx] = LegLiftHeight / 2;
    GaitPosY[idx] = -travel->y / 2;
    GaitRotZ[idx] = -travel->z / 2;
  }

    //Optional Half height Front 5 LiftedPos (5 lifted positions)
  else if ((NrLiftedPos == 5) && (step == GaitLegNr[idx] + 2 ||
                                  step == GaitLegNr[idx] - (StepsInGait - 2)) && TravelRequest) {
    GaitPosX[idx] = travel->x / 2;
    GaitPosZ[idx] = LegLiftHeight / 2;
    GaitPosY[idx] = travel->y / 2;
    GaitRotZ[idx] = travel->z / 2;
  }

    //Leg front down position
  else if ((step == GaitLegNr[idx] + NrLiftedPos ||
            step == GaitLegNr[idx] - (StepsInGait - NrLiftedPos))
           && GaitPosZ[idx] < 0) {
    GaitPosX[idx] = travel->x / 2;
    GaitPosY[idx] = travel->y / 2;
    GaitRotZ[idx] = travel->z / 2;
    GaitPosZ[idx] = 0;  //Only move leg down at once if terrain adaption is turned off
  }

    //Move body forward
  else {
    GaitPosX[idx] = GaitPosX[idx] - (travel->x / TLDivFactor);
    GaitPosZ[idx] = 0;
    GaitPosY[idx] = GaitPosY[idx] - (travel->y / TLDivFactor);
    GaitRotZ[idx] = GaitRotZ[idx] - (travel->z / TLDivFactor);
  }


}