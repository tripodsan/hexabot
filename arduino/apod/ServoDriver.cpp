//====================================================================
//Project Lynxmotion Phoenix
//
// Servo Driver - This version is setup to use the SSC-32 to control
// the servos.
//====================================================================
#include <Arduino.h>
#include "globals.h"
#include "ServoDriver.h"

#define NUMSERVOSPERLEG 3

#ifdef USE_SSC32

//Servo Pin numbers - May be SSC-32 or actual pins on main controller, depending on configuration.
const byte cCoxaPin[] PROGMEM = {cRRCoxaPin, cRMCoxaPin, cRFCoxaPin, cLRCoxaPin, cLMCoxaPin, cLFCoxaPin};
const byte cFemurPin[] PROGMEM = {cRRFemurPin, cRMFemurPin, cRFFemurPin, cLRFemurPin, cLMFemurPin, cLFFemurPin};
const byte cTibiaPin[] PROGMEM = {cRRTibiaPin, cRMTibiaPin, cRFTibiaPin, cLRTibiaPin, cLMTibiaPin, cLFTibiaPin};

// Add support for running on non-mega Arduino boards as well.
#ifdef __AVR__
#if not defined(UBRR1H)
#if cSSC_IN == 0
#define SSCSerial Serial
#else
SoftwareSerial SSCSerial(cSSC_IN, cSSC_OUT);
#endif
#endif
#endif

//=============================================================================
// Global - Local to this file only...
//=============================================================================

// definition of some helper functions
extern int SSCRead(byte *pb, int cb, word wTimeout, word wEOL);


//--------------------------------------------------------------------
//Init
//--------------------------------------------------------------------
void ServoDriver::Init() {
  DBGSerial.println("---init server driver---");
#ifdef __AVR__
#if not defined(UBRR1H)
#if cSSC_IN != 0
  pinMode(cSSC_IN, INPUT);
  pinMode(cSSC_OUT, OUTPUT);
  SSCSerial.listen();
#endif
#endif
#endif

  SSCSerial.begin(cSSC_BAUD);
  SSCSerial.listen();
  SSCSerial.print("ver\r");

  char abVer[40];        // give a nice large buffer.
  int cbRead = SSCRead((byte*)abVer, sizeof(abVer), 10000, 13);
  DBGSerial.print("SSC Version: ");
  if (cbRead > 0) {
    DBGSerial.write((byte*)abVer, cbRead);
  } else {
    DBGSerial.print("n/a");
  }
  DBGSerial.println();
}

//------------------------------------------------------------------------------------------
//[BeginServoUpdate] Does whatever preperation that is needed to starrt a move of our servos
//------------------------------------------------------------------------------------------
void ServoDriver::BeginServoUpdate()    // Start the update
{
}

//------------------------------------------------------------------------------------------
//[OutputServoInfoForLeg] Do the output to the SSC-32 for the servos associated with
//         the Leg number passed in.
//------------------------------------------------------------------------------------------
#define cPwmDiv       991  //old 1059;
#define cPFConst      592  //old 650 ; 900*(1000/cPwmDiv)+cPFConst must always be 1500
// A PWM/deg factor of 10,09 give cPwmDiv = 991 and cPFConst = 592
// For a modified 5645 (to 180 deg travel): cPwmDiv = 1500 and cPFConst = 900.

