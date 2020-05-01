#include <iostream>
#include <sstream>
#include <unistd.h>
#include "PS2X_lib.h"

using namespace std;

int main(){
  SPIDevice spi(1,1);
  spi.setSpeed(100000);
  spi.setMode(SPIDevice::MODE3);
  spi.setLSBFirst(1);
  spi.debugDump();

  PS2X ps2x(&spi);

  ps2x.SetADMode(true, false);

  ps2x.Poll();
  ps2x.GetKeyState();
  ps2x.dump();

  ps2x.Poll();
  ps2x.GetKeyState();
  ps2x.dump();

  ps2x.SetEnableMotor(false, true);
  ps2x.SetMotorLevel(0, 255);
  while (true) {
    ps2x.Poll();
    ps2x.GetKeyState();
    ps2x.dump();
    usleep(100*1000);
    ps2x.SetMotorLevel(0, 0);
  }
//  cout << "Init ps2 controller: \n";
//  int psError = ps2x.config_gamepad(false, false);
//  cout << (psError ? "error" : "ok") << endl;
//  switch(ps2x.readType()) {
//    case 1:
//      cout << "DualShock Controller found" << endl;
//      break;
//    case 2:
//      cout << "GuitarHero Controller found" << endl;
//      break;
//    case 3:
//      cout << "Wireless Sony DualShock Controller found" << endl;
//      break;
//    default:
//      cout << "Unknown Controller type found" << endl;
//      break;
//  }
}
