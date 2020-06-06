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

#ifndef APOD_HEXLEG_H
#define APOD_HEXLEG_H

#include "globals.h"

class HexLeg {
public:
  explicit HexLeg(int idx);

public:
  const int idx;
  const float ox, oy, oz; // coxa position from body center
  const float cmin, cmax, fmin, fmax, tmin, tmax; // min/max angles for servos
  const float acoxa; // coxa angle

  float ix, iy, iz;  // (initial) foot position relative to body center. only modified in single-leg mode.
  float ac; // coxa angle, calculated in IK
  float af; // femur angle, calculated in IK
  float at; // tibia angle, calculated in IK

  void Reset();
  void IK(float x, float y, float z);
  void PowerOff();
};

#endif //APOD_HEXLEG_H