void ServoDriver::OutputServoInfoForLeg(byte LegIndex, short sCoxaAngle1, short sFemurAngle1, short sTibiaAngle1)
#endif
{
  word wCoxaSSCV;        // Coxa value in SSC units
  word wFemurSSCV;        //
  word wTibiaSSCV;        //

  //Update Right Legs
  g_InputController.AllowControllerInterrupts(false);    // If on xbee on hserial tell hserial to not processess...
  if (LegIndex < 3) {
    wCoxaSSCV = ((long) (-sCoxaAngle1 + 900)) * 1000 / cPwmDiv + cPFConst;
    wFemurSSCV = ((long) (-sFemurAngle1 + 900)) * 1000 / cPwmDiv + cPFConst;
    wTibiaSSCV = ((long) (-sTibiaAngle1 + 900)) * 1000 / cPwmDiv + cPFConst;
  } else {
    wCoxaSSCV = ((long) (sCoxaAngle1 + 900)) * 1000 / cPwmDiv + cPFConst;
    wFemurSSCV = ((long) ((long) (sFemurAngle1 + 900)) * 1000 / cPwmDiv + cPFConst);
    wTibiaSSCV = ((long) (sTibiaAngle1 + 900)) * 1000 / cPwmDiv + cPFConst;
  }

#ifdef cSSC_BINARYMODE
  SSCSerial.write(pgm_read_byte(&cCoxaPin[LegIndex])  + 0x80);
  SSCSerial.write(wCoxaSSCV >> 8);
  SSCSerial.write(wCoxaSSCV & 0xff);
  SSCSerial.write(pgm_read_byte(&cFemurPin[LegIndex]) + 0x80);
  SSCSerial.write(wFemurSSCV >> 8);
  SSCSerial.write(wFemurSSCV & 0xff);
  SSCSerial.write(pgm_read_byte(&cTibiaPin[LegIndex]) + 0x80);
  SSCSerial.write(wTibiaSSCV >> 8);
  SSCSerial.write(wTibiaSSCV & 0xff);
#else
  SSCSerial.print("#");
  SSCSerial.print(pgm_read_byte(&cCoxaPin[LegIndex]), DEC);
  SSCSerial.print("P");
  SSCSerial.print(wCoxaSSCV, DEC);
  SSCSerial.print("#");
  SSCSerial.print(pgm_read_byte(&cFemurPin[LegIndex]), DEC);
  SSCSerial.print("P");
  SSCSerial.print(wFemurSSCV, DEC);
  SSCSerial.print("#");
  SSCSerial.print(pgm_read_byte(&cTibiaPin[LegIndex]), DEC);
  SSCSerial.print("P");
  SSCSerial.print(wTibiaSSCV, DEC);
#endif
  g_InputController.AllowControllerInterrupts(true);    // Ok for hserial again...
}

//------------------------------------------------------------------------------------------
//[OutputServoInfoForHead] Do the output to the SSC-32 for the head servos (w/o mandibles)
//------------------------------------------------------------------------------------------
void ServoDriver::OutputServoInfoHead(short pan, short tilt, short rot) {
  word wPanSSCV;        // Pan value in SSC units
  word wTiltSSCV;      //
  word wRotSSCV;        //

  //Update Right Legs
  g_InputController.AllowControllerInterrupts(false);    // If on xbee on hserial tell hserial to not processess...
  wPanSSCV = ((long) (-pan + 900)) * 1000 / cPwmDiv + cPFConst;
  wTiltSSCV = ((long) (-tilt + 900)) * 1000 / cPwmDiv + cPFConst;
  wRotSSCV = ((long) (-rot + 900)) * 1000 / cPwmDiv + cPFConst;

#ifdef cSSC_BINARYMODE
  SSCSerial.write(cHeadPanPin  + 0x80);
  SSCSerial.write(wPanSSCV >> 8);
  SSCSerial.write(wPanSSCV & 0xff);
  SSCSerial.write(cHeadTiltPin  + 0x80);
  SSCSerial.write(wTiltSSCV >> 8);
  SSCSerial.write(wTiltSSCV & 0xff);
  SSCSerial.write(cHeadRotPin  + 0x80);
  SSCSerial.write(wRotSSCV >> 8);
  SSCSerial.write(wRotSSCV & 0xff);
#else
  SSCSerial.print("#");
  SSCSerial.print(cHeadPanPin, DEC);
  SSCSerial.print("P");
  SSCSerial.print(wPanSSCV, DEC);
  SSCSerial.print("#");
  SSCSerial.print(cHeadTiltPin, DEC);
  SSCSerial.print("P");
  SSCSerial.print(wTiltSSCV, DEC);
  SSCSerial.print("#");
  SSCSerial.print(cHeadRotPin, DEC);
  SSCSerial.print("P");
  SSCSerial.print(wRotSSCV, DEC);
#endif
  g_InputController.AllowControllerInterrupts(true);    // Ok for hserial again...
}

