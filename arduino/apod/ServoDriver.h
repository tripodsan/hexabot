//==============================================================================
// ServoDriver.h  This header file defines the ServoDriver class.
// 
// This class is used by the main Phoenix code to talk to the servos, without having
// to know details about how the servos are connected.  There may be several implementations
// of this class, such as one will use the SSC-32 to control the servos.  There may be 
// additional versions of this class that allows the main processor to control the servos.
//==============================================================================
#ifndef _Servo_Driver_h_
#define _Servo_Driver_h_

#include "Hex_Cfg.h"  // make sure we know what options are enabled...
#include <Arduino.h>

class ServoDriver {
public:
  void Init();

  void BeginServoUpdate();

  void OutputServoInfoForLeg(byte LegIndex, short sCoxaAngle1, short sFemurAngle1, short sTibiaAngle1);

  void OutputServoInfoHead(short pan, short tilt, short rot);

  void OutputServoInfoTail(short pan, short tilt);

  void OutputServoInfoMandibles(short left, short right);

  void CommitServoDriver(word wMoveTime);

  void FreeServos();

#ifdef OPT_FIND_SERVO_OFFSETS
  // Needs to be different depending on which driver
  void FindServoOffsets();
#endif

#ifdef OPT_SSC_FORWARDER
  // Optional code used to forward commands from debug terminal to SSC prot...
  void SSCForwarder();
#endif
};

#endif //_Servo_Driver_h_
