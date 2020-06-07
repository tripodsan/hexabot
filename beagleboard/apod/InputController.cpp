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

#include <cstring>
#include "InputController.h"
#include "ssc_config.h"

static const std::string CONTROLLER_MODE_NAMES[] = {"Walk", "Translate", "Tilt", "Single Leg"};

void add_clamped(float *v, float d, float min, float max) {
  *v = std::min(std::max(*v + d, min),max);
}

InputController::InputController(PS2X *ps2, GaitSequencer *gait) : ps2(ps2), gait(gait) {
  powerOn = false;
  reset();
}

void InputController::reset() {
  speed = 50;
  balanceMode = false;
  selectedLeg = 0;
  mandibleAngle = 0;
  legLiftHeight = 40;
  mode = ControllerMode::WALK;
  memset(&headPos, 0, sizeof(Vec3f));
  memset(&tailPos, 0, sizeof(Vec3f));
  memset(&bodyPos, 0, sizeof(Vec3f));
  memset(&bodyRot, 0, sizeof(Vec3f));
  memset(&legPos, 0, sizeof(Vec3f));
  memset(&v, 0, sizeof(Vec3f));
}

/**
 * Polls the PS2 controller and updates the states
 */
void InputController::poll() {
  ps2->Poll();
  ps2->GetKeyState();
  if (ps2->ButtonPressed(PSB_START)) {
    powerOn = !powerOn;
  }
  if (!powerOn) {
    return;
  }

  // mandibles
  if (ps2->state.btnL1) {
    openMandibles(wMandibleInc);
  }
  if (ps2->state.btnL2) {
    openMandibles(-wMandibleInc);
  }

  // switch modes
  if (ps2->ButtonPressed(PSB_CROSS)) {
    if (mode != ControllerMode::SINGLE) {
      mode = ControllerMode{(static_cast<int>(mode) + 1) % 4};
    }
    printf("mode: %s\n", CONTROLLER_MODE_NAMES[static_cast<int>(mode)].c_str());
  }

  // single leg mode
  if (ps2->ButtonPressed(PSB_CIRCLE)) {
    if (mode != ControllerMode::SINGLE) {
      mode = ControllerMode::SINGLE;
      selectedLeg = 0;
    } else {
      mode = ControllerMode::WALK;
    }
    printf("mode: %s\n", CONTROLLER_MODE_NAMES[static_cast<int>(mode)].c_str());
  }

  // Switch Balance mode on/off
  if (ps2->ButtonPressed(PSB_SQUARE)) {
    balanceMode = !balanceMode;
    printf("balance mode: %d\n", balanceMode);
  }

  // Stand up, sit down
  if (ps2->ButtonPressed(PSB_TRIANGLE)) {
    if (bodyPos.z != 0) {
      bodyPos.z = 0;
    } else {
      // todo: define sitting position
      bodyPos.z = 35;
    }
  }

  // speed control
  if (ps2->state.padUp) {
    add_clamped(&speed, 5, 10.0f, 150.0f);
  }
  if (ps2->state.padDown) {
    add_clamped(&speed, -5, 10.0f, 150.0f);
  }

  // head control
  if (mode == ControllerMode::WALK && ps2->state.btnR1) {
    if (ps2->state.btnL3) {
      headPos.x = 0; headPos.y = 0; headPos.z = 0;
    } else {
      add_clamped(&headPos.x, -ps2->state.joyLXf * 5, cHeadPanMin1, cHeadPanMax1);
      add_clamped(&headPos.y, -ps2->state.joyLYf * 5, cHeadTiltMin1, cHeadTiltMax1);
      add_clamped(&headPos.z, -ps2->state.joyRXf * 5, cHeadRotMin1, cHeadRotMin1);
    }
  }
  // tail control
  else if (mode == ControllerMode::WALK && ps2->state.btnR2) {
    if (ps2->state.btnL3) {
      tailPos.x = 0;tailPos.y = 0;
    } else {
      add_clamped(&tailPos.x, ps2->state.joyLXf * 5, cTailPanMin1, cTailPanMax1);
      add_clamped(&tailPos.y, ps2->state.joyLYf * 5, cTailTiltMin1, cTailTiltMin1);
    }
  }

  // switch gaits
  else if (mode == ControllerMode::WALK && ps2->ButtonPressed(PSB_SELECT)) {
    // todo: only allow switch gait when stopped ?
    gait->Select((gait->curGait + 1) % gait->numGaits);
  }

  // walking
  else if (mode == ControllerMode::WALK) {
    v.x = -ps2->state.joyLXf * speed;
    v.y = -ps2->state.joyLYf * speed;
    v.z = ps2->state.joyRXf * 10; // rotation around z
    add_clamped(&bodyPos.z, ps2->state.joyRYf * 10, -50.0f, 50.0f);
  }

  // translate mode
  if (mode == ControllerMode::TRANSLATE) {
    if (ps2->state.btnL3 || ps2->state.btnR3) {
      bodyPos.x = 0; bodyPos.y = 0;bodyPos.z = 0;
      bodyRot.x = 0; bodyRot.y = 0;bodyRot.z = 0;
    } else {
      add_clamped(&bodyPos.x, -ps2->state.joyLXf * 5, -50.0f, 50.0f);
      add_clamped(&bodyPos.y,  ps2->state.joyLYf * 5, -50.0f, 50.0f);
      add_clamped(&bodyPos.z,  ps2->state.joyRYf * 5, -50.0f, 70.0f);
      add_clamped(&bodyRot.z,  ps2->state.joyRXf * 2, -20.0f, 20.0f);
    }
  }

  // tilt mode
  if (mode == ControllerMode::TILT) {
    if (ps2->state.btnL3 || ps2->state.btnR3) {
      bodyPos.x = 0; bodyPos.y = 0;bodyPos.z = 0;
      bodyRot.x = 0; bodyRot.y = 0;bodyRot.z = 0;
    } else {
      add_clamped(&bodyRot.x, -ps2->state.joyLXf * 2, -20.0f, 20.0f);
      add_clamped(&bodyRot.y, -ps2->state.joyLYf * 2, -20.0f, 20.0f);
      add_clamped(&bodyRot.z, ps2->state.joyRXf * 2, -20.0f, 20.0f);
      add_clamped(&bodyPos.z, ps2->state.joyRYf * 5, -50.0f, 70.0f);
    }
  }

  // single leg
  if (mode == ControllerMode::SINGLE) {
    if (ps2->state.btnSel) {
      selectedLeg = (selectedLeg + 1) % 6;
      printf("selected leg: %d\n", selectedLeg);
    }
    legPos.x += ps2->state.joyLXf * 2.0f;
    legPos.y -= ps2->state.joyLYf * 2.0f;
    legPos.z -= ps2->state.joyRYf * 2.0f;
  }
}

