//====================================================================
// Project Lynxmotion A-POD
// Description:
//    This is the hardware configuration file for the Hex Robot.
//  
//    This version of the Configuration file is set up to run on the
//    Lynxmotion BotboardDuino board, which is similiar to the Arduino Duemilanove
//
//    This version of configuration file assumes that the servos will be controlled
//    by a Lynxmotion Servo controller SSC-32 and the user is using a Lynxmotion 
//    PS2 to control the robot.
//
// Programmer: Kurt Eckhardt (aka KurtE)
//            tripod
//
// NEW IN V2.0 (2020-04-05)
//   - Support for Mandibles
// NEW IN V1.1 (2013-05-17)
//   - Support for Arduino Pro Mini on Bot Board (originally for Basic Atom Pro)
// NEW IN V1.0
//   - First Release
//
//====================================================================

//[CONDITIONAL COMPILING] - COMMENT IF NOT WANTED
// Define other optional components to be included or not...

//comment if terminal monitor is not required
#define OPT_TERMINAL_MONITOR  

#ifdef OPT_TERMINAL_MONITOR       // turning off terminal monitor will turn these off as well...
  #define OPT_SSC_FORWARDER       // only useful if terminal monitor is enabled
  #define OPT_FIND_SERVO_OFFSETS  // Only useful if terminal monitor is enabled
#endif

// Which type of control(s) do you want to compile in
#define DBGSerial         Serial

#if defined(UBRR1H)
#define SSCSerial         Serial1
#else
#endif

#define USEPS2

//[SERIAL CONNECTIONS]

// Warning I will undefine some components as the non-megas don't have enough memory...
//#undef OPT_FIND_SERVO_OFFSETS 

#define cSSC_BAUD   115200   //SSC32 BAUD rate

#define HAS_GRIP_SENSOR

//--------------------------------------------------------------------
// [BotBoarduino]
#define SOUND_PIN       5  // Botboarduino JR pin number
#define GRIP_PIN        2  // Analog pin for gripper pressure sensor
#define RSV_1           3  // used by tone()
#define RSV_1          11  // used by tone()
#define PS2_ATT         6
#define PS2_CMD         7
#define PS2_DAT         8
#define PS2_CLK         9
#define cSSC_OUT       12  // Output pin for Botboard - Input of SSC32 (RX red)
#define cSSC_IN        13  // Input pin for Botboard - Output of SSC32 (TX yellow)

#define cSSC_ADC_VOLT  'A' // Voltage ADC Pin
#define fTurnOffVol     5.0f
#define fTurnOnVol      6.0f
#define fMaxVol         10.0f // sometimes the sensor is wrong...!?!

//====================================================================
//[SSC PIN NUMBERS]
#define cRRCoxaPin       2  // Rear Right leg Hip Horizontal
#define cRRFemurPin      1  // Rear Right leg Hip Vertical
#define cRRTibiaPin      0  // Rear Right leg Knee

#define cRMCoxaPin       5  // Middle Right leg Hip Horizontal
#define cRMFemurPin      6  // Middle Right leg Hip Vertical
#define cRMTibiaPin      4  // Middle Right leg Knee

#define cRFCoxaPin      13  // Front Right leg Hip Horizontal
#define cRFFemurPin     14  // Front Right leg Hip Vertical
#define cRFTibiaPin     15  // Front Right leg Knee

#define cLRCoxaPin      18  // Rear Left leg Hip Horizontal
#define cLRFemurPin     17  // Rear Left leg Hip Vertical
#define cLRTibiaPin     16  // Rear Left leg Knee

#define cLMCoxaPin      21  // Middle Left leg Hip Horizontal
#define cLMFemurPin     22  // Middle Left leg Hip Vertical
#define cLMTibiaPin     23  // Middle Left leg Knee

#define cLFCoxaPin      29  // Front Left leg Hip Horizontal
#define cLFFemurPin     30  // Front Left leg Hip Vertical
#define cLFTibiaPin     31  // Front Left leg Knee

#define cLMandPin       26  // Left Mandible
#define cRMandPin       27  // Right Mandible
#define cHeadPanPin     24  // Head Pan (horizontal)
#define cHeadTiltPin    25  // Head Tilt (vertical)
#define cHeadRotPin     11  // Head Rotate

#define cTailPanPin     10  // Tail Pan (horizontal)
#define cTailTiltPin     9  // Tail Tilt (vertical)

//--------------------------------------------------------------------
//[MIN/MAX ANGLES]
#define cRRCoxaMin1     -650      //Mechanical limits of the Right Rear Leg
#define cRRCoxaMax1     650
#define cRRFemurMin1    -1050
#define cRRFemurMax1    750
#define cRRTibiaMin1    -530
#define cRRTibiaMax1    900

#define cRMCoxaMin1     -650      //Mechanical limits of the Right Middle Leg
#define cRMCoxaMax1     650
#define cRMFemurMin1    -1050
#define cRMFemurMax1    750
#define cRMTibiaMin1    -530
#define cRMTibiaMax1    900

#define cRFCoxaMin1     -650      //Mechanical limits of the Right Front Leg
#define cRFCoxaMax1     650
#define cRFFemurMin1    -1050
#define cRFFemurMax1    750
#define cRFTibiaMin1    -530
#define cRFTibiaMax1    900

#define cLRCoxaMin1     -650      //Mechanical limits of the Left Rear Leg
#define cLRCoxaMax1     650
#define cLRFemurMin1    -1050
#define cLRFemurMax1    750
#define cLRTibiaMin1    -530
#define cLRTibiaMax1    900

#define cLMCoxaMin1     -650      //Mechanical limits of the Left Middle Leg
#define cLMCoxaMax1     650
#define cLMFemurMin1    -1050
#define cLMFemurMax1    750
#define cLMTibiaMin1    -530
#define cLMTibiaMax1    900

