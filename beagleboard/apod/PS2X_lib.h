#ifndef PS2X_lib_h
#define PS2X_lib_h

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "bus/SPIDevice.h"

#define PSB_SELECT      0x0001
#define PSB_L3          0x0002
#define PSB_R3          0x0004
#define PSB_START       0x0008
#define PSB_PAD_UP      0x0010
#define PSB_PAD_RIGHT   0x0020
#define PSB_PAD_DOWN    0x0040
#define PSB_PAD_LEFT    0x0080
#define PSB_L2          0x0100
#define PSB_R2          0x0200
#define PSB_L1          0x0400
#define PSB_R1          0x0800
#define PSB_GREEN       0x1000
#define PSB_RED         0x2000
#define PSB_BLUE        0x4000
#define PSB_PINK        0x8000
#define PSB_TRIANGLE    0x1000
#define PSB_CIRCLE      0x2000
#define PSB_CROSS       0x4000
#define PSB_SQUARE      0x8000

typedef struct PSXPad {
  uint8_t cmdPoll[64];
  uint8_t cmdResponse[64];
  uint8_t cmdEnableMotor[64];
  uint8_t cmdADMode[64];
  uint8_t analog;
  uint8_t lock;
  uint8_t motor1Enable;
  uint8_t motor2Enable;
  uint8_t motor1Level;
  uint8_t motor2Level;
} PSXPad_t;

typedef enum {
  PSXPAD_KEYSTATE_DIGITAL = 0,
  PSXPAD_KEYSTATE_ANALOG1,
  PSXPAD_KEYSTATE_ANALOG2,
  PSXPAD_KEYSTATE_UNKNOWN
} PSXPad_KeyStateType_t;

typedef struct PSXPad_KeyState {
  PSXPad_KeyStateType_t type;
  /* PSXPAD_KEYSTATE_DIGITAL */
  uint16_t buttons;
  uint8_t btnSel;
  uint8_t btnStt;
  uint8_t padUp;
  uint8_t padRight;
  uint8_t padDown;
  uint8_t padLeft;
  uint8_t btnL1;
  uint8_t btnR1;
  uint8_t btnL2;
  uint8_t btnR2;
  uint8_t btnL3;
  uint8_t btnR3;
  uint8_t btnTri;
  uint8_t btnCir;
  uint8_t btnCrs;
  uint8_t btnSqr;
  /* PSXPAD_KEYSTATE_ANALOG1 */
  uint8_t joyRX;
  uint8_t joyRY;
  uint8_t joyLX;
  uint8_t joyLY;
  /* PSXPAD_KEYSTATE_ANALOG2 */
  uint8_t padARight;
  uint8_t padALeft;
  uint8_t padAUp;
  uint8_t padADown;
  uint8_t btnATri;
  uint8_t btnACir;
  uint8_t btnACrs;
  uint8_t btnASqr;
  uint8_t btnAL1;
  uint8_t btnAR1;
  uint8_t btnAL2;
  uint8_t btnAR2;
} PSXPad_KeyState_t;

class PS2X {
public:
  PS2X(SPIDevice *device);

  void SetADMode(bool analog, bool lock);

  void SetEnableMotor(bool motor1, bool motor2);

  void SetMotorLevel(uint8_t motor1Level, uint8_t motor2Level);

  void Poll();

  void GetKeyState();

  void dump();

  PSXPad_KeyState_t state;

private:
  PSXPad_t pad;

  void command(const uint8_t cmd[], uint8_t len);

  SPIDevice *device;
};

#endif


