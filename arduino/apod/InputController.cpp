#include <Arduino.h>
#include "globals.h"
#include "sound.h"

#ifdef USEPS2

#include "PS2Support.h"

#define WALKMODE          0
#define TRANSLATEMODE     1
#define TILTMODE          2
#define NUM_MODES         3

#define SINGLELEGMODE     3

static const char *MODE_NAMES[] = {"Walk", "Translate", "Tilt", "Single Leg"};

#define  MAXPS2ERRORCNT  5     // How many times through the loop will we go before shutting off robot?

//=============================================================================
// Global - Local to this file only...
//=============================================================================

// Define an Instance of the Input Controller...
InputController g_InputController;       // Our Input controller

static short g_BodyYOffset;
static short g_sPS2ErrorCnt;
static short g_BodyYShift;
static byte ControlMode;
static byte PrevControlMode;
static bool DoubleHeightOn;
static bool DoubleTravelOn;

void PS2TurnRobotOff();

void InputController::Init() {
  g_BodyYOffset = 65;
  g_BodyYShift = 0;
  g_sPS2ErrorCnt = 0;  // error count

  ControlMode = WALKMODE;
  PrevControlMode = WALKMODE;
  DoubleHeightOn = false;
  DoubleTravelOn = false;

  g_InControlState.SpeedControl = 100;
}

void openMandibles(short angle1) {
  g_InControlState.MandibleAngle = min(max(g_InControlState.MandibleAngle + angle1, cMandibleMin1), cMandibleMax1);
  DBGSerial.print("mandibles: ");
  DBGSerial.println(g_InControlState.MandibleAngle);
}

