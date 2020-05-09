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

#ifndef APOD_GAITSEQUENCER_H
#define APOD_GAITSEQUENCER_H

#include <cstdlib>
#include <cstdint>
#include "globals.h"

class Gait {
public:
  Gait(const char *name, int len,  float* seq, const int (&phase)[6], uint16_t stepTime);

  /**
   * Creates a gait pattern.
   * @param name Name of the pattern
   * @param rs Number of return stroke steps
   * @param ps Number of power stroke steps
   * @param phase Phase array for the legs
   * @param stepTime Servo step time.
   */
  Gait(const char *name, int rs, int ps, const int (&phase)[6], uint16_t stepTime);

  ~Gait();

  const char* name;
  int phase[6];
  int len;
  Vec3f* seq;
  uint8_t stepTime;
};

class GaitSequencer {
public:
  size_t numGaits;
  Gait** gaits;
  Gait* gait;

  GaitSequencer();

  float LegLiftHeight; // input set by user





  short NomGaitSpeed;    //Nominal speed of the gait
  short TLDivFactor;         //Number of steps that a leg is on the floor while walking
  short NrLiftedPos;         //Number of positions that a single leg is lifted [1-3]
  float LiftDivFactor;       //Normaly: 2, when NrLiftedPos=5: 4

  float HalfLiftHeight;      //If TRUE the outer positions of the ligted legs will be half height

  bool TravelRequest;        //Temp to check if the gait is in motion
  int StepsInGait;         //Number of steps in gait

  int step;            //Actual Gait step

  int GaitLegNr[6];        //Init position of the leg

  int GaitLegNrIn;         //Input Number of the leg

  float GaitPosX[6];         //Array containing Relative X position corresponding to the Gait
  float GaitPosY[6];         //Array containing Relative Y position corresponding to the Gait
  float GaitPosZ[6];         //Array containing Relative Z position corresponding to the Gait
  float GaitRotZ[6];         //Array containing Relative Z rotation corresponding to the Gait

  void Select(int nr);

  void Seq(Vec3f *travel);

  void Step(Vec3f *travel, int idx);
  void Step2(Vec3f *travel, int idx);
};


#endif //APOD_GAITSEQUENCER_H
