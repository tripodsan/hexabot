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
#include "HexaPod.h"
#include "InputController.h"

#define RAD2DEG(a) ((a) * 180.0f / (float) M_PI)
#define DEG2RAD(a) ((a) * (float) M_PI / 180.0f)

HexaPod::HexaPod() {
  Reset();
  gait.Select(0);
  gait.legLiftHeight = 60;
}

int HexaPod::Step(InputController* ctrl) {
  int time = gait.Step(&ctrl->v);

  for (int i = 0; i < 6; i++) {
    // position of foot relative to the body center
    const float ix = legs[i].ix;
    const float iy = legs[i].iy;
    const float iz = legs[i].iz;

    // rotate around body
    const float ca = cosf(DEG2RAD(ctrl->bodyRot.z + gait.rot[i])); // alpha (yaw)
    const float sa = sinf(DEG2RAD(ctrl->bodyRot.z + gait.rot[i]));
    const float cb = cosf(DEG2RAD(ctrl->bodyRot.x)); // beta (pitch)
    const float sb = sinf(DEG2RAD(ctrl->bodyRot.x));
    const float cg = cosf(DEG2RAD(ctrl->bodyRot.y)); // gamma (roll)
    const float sg = sinf(DEG2RAD(ctrl->bodyRot.y));

    float x = ctrl->bodyPos.x + ca*cb*ix + (ca*sb*sg-sa*cg)*iy + (ca*sb*cg+sa*sg)*iz;
    float y = ctrl->bodyPos.y + sa*cb*ix + (sa*sb*sg+ca*cg)*iy + (sa*sb*cg-ca*sg)*iz;
    float z = ctrl->bodyPos.z - sb*ix + cb*sg*iy + cb*cg*iz;

    legs[i].IK(
        x + gait.pos[i].x,
        y + gait.pos[i].y,
        z + gait.pos[i].z
    );
  }

  return time;
}

void HexaPod::Reset() {
  for (auto & leg : legs) {
    leg.Reset();
  }
}

void HexaPod::PowerOff() {
  for (auto & leg : legs) {
    leg.PowerOff();
  }
}