#define cLFCoxaMin1     -650      //Mechanical limits of the Left Front Leg
#define cLFCoxaMax1     650
#define cLFFemurMin1    -1050
#define cLFFemurMax1    750
#define cLFTibiaMin1    -530
#define cLFTibiaMax1    900

#define cMandibleMin1   -200
#define cMandibleMax1    900
#define wMandibleInc      40  // mandible increment
#define wMandibleTime     10  // mandible update time
#define cHeadPanMin1    -380
#define cHeadPanMax1     380
#define cHeadTiltMin1   -370
#define cHeadTiltMax1    470
#define cHeadRotMin1    -900
#define cHeadRotMax1     900

#define cTailPanMin1    -500
#define cTailPanMax1     500
#define cTailTiltMin1   -500
#define cTailTiltMax1    500

//--------------------------------------------------------------------
//[LEG DIMENSIONS]
//Universal dimensions for each leg in mm
#define cXXCoxaLength     50    // This is for CH3-R with Type 3 legs
#define cXXFemurLength    80
#define cXXTibiaLength    120

#define cRRCoxaLength     cXXCoxaLength	    //Right Rear leg
#define cRRFemurLength    cXXFemurLength
#define cRRTibiaLength    cXXTibiaLength

#define cRMCoxaLength     cXXCoxaLength	    //Right middle leg
#define cRMFemurLength    cXXFemurLength
#define cRMTibiaLength    cXXTibiaLength

#define cRFCoxaLength     cXXCoxaLength	    //Rigth front leg
#define cRFFemurLength    cXXFemurLength
#define cRFTibiaLength    cXXTibiaLength

#define cLRCoxaLength     cXXCoxaLength	    //Left Rear leg
#define cLRFemurLength    cXXFemurLength
#define cLRTibiaLength    cXXTibiaLength

#define cLMCoxaLength     cXXCoxaLength	    //Left middle leg
#define cLMFemurLength    cXXFemurLength
#define cLMTibiaLength    cXXTibiaLength

#define cLFCoxaLength     cXXCoxaLength	    //Left front leg
#define cLFFemurLength    cXXFemurLength
#define cLFTibiaLength    cXXTibiaLength

//--------------------------------------------------------------------
//[BODY DIMENSIONS]
#define cRRCoxaAngle1  -450   // Default Coxa setup angle, decimals = 1
#define cRMCoxaAngle1     0   // Default Coxa setup angle, decimals = 1
#define cRFCoxaAngle1   450   // Default Coxa setup angle, decimals = 1
#define cLRCoxaAngle1  -450   // Default Coxa setup angle, decimals = 1
#define cLMCoxaAngle1     0   // Default Coxa setup angle, decimals = 1
#define cLFCoxaAngle1   450   // Default Coxa setup angle, decimals = 1

#define cRROffsetX      -70   // Distance X from center of the body to the Right Rear coxa
#define cRROffsetZ       85   // Distance Z from center of the body to the Right Rear coxa
#define cRMOffsetX      -70   // Distance X from center of the body to the Right Middle coxa
#define cRMOffsetZ        0   // Distance Z from center of the body to the Right Middle coxa
#define cRFOffsetX      -70   // Distance X from center of the body to the Right Front coxa
#define cRFOffsetZ      -85   // Distance Z from center of the body to the Right Front coxa

#define cLROffsetX       70   // Distance X from center of the body to the Left Rear coxa
#define cLROffsetZ       85   // Distance Z from center of the body to the Left Rear coxa
#define cLMOffsetX       70   // Distance X from center of the body to the Left Middle coxa
#define cLMOffsetZ        0   // Distance Z from center of the body to the Left Middle coxa
#define cLFOffsetX       70   // Distance X from center of the body to the Left Front coxa
#define cLFOffsetZ      -85   // Distance Z from center of the body to the Left Front coxa

//--------------------------------------------------------------------
//[START POSITIONS FEET]
#define cHexInitXZ	     120
#define CHexInitXZCos45   85  // COS(45) * XZ
#define CHexInitXZSin45   85  // sin(45) * XZ
#define CHexInitY	        80


#define cRRInitPosX     CHexInitXZCos45      //Start positions of the Right Rear leg
#define cRRInitPosY     CHexInitY
#define cRRInitPosZ     CHexInitXZSin45

#define cRMInitPosX     cHexInitXZ      //Start positions of the Right Middle leg
#define cRMInitPosY     CHexInitY
#define cRMInitPosZ     0

#define cRFInitPosX     CHexInitXZCos45      //Start positions of the Right Front leg
#define cRFInitPosY     CHexInitY
#define cRFInitPosZ     -CHexInitXZSin45

#define cLRInitPosX     CHexInitXZCos45      //Start positions of the Left Rear leg
#define cLRInitPosY     CHexInitY
#define cLRInitPosZ     CHexInitXZSin45

#define cLMInitPosX     cHexInitXZ      //Start positions of the Left Middle leg
#define cLMInitPosY     CHexInitY
#define cLMInitPosZ     0

#define cLFInitPosX     CHexInitXZCos45      //Start positions of the Left Front leg
#define cLFInitPosY     CHexInitY
#define cLFInitPosZ     -CHexInitXZSin45

#define cPwmDiv       991  //old 1059;
#define cPFConst      592  //old 650 ; 900*(1000/cPwmDiv)+cPFConst must always be 1500
// A PWM/deg factor of 10,09 give cPwmDiv = 991 and cPFConst = 592
// For a modified 5645 (to 180 deg travel): cPwmDiv = 1500 and cPFConst = 900.
#define DEG2PWM(deg) ((long)(deg) * 2000L / 1800L + 1500L)