void InputController::ControlInput() {
  if (ps2x.read_gamepad()) {
    // In an analog mode so should be OK...
    g_sPS2ErrorCnt = 0;    // clear out error count...

    if (ps2x.ButtonPressed(PSB_START)) {// OK lets try "0" button for Start.
      if (g_InControlState.fHexOn) {
        PS2TurnRobotOff();
      } else {
        //Turn on
        g_InControlState.fHexOn = true;
      }
    }

    if (g_InControlState.fHexOn) {
      // mandibles
      if (ps2x.Button(PSB_L1)) {
        openMandibles(wMandibleInc);
      }
      if (ps2x.Button(PSB_L2)) {
        openMandibles(-wMandibleInc);
      }

      // switch modes
      if (ps2x.ButtonPressed(PSB_CROSS)) {
        if (ControlMode != SINGLELEGMODE) {
          ControlMode = (ControlMode + 1) % NUM_MODES;
          if (ControlMode == WALKMODE) {
            tone(SOUND_PIN, 333, 200);
            delay(400);
          }
          tone(SOUND_PIN, 333, 200);
        }
      }

      // single leg mode
      if (ps2x.ButtonPressed(PSB_CIRCLE)) {
        if (!isTravel) {
          tone(SOUND_PIN, 333, 200);
          delay(400);
          if (ControlMode != SINGLELEGMODE) {
            ControlMode = SINGLELEGMODE;
            if (g_InControlState.SelectedLeg == 255) {
              g_InControlState.SelectedLeg = cRF;
            }
          } else {
            ControlMode = WALKMODE;
            g_InControlState.SelectedLeg = 255;
            tone(SOUND_PIN, 333, 200);
          }
        }
      }

      // Switch Balance mode on/off
      if (ps2x.ButtonPressed(PSB_SQUARE)) {
        tone(SOUND_PIN, 555, 200);
        delay(400);
        g_InControlState.BalanceMode = !g_InControlState.BalanceMode;
        if (!g_InControlState.BalanceMode) {
          tone(SOUND_PIN, 555, 200);
        }
        DBGSerial.print("balance mode: ");
        DBGSerial.println(g_InControlState.BalanceMode);
      }

      // Stand up, sit down
      if (ps2x.ButtonPressed(PSB_TRIANGLE)) {
        if (g_BodyYOffset) {
          g_BodyYOffset = 0;
        } else {
          g_BodyYOffset = 35;
        }
      }

      // speed control
      if (ps2x.ButtonPressed(PSB_PAD_RIGHT)) {
        if (g_InControlState.SpeedControl > 0) {
          g_InControlState.SpeedControl = g_InControlState.SpeedControl - 50;
          sound_effect(SOUND_EFFECT_SPEED_DOWN);
        }
      }
      if (ps2x.ButtonPressed(PSB_PAD_LEFT)) {
        if (g_InControlState.SpeedControl < 2000) {
          g_InControlState.SpeedControl = g_InControlState.SpeedControl + 50;
          sound_effect(SOUND_EFFECT_SPEED_UP);
        }
      }

      // walk functions
      if (ControlMode == WALKMODE && ps2x.Button(PSB_R1)) {
        if (ps2x.ButtonPressed(PSB_L3)) {
          g_InControlState.HeadAnglePan = 0;
          g_InControlState.HeadAngleTilt = 0;
          g_InControlState.HeadAngleRot = 0;
        } else {
          g_InControlState.HeadAnglePan -= (ps2x.Analog(PSS_LX) - 127) / 4;
          g_InControlState.HeadAnglePan = min(max(g_InControlState.HeadAnglePan, cHeadPanMin1), cHeadPanMax1);

          g_InControlState.HeadAngleTilt -= (ps2x.Analog(PSS_LY) - 127) / 4;
          g_InControlState.HeadAngleTilt = min(max(g_InControlState.HeadAngleTilt, cHeadTiltMin1), cHeadTiltMax1);

          g_InControlState.HeadAngleRot -= (ps2x.Analog(PSS_RX) - 127) / 4;
          g_InControlState.HeadAngleRot = min(max(g_InControlState.HeadAngleRot, cHeadRotMin1), cHeadRotMax1);
        }
      }
      else if (ControlMode == WALKMODE && ps2x.Button(PSB_R2)) {
        if (ps2x.ButtonPressed(PSB_L3)) {
          g_InControlState.TailAnglePan = 0;
          g_InControlState.TailAngleTilt = 0;
        } else {
          g_InControlState.TailAnglePan -= (ps2x.Analog(PSS_LX) - 127) / 4;
          g_InControlState.TailAnglePan = min(max(g_InControlState.TailAnglePan, cTailPanMin1), cTailPanMax1);

          g_InControlState.TailAngleTilt -= (ps2x.Analog(PSS_LY) - 127) / 4;
          g_InControlState.TailAngleTilt = min(max(g_InControlState.TailAngleTilt, cTailTiltMin1), cTailTiltMax1);
        }
      }
      else if (ControlMode == WALKMODE) {
        // switch gates
        if (ps2x.ButtonPressed(PSB_SELECT) && !isTravel) {
          g_InControlState.GaitType = (g_InControlState.GaitType + 1) % NUM_GAITS;
          if (g_InControlState.GaitType == 0) {
            tone(SOUND_PIN, 666, 200);
            delay(400);
          }
          tone(SOUND_PIN, 666, 200);
          GaitSelect();
        }

        //Double leg lift height
//        if (ps2x.ButtonPressed(PSB_R1)) { // R1 Button Test
//          tone(SOUND_PIN, 666, 200);
//          DoubleHeightOn = !DoubleHeightOn;
//          if (DoubleHeightOn)
//            g_InControlState.LegLiftHeight = 80;
//          else
//            g_InControlState.LegLiftHeight = 50;
//        }
//
//        //Double Travel Length
//        if (ps2x.ButtonPressed(PSB_R2)) {// R2 Button Test
//          tone(SOUND_PIN, 666, 200);
//          DoubleTravelOn = !DoubleTravelOn;
//        }

        //Walking
        g_InControlState.TravelLength.x = -(ps2x.Analog(PSS_LX) - 128);
        g_InControlState.TravelLength.z = (ps2x.Analog(PSS_LY) - 128);

        g_BodyYOffset += (ps2x.Analog(PSS_RY) - 127) / 3;
        g_BodyYOffset = min(max(g_BodyYOffset, 0), 100);

        if (!DoubleTravelOn) {
          g_InControlState.TravelLength.x = g_InControlState.TravelLength.x / 2;
          g_InControlState.TravelLength.z = g_InControlState.TravelLength.z / 2;
        }
        g_InControlState.TravelLength.y = -(ps2x.Analog(PSS_RX) - 128) / 4;
      }

      // translate mode
      g_BodyYShift = 0;
      if (ControlMode == TRANSLATEMODE) {
        g_InControlState.BodyPos.x = (ps2x.Analog(PSS_LX) - 128) / 2;
        g_InControlState.BodyPos.z = -(ps2x.Analog(PSS_LY) - 128) / 3;
        g_InControlState.BodyRot1.y = (ps2x.Analog(PSS_RX) - 128) * 2;
        g_BodyYShift = (-(ps2x.Analog(PSS_RY) - 128) / 2);
      }

      // rotate mode
      if (ControlMode == TILTMODE) {
        g_InControlState.BodyRot1.x = (ps2x.Analog(PSS_LY) - 128);
        g_InControlState.BodyRot1.y = (ps2x.Analog(PSS_RX) - 128) * 2;
        g_InControlState.BodyRot1.z = (ps2x.Analog(PSS_LX) - 128);
        g_BodyYShift = (-(ps2x.Analog(PSS_RY) - 128) / 2);
      }

      // single leg
      if (ControlMode == SINGLELEGMODE) {
        if (ps2x.ButtonPressed(PSB_SELECT)) {
          g_InControlState.SelectedLeg = (g_InControlState.SelectedLeg + 1) % 6;
          if (g_InControlState.SelectedLeg == 0) {
            tone(SOUND_PIN, 666, 200);
            delay(400);
          }
          tone(SOUND_PIN, 666, 200);
          DBGSerial.print("Selected leg: ");
          DBGSerial.println(g_InControlState.SelectedLeg);
        }
        g_InControlState.SLLeg.x = (ps2x.Analog(PSS_LX) - 128) / 2;
        g_InControlState.SLLeg.y = (ps2x.Analog(PSS_RY) - 128) / 10;
        g_InControlState.SLLeg.z = (ps2x.Analog(PSS_LY) - 128) / 2;

        // Hold single leg in place
        if (ps2x.ButtonPressed(PSB_R2)) { // R2 Button Test
          tone(SOUND_PIN, 777, 500);
          g_InControlState.fSLHold = !g_InControlState.fSLHold;
        }
      }

      //Calculate walking time delay
      g_InControlState.InputTimeDelay = 128 - max(max(abs(ps2x.Analog(PSS_LX) - 128), abs(ps2x.Analog(PSS_LY) - 128)),
                                                  abs(ps2x.Analog(PSS_RX) - 128));
    }

    if (PrevControlMode != ControlMode) {
      DBGSerial.print("control mode: ");
      DBGSerial.println(MODE_NAMES[ControlMode]);
      PrevControlMode = ControlMode;
    }

    //Calculate g_InControlState.BodyPos.y
    g_InControlState.BodyPos.y = max(g_BodyYOffset + g_BodyYShift, 0);
  } else {
    // We may have lost the PS2... See what we can do to recover...
    if (g_sPS2ErrorCnt < MAXPS2ERRORCNT) {
      g_sPS2ErrorCnt++;
    } else if (g_InControlState.fHexOn) {
      PS2TurnRobotOff();
    }
  }
}

//==============================================================================
// PS2TurnRobotOff - code used couple of places so save a little room...
//==============================================================================
void PS2TurnRobotOff() {
  //Turn off
  g_InControlState.BodyPos.x = 0;
  g_InControlState.BodyPos.y = 0;
  g_InControlState.BodyPos.z = 0;
  g_InControlState.BodyRot1.x = 0;
  g_InControlState.BodyRot1.y = 0;
  g_InControlState.BodyRot1.z = 0;
  g_InControlState.TravelLength.x = 0;
  g_InControlState.TravelLength.z = 0;
  g_InControlState.TravelLength.y = 0;
  g_InControlState.MandibleAngle = 0;
  g_InControlState.HeadAnglePan = 0;
  g_InControlState.HeadAngleTilt = 0;
  g_InControlState.HeadAngleRot = 0;
  g_InControlState.TailAnglePan = 0;
  g_InControlState.TailAngleTilt = 0;
  g_BodyYOffset = 0;
  g_BodyYShift = 0;
  g_InControlState.SelectedLeg = 255;
  g_InControlState.fHexOn = false;
}


#endif //USEPS2


