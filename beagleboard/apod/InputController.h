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

#ifndef APOD_INPUTCONTROLLER_H
#define APOD_INPUTCONTROLLER_H

#include "globals.h"
#include "PS2X_lib.h"
#include "GaitSequencer.h"

enum class ControllerMode {
  WALK,
  TRANSLATE,
  TILT,
  SINGLE
};

class InputController {
public:
  /**
   * current mode
   */
  ControllerMode mode;

  /**
   * power on/off
   */
  bool powerOn;

  /**
   * Body position (shift)
   */
  Vec3f bodyPos;

  /**
   * Body position (x: pitch, y: roll, z: rotation)
   */
  Vec3f bodyRot;

  /**
   * Movement _velocity_
   */
  Vec3f v;

  /**
   * Leg lift height
   */
  float legLiftHeight;

  /**
   * Head control (x: pitch, y: roll, z: rotation)
   */
  Vec3f headPos;

  /**
   * Tail control (x: pitch, z: rotation)
   */
  Vec3f tailPos;

  /**
   * Mandible control
   */
  float mandibleAngle;

  /**
   * selected leg (single leg mode only)
   */
  int selectedLeg;

  /**
   * leg position (single leg mode only)
   */
  Vec3f legPos;

  /**
   * Balance mode (on / off)
   */
  bool balanceMode;

  /**
   * Speed...
   * todo: currently time of 1 gait sequence step. but need better definition
   */
  int speed;

  InputController(PS2X *ps2, GaitSequencer *gait);

  void poll();

  void dump();

  void reset();

private:
  PS2X *ps2;

  GaitSequencer* gait;

  void openMandibles(float angle);

  void changeSpeed(int d);
};


#endif //APOD_INPUTCONTROLLER_H
