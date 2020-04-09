#include <Arduino.h>
#include "config.h"
#include "PS2Support.h"

#ifdef USEPS2

void PS2Init() {
  DBGSerial.print(F("Init ps2 controller: "));
  int psError = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT);
  DBGSerial.println(psError ? "error" : "ok");
  switch(ps2x.readType()) {
    case 1:
      DBGSerial.println(F("DualShock Controller found"));
      break;
    case 2:
      DBGSerial.println(F("GuitarHero Controller found"));
      break;
    case 3:
      DBGSerial.println(F("Wireless Sony DualShock Controller found"));
      break;
    default:
      DBGSerial.println(F("Unknown Controller type found"));
      break;
  }
  DBGSerial.flush();
}

#endif //USEPS2


