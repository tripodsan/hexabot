#include <Arduino.h>
#include "config.h"
#include "PS2Support.h"

//#ifdef USEPS2

void PS2Init() {
  Serial.print("init ps2 controller...");
  Serial.flush();
  delay(500);
  int psError = PS2X::Instance()->config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT);
  Serial.println(psError ? "error" : "ok");
  Serial.flush();
}

//#endif //USEPS2


