//==============================================================================
// InputController.h - This is is the class definition for the abstraction of
//     which input controller is used to control the Hex robot.  There will be
//     several implementations of this class, which include:
//         PS2 - 
//         XBEE
//         Serial
//         Autonomous
//==============================================================================
#ifndef _INPUT_CONTROLLER_h_
#define _INPUT_CONTROLLER_h_

#include "config.h"  // make sure we know what options are enabled...

#include <Arduino.h>

#define cTravelDeadZone 4      //The deadzone for the analog input from the remote
#define isTravel (abs(g_InControlState.TravelLength.x) > cTravelDeadZone || \
                  abs(g_InControlState.TravelLength.z) > cTravelDeadZone || \
                  abs(g_InControlState.TravelLength.y * 2) > cTravelDeadZone)


class InputController {
public:
  void Init();

  void ControlInput();

  void Reset();
};

typedef struct _Coord3D {
  long x;
  long y;
  long z;
} COORD3D;


//==============================================================================
// class ControlState: This is the main structure of data that the Control 
//      manipulates and is used by the main Phoenix Code to make it do what is
//      requested.
//==============================================================================
typedef struct _InControlState {
  boolean fHexOn;        //Switch to turn on Phoenix
  boolean fPrev_HexOn;      //Previous loop state
//Body position
  COORD3D BodyPos;

//Body Inverse Kinematics
  COORD3D BodyRot1;               // X -Pitch, Y-Rotation, Z-Roll

//[gait]
  byte GaitType;      //Gait type

  short LegLiftHeight;    //Current Travel height
  COORD3D TravelLength;            // X-Z or Length, Y is rotation.

// Head Control
  short HeadAnglePan;
  short HeadAngleTilt;
  short HeadAngleRot;

// Tail Control
  short TailAnglePan;
  short TailAngleTilt;

  // Mandible control
  short MandibleAngle;

  //[Single Leg Control]
  byte SelectedLeg;
  COORD3D SLLeg;                //
  boolean fSLHold;      //Single leg control mode


//[Balance]
  boolean BalanceMode;

//[TIMING]
  byte InputTimeDelay;  //Delay that depends on the input to get the "sneaking" effect
  word SpeedControl;  //Adjustible Delay
} INCONTROLSTATE;


#endif //_INPUT_CONTROLLER_h_