void InputController::openMandibles(float angle) {
  mandibleAngle = std::min(std::max(mandibleAngle + angle, cMandibleMin1), cMandibleMax1);
  printf("mandible angle: %.2f\n", mandibleAngle);
}

void InputController::dump() {
  printf("\033[2J\033[H");
  printf("-----------------------------\n");
  printf("   Power: %d\n", powerOn);
  printf("    gait: %s\n", gait->gait->name);
  printf("    Mode: %s\n", CONTROLLER_MODE_NAMES[static_cast<int>(mode)].c_str());
  printf("bal mode: %d\n", balanceMode);
  printf("velocity: x:%.2f, y:%.2f, z:%.2f\n", v.x, v.y, v.z);
  printf("body pos: x:%.2f, y:%.2f, z:%.2f\n", bodyPos.x, bodyPos.y, bodyPos.z);
  printf("body rot: x:%.2f, y:%.2f, z:%.2f\n", bodyRot.x, bodyRot.y, bodyRot.z);
  printf("  leg ht: %.2f\n", gait->legLiftHeight);
  printf("head pos: x:%.2f, y:%.2f, z:%.2f\n", headPos.x, headPos.y, headPos.z);
  printf("tail pos: x:%.2f, y:%.2f, z:%.2f\n", tailPos.x, tailPos.y, tailPos.z);
  printf("    mand: %.2f\n", mandibleAngle);
  printf(" sel leg: %d\n", selectedLeg);
  printf(" leg pos: x:%.2f, y:%.2f, z:%.2f\n", legPos.x, legPos.y, legPos.z);
  printf("   speed: %0.2f\n", speed);
}

