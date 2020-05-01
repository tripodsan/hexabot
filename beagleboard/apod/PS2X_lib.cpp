#include "PS2X_lib.h"
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <string.h>

using namespace std;

const uint8_t PSX_CMD_INIT_PRESSURE[]	= {0x01, 0x40, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00};
const uint8_t PSX_CMD_POLL[]		= {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t PSX_CMD_ENTER_CFG[]	= {0x01, 0x43, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t PSX_CMD_EXIT_CFG[]	= {0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
const uint8_t PSX_CMD_ENABLE_MOTOR[]	= {0x01, 0x4D, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t PSX_CMD_ALL_PRESSURE[]	= {0x01, 0x4F, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00};
const uint8_t PSX_CMD_AD_MODE[]		= {0x01, 0x44, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

PS2X::PS2X(SPIDevice *device) {
  this->device = device;
  memset(&this->pad, 0, sizeof(PSXPad_t));
  memcpy(&pad.cmdPoll, PSX_CMD_POLL, sizeof(PSX_CMD_POLL));
  memcpy(&pad.cmdADMode, PSX_CMD_AD_MODE, sizeof(PSX_CMD_AD_MODE));
  memcpy(&pad.cmdEnableMotor, PSX_CMD_ENABLE_MOTOR, sizeof(PSX_CMD_ENABLE_MOTOR));
}

void PS2X::SetADMode(const bool analog, const bool lock) {
  pad.analog = analog;
  pad.lock = lock;

  pad.cmdADMode[3] = analog ? 0x01 : 0x00;
  pad.cmdADMode[4] = lock ? 0x03 : 0x00;

  command(PSX_CMD_ENTER_CFG, sizeof(PSX_CMD_ENTER_CFG));
  command(pad.cmdADMode, sizeof(PSX_CMD_AD_MODE));
  command(PSX_CMD_INIT_PRESSURE, sizeof(PSX_CMD_INIT_PRESSURE));
  command(PSX_CMD_ALL_PRESSURE, sizeof(PSX_CMD_ALL_PRESSURE));
  command(PSX_CMD_EXIT_CFG, sizeof(PSX_CMD_EXIT_CFG));
}

void PS2X::SetEnableMotor(const bool motor1, const bool motor2) {
  pad.motor1Enable = motor1;
  pad.motor2Enable = motor2;

  pad.cmdEnableMotor[3] = motor1 ? 0x00 : 0xFF;
  pad.cmdEnableMotor[4] = motor2 ? 0x01 : 0xFF;

  command(PSX_CMD_ENTER_CFG, sizeof(PSX_CMD_ENTER_CFG));
  command(pad.cmdEnableMotor, sizeof(PSX_CMD_ENABLE_MOTOR));
  command(PSX_CMD_EXIT_CFG, sizeof(PSX_CMD_EXIT_CFG));
}

void PS2X::SetMotorLevel(const uint8_t motor1Level, const uint8_t motor2Level) {
  pad.motor1Level = motor1Level;
  pad.motor2Level = motor2Level;
  pad.cmdPoll[3] = motor1Level ? 0xFF : 0x00;
  pad.cmdPoll[4] = motor2Level;
}

void PS2X::Poll() {
  command(pad.cmdPoll, sizeof(PSX_CMD_POLL));
}

void PS2X::GetKeyState() {
  memset(&state, 0, sizeof(PSXPad_KeyState_t));
  state.type = PSXPAD_KEYSTATE_UNKNOWN;
  switch(pad.cmdResponse[1]) {
    case 0x79:
      state.type = PSXPAD_KEYSTATE_ANALOG2;
      state.padARight = pad.cmdResponse[ 9];
      state.padALeft  = pad.cmdResponse[10];
      state.padAUp    = pad.cmdResponse[11];
      state.padADown  = pad.cmdResponse[12];
      state.btnATri   = pad.cmdResponse[13];
      state.btnACir   = pad.cmdResponse[14];
      state.btnACrs   = pad.cmdResponse[15];
      state.btnASqr   = pad.cmdResponse[16];
      state.btnAL1    = pad.cmdResponse[17];
      state.btnAR1    = pad.cmdResponse[18];
      state.btnAL2    = pad.cmdResponse[19];
      state.btnAR2    = pad.cmdResponse[20];
      // no break
    case 0x73:
      if(state.type == PSXPAD_KEYSTATE_UNKNOWN) {
        state.type = PSXPAD_KEYSTATE_ANALOG1;
      }
      state.joyRX = pad.cmdResponse[5];
      state.joyRY = pad.cmdResponse[6];
      state.joyLX = pad.cmdResponse[7];
      state.joyLY = pad.cmdResponse[8];
      state.joyL3 = (pad.cmdResponse[3] & 0x02U) ? 0 : 1;
      state.joyR3 = (pad.cmdResponse[3] & 0x04U) ? 0 : 1;
      // no break
    case 0x41:
      if(state.type == PSXPAD_KEYSTATE_UNKNOWN) {
        state.type = PSXPAD_KEYSTATE_DIGITAL;
      }
      state.btnSel   = (pad.cmdResponse[3] & 0x01U) ? 0 : 1;
      state.btnStt   = (pad.cmdResponse[3] & 0x08U) ? 0 : 1;
      state.padUp    = (pad.cmdResponse[3] & 0x10U) ? 0 : 1;
      state.padRight = (pad.cmdResponse[3] & 0x20U) ? 0 : 1;
      state.padDown  = (pad.cmdResponse[3] & 0x40U) ? 0 : 1;
      state.padLeft  = (pad.cmdResponse[3] & 0x80U) ? 0 : 1;
      state.btnL2    = (pad.cmdResponse[4] & 0x01U) ? 0 : 1;
      state.btnR2    = (pad.cmdResponse[4] & 0x02U) ? 0 : 1;
      state.btnL1    = (pad.cmdResponse[4] & 0x04U) ? 0 : 1;
      state.btnR1    = (pad.cmdResponse[4] & 0x08U) ? 0 : 1;
      state.btnTri   = (pad.cmdResponse[4] & 0x10U) ? 0 : 1;
      state.btnCir   = (pad.cmdResponse[4] & 0x20U) ? 0 : 1;
      state.btnCrs   = (pad.cmdResponse[4] & 0x40U) ? 0 : 1;
      state.btnSqr   = (pad.cmdResponse[4] & 0x80U) ? 0 : 1;
  }
}

void PS2X::command(const uint8_t cmd[], const uint8_t len) {
  int sent = device->transfer(cmd, pad.cmdResponse, len);
  if (sent < 0) {
    return;
  }
  std::cout << "i/o(" << sent << ")\n>> ";
  for (int ret = 0; ret < len; ret++) {
    printf("%.2X ", cmd[ret]);
  }
  std::cout << "\n<< ";
  for (int ret = 0; ret < len; ret++) {
    printf("%.2X ", pad.cmdResponse[ret]);
  }
  std::cout << "\n";
}

void PS2X::dump() {
  cout << " U  D  L  R  Tri Sqr Crs Cir  L1 L2 L3 R1 R2 R3  Sel Stt  LX,LY  RX,RY \n";
  printf("%2d %2d %2d %2d  ", state.padUp, state.padDown, state.padLeft, state.padRight);
  printf("%3d %3d %3d %3d  ", state.btnTri, state.btnSqr, state.btnCrs, state.btnCir);
  printf("%2d %2d %2d ", state.btnL1, state.btnL2, state.joyL3);
  printf("%2d %2d %2d  ", state.btnR1, state.btnR2, state.joyR3);
  printf("%3d %3d  ", state.btnSel, state.btnStt);
  printf("%2x,%2x  ", state.joyLX, state.joyLY);
  printf("%2x,%2x  ", state.joyRX, state.joyRY);
  printf("\n");
  printf("%2x %2x %2x %2x  ", state.padAUp, state.padADown, state.padALeft, state.padARight);
  printf("%3x %3x %3x %3x  ", state.btnATri, state.btnASqr, state.btnACrs, state.btnACir);
  printf("%2x %2x    ", state.btnAL1, state.btnAL2);
  printf("%2x %2x     ", state.btnAR1, state.btnAR2);
  printf("\n");
}
