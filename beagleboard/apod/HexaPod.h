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

#ifndef APOD_HEXAPOD_H
#define APOD_HEXAPOD_H


class HexLeg {
public:
  HexLeg(const int idx);

public:
  const int idx;
  const float dx, dy, dz; // position from body center
  const float cmin, cmax, fmin, fmax, tmin, tmax; // min/max angles for servos
  const float acoxa; // coxa angle

  float x, y, z;  // foot position
  float ac; // coxa angle
  float af; // femur angle
  float at; // tibia angle

  void Reset();
  void IK();

};

class HexaPod {
public:
  HexaPod();

public:
  void Reset();
  void IK();
  HexLeg legs[6]{HexLeg(0), HexLeg(1), HexLeg(2), HexLeg(3), HexLeg(4), HexLeg(5)};
};


#endif //APOD_HEXAPOD_H