//------------------------------------------------------------------------------------------
//[OutputServoInfoForTail] Do the output to the SSC-32 for the tail servos
//------------------------------------------------------------------------------------------
void ServoDriver::OutputServoInfoTail(short pan, short tilt) {
  word wPanSSCV;        // Pan value in SSC units
  word wTiltSSCV;      //

  //Update Right Legs
  g_InputController.AllowControllerInterrupts(false);    // If on xbee on hserial tell hserial to not processess...
  wPanSSCV = ((long) (-pan + 900)) * 1000 / cPwmDiv + cPFConst;
  wTiltSSCV = ((long) (-tilt + 900)) * 1000 / cPwmDiv + cPFConst;

#ifdef cSSC_BINARYMODE
  SSCSerial.write(cTailPanPin  + 0x80);
  SSCSerial.write(wPanSSCV >> 8);
  SSCSerial.write(wPanSSCV & 0xff);
  SSCSerial.write(cTailTiltPin  + 0x80);
  SSCSerial.write(wTiltSSCV >> 8);
  SSCSerial.write(wTiltSSCV & 0xff);
#else
  SSCSerial.print("#");
  SSCSerial.print(cTailPanPin, DEC);
  SSCSerial.print("P");
  SSCSerial.print(wPanSSCV, DEC);
  SSCSerial.print("#");
  SSCSerial.print(cTailTiltPin, DEC);
  SSCSerial.print("P");
  SSCSerial.print(wTiltSSCV, DEC);
#endif
  g_InputController.AllowControllerInterrupts(true);    // Ok for hserial again...
}

//------------------------------------------------------------------------------------------
//[OutputServoInfoForMandibles] Do the output to the SSC-32 for the mandible servos
//------------------------------------------------------------------------------------------

/**
 * updates the mandibles
 * @param left Left mandible angle in degrees. (1 decimal)
 * @param right Right mandible angle in degrees. (1 decimal)
 */
void ServoDriver::OutputServoInfoMandibles(short left, short right) {
  uint16_t wLeftSSCV;
  uint16_t wRightSSCV;

  //Update Right Legs
  g_InputController.AllowControllerInterrupts(false);    // If on xbee on hserial tell hserial to not processess...
  wLeftSSCV = ((long) (-left + 900)) * 1000 / cPwmDiv + cPFConst;
  wRightSSCV = ((long) (-right + 900)) * 1000 / cPwmDiv + cPFConst;

#ifdef cSSC_BINARYMODE
  SSCSerial.write(cLMandPin  + 0x80);
  SSCSerial.write(wLeftSSCV >> 8);
  SSCSerial.write(wLeftSSCV & 0xff);
  SSCSerial.write(cRMandPin + 0x80);
  SSCSerial.write(wRightSSCV >> 8);
  SSCSerial.write(wRightSSCV & 0xff);
  byte abOut[3];
  abOut[0] = 0xA1;
  abOut[1] = wMandibleTime >> 8;
  abOut[2] = wMandibleTime & 0xff;
  SSCSerial.write(abOut, 3);
#else
  SSCSerial.print("#");
  SSCSerial.print(cLMandPin, DEC);
  SSCSerial.print("P");
  SSCSerial.print(wLeftSSCV, DEC);
  SSCSerial.print("#");
  SSCSerial.print(cRMandPin, DEC);
  SSCSerial.print("P");
  SSCSerial.print(wRightSSCV, DEC);
  SSCSerial.print("T");
  SSCSerial.print(wMandibleTime, DEC);
#endif
  g_InputController.AllowControllerInterrupts(true);    // Ok for hserial again...
}


