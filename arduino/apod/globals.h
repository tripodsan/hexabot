//==============================================================================
// GLOBALS - The main global definitions for the CPhenix program - still needs
//		to be cleaned up.
// This program assumes that the main files were compiled as C files
//==============================================================================
#ifndef _HEX_GLOBALS_H_
#define _HEX_GLOBALS_H_

#include <SoftwareSerial.h>
#include "config.h"
#include "ServoDriver.h"
#include "InputController.h"

//=============================================================================
//[CONSTANTS]
//=============================================================================
#define BUTTON_DOWN 0
#define BUTTON_UP  1

#define  c1DEC    10
#define  c2DEC    100
#define  c4DEC    10000
#define  c6DEC    1000000

#define  cRR      0
#define  cRM      1
#define  cRF      2
#define  cLR      3
#define  cLM      4
#define  cLF      5

#define  WTIMERTICSPERMSMUL    64  // BAP28 is 16mhz need a multiplyer and divider to make the conversion with /8192
#define WTIMERTICSPERMSDIV    125 //
#define USEINT_TIMERAV

#define SERVO_MAND_IDX (NUMSERVOSPERLEG * 6)
#define SERVO_HEAD_IDX (NUMSERVOSPERLEG * 6 + 2)
#define SERVO_TAIL_IDX (NUMSERVOSPERLEG * 6 + 5)

const static uint8_t ALL_SERVOS_PINS[] = {
    cRRCoxaPin, cRRFemurPin, cRRTibiaPin, cRMCoxaPin, cRMFemurPin, cRMTibiaPin, cRFCoxaPin, cRFFemurPin, cRFTibiaPin,
    cLRCoxaPin, cLRFemurPin, cLRTibiaPin, cLMCoxaPin, cLMFemurPin, cLMTibiaPin, cLFCoxaPin, cLFFemurPin, cLFTibiaPin,
    cLMandPin, cRMandPin, cHeadPanPin, cHeadTiltPin, cHeadRotPin, cTailPanPin, cTailTiltPin
};

const static char *ALL_SERVOS_NAMES[] = {
    "RR  Coxa", "RR Femur", "RR Tibia",
    "RM  Coxa", "RM Femur", "RM Tibia",
    "RF  Coxa", "RF Femur", "RF Tibia",
    "LR  Coxa", "LR Femur", "LR Tibia",
    "LM  Coxa", "LM Femur", "LM Tibia",
    "LF  Coxa", "LF Femur", "LF Tibia",
    "HD LMand", "HD RMand",
    "HD  Pan ", "HD  Tilt", "HD  Rot ",
    "TL  Pan ", "TL  Tilt",
};

#define NUM_GAITS    5

extern void GaitSelect();

extern short SmoothControl(short CtrlMoveInp, short CtrlMoveOut, byte CtrlDivider);


//-----------------------------------------------------------------------------
// Define global class objects
//-----------------------------------------------------------------------------
extern ServoDriver g_ServoDriver;           // our global servo driver class
#ifdef USEPS2
extern InputController g_InputController;       // Our Input controller
extern INCONTROLSTATE g_InControlState;     // State information that controller changes
#endif

//-----------------------------------------------------------------------------
// Define Global variables
//-----------------------------------------------------------------------------
extern boolean g_fDebugOutput;

#if 0
extern boolean		g_fHexOn;				//Switch to turn on Phoenix
extern boolean		g_fPrev_HexOn;			//Previous loop state 
//Body position
extern long		BodyPosX; 		//Global Input for the position of the body
extern long		BodyPosY; 
extern long		BodyPosZ; 

//Body Inverse Kinematics
extern long		BodyRotX1; 		//Global Input pitch of the body
extern long		BodyRotY1;		//Global Input rotation of the body
extern long		BodyRotZ1; 		//Global Input roll of the body


//[gait]
extern byte		GaitType;			//Gait type
extern short		NomGaitSpeed;		//Nominal speed of the gait

extern short		LegLiftHeight;		//Current Travel height
extern long		TravelLengthX;		//Current Travel length X
extern long		TravelLengthZ; 		//Current Travel length Z
extern long		TravelRotationY; 	//Current Travel Rotation Y

//[Single Leg Control]
extern byte		SelectedLeg;
extern short		SLLegX;
extern short		SLLegY;
extern short		SLLegZ;
extern boolean		fSLHold;		 	//Single leg control mode


//[Balance]
extern boolean BalanceMode;

//[TIMING]
extern byte			InputTimeDelay;	//Delay that depends on the input to get the "sneaking" effect
extern word			SpeedControl;	//Adjustible Delay
#endif


extern void MSound(uint8_t _pin, byte cNotes, ...);
//extern int DBGPrintf(const char *format, ...);
//extern int SSCPrintf(const char *format, ...);


// The defined controller must provide the following
extern void InitController();

extern void ControlInput();

extern void AllowControllerInterrupts(boolean fAllow);


extern bool CheckVoltage();

// debug handler...
extern boolean g_fDBGHandleError;

#ifdef __AVR__
#if not defined(UBRR1H)
//extern NewSoftSerial SSCSerial;
extern SoftwareSerial SSCSerial;
#endif
#endif
#endif


