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
   * Creates a gait pattern. The `rs` defines the number of steps a leg does a _return stroke_
   * (leg lifted, moving forward).
   * The `ps` defines the number of steps a leg does a _power stroke_ (leg on the ground,
   * pushing backwards).
   *
   * The phase array defines the phase of each leg in respect to the sequence steps. the leg
   * numbering is as follows:
   *
   *  ╭───^───╮
   *  │5     2│
   *  │4     1│
   *  │3     0│
   *  ╰───^───╯
   *
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
  GaitSequencer();

  /**
   * Select gait.
   * @param nr
   */
  void Select(int nr);

  /**
   * Calculate next gait step. The `v` is a not very accurate velocity vector.
   * It more or less specifies the distance the leg should travel within one
   * gait sequence cycle.
   *
   * @param v Movement vector (note the v.z is used for z-rotation during walking)
   */
  void Step(Vec3f *v);

  /**
   * Number of available gaits
   */
  int numGaits;

  /**
   * Current gait nr
   */
  int curGait;

  /**
   * List of gaits
   */
  Gait** gaits;

  /**
   * Current gait
   */
  Gait* gait;

  /**
   * Leg height when lifted (input)
   */
  float legLiftHeight;

  /**
   * Current gait step
   */
  int step;

  /**
   * Calculated gait positions of the legs
   */
  Vec3f pos[6];
  float rot[6];

};


#endif //APOD_GAITSEQUENCER_H
