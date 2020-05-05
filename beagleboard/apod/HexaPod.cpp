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

void HexaPod::IK() {
  // a=\arctan\left(P.y,P.x\right)-\arctan\left(k_{2},k_{1}\right)

  // k_{1}=l_{1}+l_{2}\cdot\cos\left(b\right)

  //      x^2 + y^2 - l1^2 - l2^2
  // d = ------------------------
  //           2 * l1 * l2
  //
  // b = +- acos(d);
  // k1 = l1 + l2 * d
  // k2 = l2 * sqrt(1-d*d)

  float l1 = cXXFemurLength;
  float l2 = cXXTibiaLength;

  // adjust for body dimensions
  float x = leg.x - cRROffsetX;
  float y = leg.y - cRROffsetY;
  float z = leg.z;

  // first rotate x/y for coxa angle
  float c = atan2(y, x) - DEG2RAD(cRRCoxaAngle1);

  // the x for the x/z IK is the length to x/y, minus the coxa length
  x = sqrt(x*x + y*y) - cXXCoxaLength;

  float d = (x*x + z*z - l1*l1 - l2*l2) / (2.0f * l1 * l2);
  float b1 = acos(d);
  float b2 = -b1;

  float k1 = l1 + l2 * d;
  float k2 = l2 * sqrt(1 - d * d);
  float ah2 = atan2(k2, k1);
  float ah1 = atan2(z, x);

  float a1 = ah2 - ah1;
  float a2 = ah2 + ah1;

  b1 = - b1 - M_PI_2;
  b2 = - b2 - M_PI_2;

  leg.ac = std::min(std::max(RAD2DEG(c), cRRCoxaMin1), cRRCoxaMax1);;
  leg.af = std::min(std::max(RAD2DEG(a2), cRRFemurMin1), cRRFemurMax1);
  leg.at = std::min(std::max(RAD2DEG(b2), cRRTibiaMin1), cRRTibiaMax1);
  printf("x:%.2f, y:%.2f, z:%.2f, c:%.2f, a1:%.2f, b1:%.2f, a2:%.2f, b2:%.2f\n",
      leg.x, leg.y, leg.z, RAD2DEG(c), RAD2DEG(a1), RAD2DEG(b1), RAD2DEG(a2), RAD2DEG(b2));
}

HexaPod::HexaPod() {
  leg.x = (cXXFemurLength+cXXCoxaLength)*M_SQRT1_2 + cRROffsetX;
  leg.y = (cXXFemurLength+cXXCoxaLength)*M_SQRT1_2 + cRROffsetY;
  leg.z = -cXXTibiaLength;
}
