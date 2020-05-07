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
#include <cmath>
#include <cstdio>
#include "HexaPod.h"
#include "ssc_config.h"

#define RAD2DEG(a) ((a) * 180.0f / (float) M_PI)
#define DEG2RAD(a) ((a) * (float) M_PI / 180.0f)

const static float LEG_START_X[] = { cRRInitPosX, cRMInitPosX, cRFInitPosX, cLRInitPosX, cLMInitPosX, cLFInitPosX };
const static float LEG_START_Y[] = { cRRInitPosY, cRMInitPosY, cRFInitPosY, cLRInitPosY, cLMInitPosY, cLFInitPosY };
const static float LEG_START_Z[] = { cRRInitPosZ, cRMInitPosZ, cRFInitPosZ, cLRInitPosZ, cLMInitPosZ, cLFInitPosZ };

const static float LEG_OFFSET_X[] = { cRROffsetX, cRMOffsetX, cRFOffsetX, cLROffsetX, cLMOffsetX, cLFOffsetX };
const static float LEG_OFFSET_Y[] = { cRROffsetY, cRMOffsetY, cRFOffsetY, cLROffsetY, cLMOffsetY, cLFOffsetY };
const static float LEG_OFFSET_A[] = { cRRCoxaAngle1, cRMCoxaAngle1, cRFCoxaAngle1, cLRCoxaAngle1, cLMCoxaAngle1, cLFCoxaAngle1 };

const static float MIN_ANGLE_COXA[] = { cRRCoxaMin1, cRMCoxaMin1, cRFCoxaMin1, cLRCoxaMin1, cLMCoxaMin1, cLFCoxaMin1 };
const static float MAX_ANGLE_COXA[] = { cRRCoxaMax1, cRMCoxaMax1, cRFCoxaMax1, cLRCoxaMax1, cLMCoxaMax1, cLFCoxaMax1 };
const static float MIN_ANGLE_FEMUR[] = { cRRFemurMin1, cRMFemurMin1, cRFFemurMin1, cLRFemurMin1, cLMFemurMin1, cLFFemurMin1 };
const static float MAX_ANGLE_FEMUR[] = { cRRFemurMax1, cRMFemurMax1, cRFFemurMax1, cLRFemurMax1, cLMFemurMax1, cLFFemurMax1 };
const static float MIN_ANGLE_TIBIA[] = { cRRTibiaMin1, cRMTibiaMin1, cRFTibiaMin1, cLRTibiaMin1, cLMTibiaMin1, cLFTibiaMin1 };
const static float MAX_ANGLE_TIBIA[] = { cRRTibiaMax1, cRMTibiaMax1, cRFTibiaMax1, cLRTibiaMax1, cLMTibiaMax1, cLFTibiaMax1 };

HexLeg::HexLeg(const int idx) :
  idx(idx),
  dx(LEG_OFFSET_X[idx]), dy(LEG_OFFSET_Y[idx]), dz(0.0f), acoxa(DEG2RAD(LEG_OFFSET_A[idx])),
  cmin(DEG2RAD(MIN_ANGLE_COXA[idx])), cmax(DEG2RAD(MAX_ANGLE_COXA[idx])),
  fmin(DEG2RAD(MIN_ANGLE_FEMUR[idx])), fmax(DEG2RAD(MAX_ANGLE_FEMUR[idx])),
  tmin(DEG2RAD(MIN_ANGLE_TIBIA[idx])), tmax(DEG2RAD(MAX_ANGLE_TIBIA[idx])) {
}

void HexLeg::Reset() {
  x = LEG_START_X[idx];
  y = LEG_START_Y[idx];
  z = LEG_START_Z[idx];
  IK();
}

void HexLeg::IK() {
  //      x^2 + y^2 - l1^2 - l2^2
  // d = ------------------------
  //           2 * l1 * l2
  //
  // b = +- acos(d);
  // k1 = l1 + l2 * d
  // k2 = l2 * sqrt(1-d*d) = l2 * sin(b)
  // a = arctan(y,x) - arctan(k2,k1)

  #define L1 cXXFemurLength
  #define L2 cXXTibiaLength

  // adjust for body dimensions
  float cx = x - dx;
  float cy = y - dy;
  float cz = z;

  // flip x for left legs
  if (idx >= 3) {
    cx = -cx;
  }

  // first rotate x/y for coxa angle
  float c = atan2f(cy, cx) - acoxa;
  ac = RAD2DEG(std::min(std::max(c, cmin), cmax));

  // the x for the x/z IK is the length to x/y, minus the coxa length
  cx = sqrtf(cx * cx + cy * cy) - cXXCoxaLength;

  float h2 = cx * cx + cz * cz;
  float d = (h2 - L1*L1 - L2*L2) / (2.0f * L1 * L2);

  // point is out of reach, limit d.
  if (d > 0.7f) {
    d = 0.7f;
  } else if (d < -0.7f) {
    d = -0.7f;
  }

  float k1 = L1 + L2 * d;
  float k2 = L2 * sqrtf(1 - d * d);

  float a = atan2f(k2, k1) + atan2f(cz, cx);
  float b = acosf(d) - (float) M_PI_2;

  af = RAD2DEG(std::min(std::max(a, fmin), fmax));
  at = RAD2DEG(std::min(std::max(b, tmin), tmax));

//  printf("x:%.2f, y:%.2f, z:%.2f, c:%.2f, a:%.2f, b:%.2f, c:%.2f, d:%.4f\n",
//         x, y, z, RAD2DEG(c), RAD2DEG(a), RAD2DEG(b), RAD2DEG(c), d);
}

void HexLeg::PowerOff() {
  ac = 0;
  af = RAD2DEG(fmax);
  at = RAD2DEG(tmax);
}

HexaPod::HexaPod() {
  Reset();
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