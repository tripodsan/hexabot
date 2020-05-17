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

HexaPod::HexaPod() {
  Reset();
  gait.Select(0);
  gait.legLiftHeight = 60;
}

void HexaPod::Step(InputController* ctrl) {
  gait.Step(&ctrl->v);

  for (int i = 0; i < 6; i++) {
    legs[i].dx = gait.pos[i].x;
    legs[i].dy = gait.pos[i].y;
    legs[i].dz = gait.pos[i].z;
    legs[i].IK();
  }

//  LegIK(LegPosX[LegIndex] - g_InControlState.BodyPos.x + BodyFKPosX - (GaitPosX[LegIndex] - TotalTransX),
//        LegPosY[LegIndex] + g_InControlState.BodyPos.y - BodyFKPosY + GaitPosY[LegIndex] - TotalTransY,
//        LegPosZ[LegIndex] + g_InControlState.BodyPos.z - BodyFKPosZ + GaitPosZ[LegIndex] - TotalTransZ, LegIndex);

}

void HexaPod::IK() {
  for (auto & leg : legs) {
    leg.IK();
  }
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