//--------------------------------------------------------------------
//[CommitServoDriver Updates the positions of the servos - This outputs
//         as much of the command as we can without committing it.  This
//         allows us to once the previous update was completed to quickly 
//        get the next command to start
//--------------------------------------------------------------------
void ServoDriver::CommitServoDriver(word wMoveTime) {
#ifdef cSSC_BINARYMODE
  byte    abOut[3];
#endif

  g_InputController.AllowControllerInterrupts(false);    // If on xbee on hserial tell hserial to not processess...

#ifdef cSSC_BINARYMODE
  abOut[0] = 0xA1;
  abOut[1] = wMoveTime >> 8;
  abOut[2] = wMoveTime & 0xff;
  SSCSerial.write(abOut, 3);
#else
  //Send <CR>
  SSCSerial.print("T");
  SSCSerial.println(wMoveTime, DEC);
#endif

  g_InputController.AllowControllerInterrupts(true);

}

//--------------------------------------------------------------------
//[FREE SERVOS] Frees all the servos
//--------------------------------------------------------------------
void ServoDriver::FreeServos() {
  g_InputController.AllowControllerInterrupts(false);    // If on xbee on hserial tell hserial to not processess...
  for (byte LegIndex = 0; LegIndex < 32; LegIndex++) {
    SSCSerial.print("#");
    SSCSerial.print(LegIndex, DEC);
    SSCSerial.print("P0");
  }
  SSCSerial.print("T200\r");
  g_InputController.AllowControllerInterrupts(true);
}


//==============================================================================
// SSC Forwarder - used to allow things like Lynxterm to talk to the SSC-32 
// through the Arduino...  Will see if it is fast enough...
//==============================================================================
#ifdef OPT_SSC_FORWARDER

void ServoDriver::SSCForwarder() {
  MSound(SOUND_PIN, 1, 1000, 2000);  //sound SOUND_PIN, [50\4000]
  delay(2000);
  int sChar;
  int sPrevChar;
  DBGSerial.println("SSC Forwarder mode - Enter $<cr> to exit");

  while (digitalRead(PS2_CMD)) {
    if ((sChar = DBGSerial.read()) != -1) {
      SSCSerial.write(sChar & 0xff);
      if (((sChar == '\n') || (sChar == '\r')) && (sPrevChar == '$'))
        break;    // exit out of the loop
      sPrevChar = sChar;
    }

    if ((sChar = SSCSerial.read()) != -1) {
      DBGSerial.write(sChar & 0xff);
    }
  }
  DBGSerial.println("Exited SSC Forwarder mode");
}

#endif // OPT_SSC_FORWARDER


//==============================================================================
// Quick and dirty helper function to read so many bytes in from the SSC with a timeout and an end of character marker...
//==============================================================================
int SSCRead(byte *pb, int cb, word wTimeout, word wEOL) {
  int ich;
  byte *pbIn = pb;
  unsigned long ulTimeLastChar = micros();
  while (cb) {
    while (!SSCSerial.available()) {
      // check for timeout
      if ((word) (micros() - ulTimeLastChar) > wTimeout) {
        return (int) (pb - pbIn);
      }
    }
    ich = SSCSerial.read();
    *pb++ = (byte) ich;
    cb--;

    if ((word) ich == wEOL)
      break;    // we matched so get out of here.
    ulTimeLastChar = micros();    // update to say we received something
  }

  return (int) (pb - pbIn);
}

//==============================================================================
//	FindServoOffsets - Find the zero points for each of our servos... 
// 		Will use the new servo function to set the actual pwm rate and see
//		how well that works...
//==============================================================================
#ifdef OPT_FIND_SERVO_OFFSETS

void ServoDriver::FindServoOffsets() {
  // not clean but...
  byte abSSCServoNum[NUMSERVOSPERLEG * 6];           // array of servos...
  signed char asOffsets[NUMSERVOSPERLEG * 6];        // we have 18 servos to find/set offsets for...
  signed char asOffsetsRead[NUMSERVOSPERLEG * 6];    // array for our read in servos...

  static char *apszLegs[] = {"RR", "RM", "RF", "LR", "LM", "LF"};  // Leg Order
  static char *apszLJoints[] = {" Coxa", " Femur", " Tibia", " tArs"}; // which joint on the leg...

  byte szTemp[5];
  byte cbRead;

  int data;
  short sSN;      // which servo number
  boolean fNew = true;  // is this a new servo to work with?
  boolean fExit = false;  // when to exit
  int ich;

  if (CheckVoltage()) {
    // Voltage is low...
    Serial.println("Low Voltage: fix or hit $ to abort");
    while (CheckVoltage()) {
      if (Serial.read() == '$') return;
    }
  }

  // Fill in array of SSC-32 servo numbers
  for (sSN = 0; sSN < 6; sSN++) {   // Make sure all of our servos initialize to 0 offset from saved.
    abSSCServoNum[sSN * NUMSERVOSPERLEG + 0] = pgm_read_byte(&cCoxaPin[sSN]);
    abSSCServoNum[sSN * NUMSERVOSPERLEG + 1] = pgm_read_byte(&cFemurPin[sSN]);
    abSSCServoNum[sSN * NUMSERVOSPERLEG + 2] = pgm_read_byte(&cTibiaPin[sSN]);
  }
  // now lets loop through and get information and set servos to 1500
  for (sSN = 0; sSN < 6 * NUMSERVOSPERLEG; sSN++) {
    asOffsets[sSN] = 0;
    asOffsetsRead[sSN] = 0;

    SSCSerial.print("R");
    SSCSerial.println(32 + abSSCServoNum[sSN], DEC);
    // now read in the current value...  Maybe should use atoi...
    cbRead = SSCRead((byte *) szTemp, sizeof(szTemp), 10000, 13);
    if (cbRead > 0)
      asOffsetsRead[sSN] = atoi((const char *) szTemp);

    SSCSerial.print("#");
    SSCSerial.print(abSSCServoNum[sSN], DEC);
    SSCSerial.println("P1500");
  }
  for (sSN = 0; sSN < 6 * NUMSERVOSPERLEG; sSN++) {
    Serial.print("Servo: ");
    Serial.print(apszLegs[sSN / NUMSERVOSPERLEG]);
    Serial.print(apszLJoints[sSN % NUMSERVOSPERLEG]);
    Serial.print("(");
    Serial.print(asOffsetsRead[sSN] + asOffsets[sSN], DEC);
    Serial.println(")");
  }
  // OK lets move all of the servos to their zero point.
  Serial.println("Find Servo Zeros.\n$-Exit, +- changes, *-change servo");
  Serial.println("    0-5 Chooses a leg, C-Coxa, F-Femur, T-Tibia");

  sSN = true;
  while (!fExit) {
    if (fNew) {
      Serial.print("Servo: ");
      Serial.print(apszLegs[sSN / NUMSERVOSPERLEG]);
      Serial.print(apszLJoints[sSN % NUMSERVOSPERLEG]);
      Serial.print("(");
      Serial.print(asOffsetsRead[sSN] + asOffsets[sSN], DEC);
      Serial.println(")");

      // Now lets wiggle the servo
      SSCSerial.print("#");
      SSCSerial.print(abSSCServoNum[sSN], DEC);
      SSCSerial.print("P");
      SSCSerial.print(1500 + asOffsets[sSN] + 250, DEC);
      SSCSerial.println("T250");
      delay(250);

      SSCSerial.print("#");
      SSCSerial.print(abSSCServoNum[sSN], DEC);
      SSCSerial.print("P");
      SSCSerial.print(1500 + asOffsets[sSN] - 250, DEC);
      SSCSerial.println("T500");
      delay(500);

      SSCSerial.print("#");
      SSCSerial.print(abSSCServoNum[sSN], DEC);
      SSCSerial.print("P");
      SSCSerial.print(1500 + asOffsets[sSN], DEC);
      SSCSerial.println("T250");
      delay(250);

      fNew = false;
    }

    //get user entered data
    data = Serial.read();
    //if data received
    if (data != -1) {
      if (data == '$')
        fExit = true;  // not sure how the keypad will map so give NL, CR, LF... all implies exit

      else if ((data == '+') || (data == '-')) {
        if (data == '+')
          asOffsets[sSN] += 5;    // increment by 5us
        else
          asOffsets[sSN] -= 5;    // increment by 5us

        Serial.print("    ");
        Serial.println(asOffsetsRead[sSN] + asOffsets[sSN], DEC);

        SSCSerial.print("#");
        SSCSerial.print(abSSCServoNum[sSN], DEC);
        SSCSerial.print("P");
        SSCSerial.print(1500 + asOffsets[sSN], DEC);
        SSCSerial.println("T100");
      } else if ((data >= '0') && (data <= '5')) {
        // direct enter of which servo to change
        fNew = true;
        sSN = (sSN % NUMSERVOSPERLEG) + (data - '0') * NUMSERVOSPERLEG;
      } else if ((data == 'c') && (data == 'C')) {
        fNew = true;
        sSN = (sSN / NUMSERVOSPERLEG) * NUMSERVOSPERLEG + 0;
      } else if ((data == 'c') && (data == 'C')) {
        fNew = true;
        sSN = (sSN / NUMSERVOSPERLEG) * NUMSERVOSPERLEG + 1;
      } else if ((data == 'c') && (data == 'C')) {
        // direct enter of which servo to change
        fNew = true;
        sSN = (sSN / NUMSERVOSPERLEG) * NUMSERVOSPERLEG + 2;
      } else if (data == '*') {
        // direct enter of which servo to change
        fNew = true;
        sSN++;
        if (sSN == 6 * NUMSERVOSPERLEG)
          sSN = 0;
      }
    }
  }
  Serial.print("Find Servo exit ");
  for (sSN = 0; sSN < 6 * NUMSERVOSPERLEG; sSN++) {
    Serial.print("Servo: ");
    Serial.print(apszLegs[sSN / NUMSERVOSPERLEG]);
    Serial.print(apszLJoints[sSN % NUMSERVOSPERLEG]);
    Serial.print("(");
    Serial.print(asOffsetsRead[sSN] + asOffsets[sSN], DEC);
    Serial.println(")");
  }

  Serial.print("\nSave Changes? Y/N: ");

  //get user entered data
  while (((data = Serial.read()) == -1) || ((data >= 10) && (data <= 15)));

  if ((data == 'Y') || (data == 'y')) {
    // Ok they asked for the data to be saved.  We will store the data with a
    // number of servos (byte)at the start, followed by a byte for a checksum...followed by our offsets array...
    // Currently we store these values starting at EEPROM address 0. May later change...
    //

    for (sSN = 0; sSN < 6 * NUMSERVOSPERLEG; sSN++) {
      SSCSerial.print("R");
      SSCSerial.print(32 + abSSCServoNum[sSN], DEC);
      SSCSerial.print("=");
      SSCSerial.println(asOffsetsRead[sSN] + asOffsets[sSN], DEC);
      delay(10);
    }

    // Then I need to have the SSC-32 reboot in order to use the new values.
    delay(10);    // give it some time to write stuff out.
    SSCSerial.println("GOBOOT");
    delay(5);        // Give it a little time
    SSCSerial.println("g0000");    // tell it that we are done in the boot section so go run the normall SSC stuff...
    delay(500);                // Give it some time to boot up...

  } else {
    void LoadServosConfig();
  }

  FreeServos();

}

#endif  // OPT_FIND_SERVO_OFFSETS
