//====================================================================
// Servo Driver - This version is setup to use the SSC-32 to control
// the servos.
//====================================================================
#include <Arduino.h>
#include "globals.h"
#include "ServoDriver.h"

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

// definition of some helper functions
extern int SSCRead(byte *pb, int cb, word wTimeout, word wEOL);
int SSCReadByte(word wTimeout);

//--------------------------------------------------------------------
//Init
//--------------------------------------------------------------------
void ServoDriver::Init() {
  DBGSerial.println(F("Init servo driver"));
  SSCSerial.begin(cSSC_BAUD);
  SSCSerial.listen();
  delay(100);
  SSCSerial.print("ver\r");

  char abVer[40];        // give a nice large buffer.
  int cbRead = SSCRead((byte*)abVer, sizeof(abVer), 10000, 13);
  DBGSerial.print(F("SSC Version: "));
  if (cbRead > 0) {
    DBGSerial.write((byte*)abVer, cbRead);
  } else {
    DBGSerial.print("n/a");
  }
  DBGSerial.println();
}

float ServoDriver::ReadVoltage() {
//  DBGSerial.println(F("Reading voltage"));
  SSCSerial.write('V');
  SSCSerial.write(cSSC_ADC_VOLT);
  SSCSerial.write('\r');

  int rd = SSCReadByte(10000);
  if (rd < 0) {
//    DBGSerial.println(F("Timeout."));
    return 0;
  }
  float v = rd / 10.2f;
//  DBGSerial.print(v);
//  DBGSerial.println(F("v\n"));
  return v;
}

//------------------------------------------------------------------------------------------
//[OutputServoInfoForLeg] Do the output to the SSC-32 for the servos associated with
//         the Leg number passed in.
//------------------------------------------------------------------------------------------

void ServoDriver::OutputServoInfoForLeg(byte LegIndex, short sCoxaAngle1, short sFemurAngle1, short sTibiaAngle1)
{
  uint16_t wCoxaSSCV;
  uint16_t wFemurSSCV;
  uint16_t wTibiaSSCV;

  //Update Right Legs
  if (LegIndex < 3) {
    wCoxaSSCV = ((long) (-sCoxaAngle1 + 900)) * 1000 / cPwmDiv + cPFConst;
    wFemurSSCV = ((long) (-sFemurAngle1 + 900)) * 1000 / cPwmDiv + cPFConst;
    wTibiaSSCV = ((long) (-sTibiaAngle1 + 900)) * 1000 / cPwmDiv + cPFConst;
  } else {
    wCoxaSSCV = ((long) (sCoxaAngle1 + 900)) * 1000 / cPwmDiv + cPFConst;
    wFemurSSCV = ((long) ((long) (sFemurAngle1 + 900)) * 1000 / cPwmDiv + cPFConst);
    wTibiaSSCV = ((long) (sTibiaAngle1 + 900)) * 1000 / cPwmDiv + cPFConst;
  }

  SSCSerial.write(pgm_read_byte(&cCoxaPin[LegIndex])  + 0x80);
  SSCSerial.write(wCoxaSSCV >> 8);
  SSCSerial.write(wCoxaSSCV & 0xff);
  SSCSerial.write(pgm_read_byte(&cFemurPin[LegIndex]) + 0x80);
  SSCSerial.write(wFemurSSCV >> 8);
  SSCSerial.write(wFemurSSCV & 0xff);
  SSCSerial.write(pgm_read_byte(&cTibiaPin[LegIndex]) + 0x80);
  SSCSerial.write(wTibiaSSCV >> 8);
  SSCSerial.write(wTibiaSSCV & 0xff);
}

//------------------------------------------------------------------------------------------
//[OutputServoInfoForHead] Do the output to the SSC-32 for the head servos (w/o mandibles)
//------------------------------------------------------------------------------------------
void ServoDriver::OutputServoInfoHead(short pan, short tilt, short rot) {
  uint16_t wPanSSCV = DEG2PWM(pan);
  uint16_t wTiltSSCV = DEG2PWM(tilt);
  uint16_t wRotSSCV = DEG2PWM(rot);

  SSCSerial.write(cHeadPanPin  + 0x80);
  SSCSerial.write(wPanSSCV >> 8);
  SSCSerial.write(wPanSSCV & 0xff);
  SSCSerial.write(cHeadTiltPin  + 0x80);
  SSCSerial.write(wTiltSSCV >> 8);
  SSCSerial.write(wTiltSSCV & 0xff);
  SSCSerial.write(cHeadRotPin  + 0x80);
  SSCSerial.write(wRotSSCV >> 8);
  SSCSerial.write(wRotSSCV & 0xff);
}

//------------------------------------------------------------------------------------------
//[OutputServoInfoForTail] Do the output to the SSC-32 for the tail servos
//------------------------------------------------------------------------------------------
void ServoDriver::OutputServoInfoTail(short pan, short tilt) {
  uint16_t wPanSSCV = ((long) (-pan + 900)) * 1000 / cPwmDiv + cPFConst;
  uint16_t wTiltSSCV = ((long) (-tilt + 900)) * 1000 / cPwmDiv + cPFConst;

  SSCSerial.write(cTailPanPin  + 0x80);
  SSCSerial.write(wPanSSCV >> 8);
  SSCSerial.write(wPanSSCV & 0xff);
  SSCSerial.write(cTailTiltPin  + 0x80);
  SSCSerial.write(wTiltSSCV >> 8);
  SSCSerial.write(wTiltSSCV & 0xff);
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
//  uint16_t wLeftSSCV = ((long) (-left + 900)) * 1000 / cPwmDiv + cPFConst;
//  uint16_t wRightSSCV = ((long) (-right + 900)) * 1000 / cPwmDiv + cPFConst;
  uint16_t wLeftSSCV = DEG2PWM(-left);
  uint16_t wRightSSCV = DEG2PWM(-right);

  SSCSerial.write(cLMandPin  + 0x80);
  SSCSerial.write(wLeftSSCV >> 8u);
  SSCSerial.write(wLeftSSCV & 0xffu);
  SSCSerial.write(cRMandPin + 0x80);
  SSCSerial.write(wRightSSCV >> 8u);
  SSCSerial.write(wRightSSCV & 0xffu);
  this->CommitServoDriver(wMandibleTime);
}


//--------------------------------------------------------------------
//[CommitServoDriver Updates the positions of the servos - This outputs
//         as much of the command as we can without committing it.  This
//         allows us to once the previous update was completed to quickly 
//        get the next command to start
//--------------------------------------------------------------------
void ServoDriver::CommitServoDriver(word wMoveTime) {
  byte    abOut[3];
  abOut[0] = 0xA1;
  abOut[1] = wMoveTime >> 8u;
  abOut[2] = wMoveTime & 0xffu;
  SSCSerial.write(abOut, 3);
}

//--------------------------------------------------------------------
//[FREE SERVOS] Frees all the servos
//--------------------------------------------------------------------
void ServoDriver::FreeServos() {
  for (byte LegIndex = 0; LegIndex < 32; LegIndex++) {
    SSCSerial.print("#");
    SSCSerial.print(LegIndex, DEC);
    SSCSerial.print("P0");
  }
  SSCSerial.print("T200\r");
}


//==============================================================================
// SSC Forwarder - used to allow things like Lynxterm to talk to the SSC-32 
// through the Arduino...  Will see if it is fast enough...
//==============================================================================
#ifdef OPT_SSC_FORWARDER

void ServoDriver::SSCForwarder() {
  tone(SOUND_PIN, 440, 2000);
  delay(2000);
  int sChar = 0;
  int sPrevChar = 0;
  int sPrevPrevChar = 0;
  int sPrevPrevPrevChar = 0;
  DBGSerial.println("SSC Forwarder mode - Enter $<cr> to exit");

  while (digitalRead(PS2_CMD)) {
    if ((sChar = DBGSerial.read()) != -1) {
      SSCSerial.write(sChar & 0xff);
      if ((sChar == '\n') || (sChar == '\r')) {
        sChar = '\r';
        if (sPrevChar == '$') {
          break;    // exit out of the loop
        }
      }
      sPrevPrevPrevChar = sPrevPrevChar;
      sPrevPrevChar = sPrevChar;
      sPrevChar = sChar;
    }

    if ((sChar = SSCSerial.read()) != -1) {
      if (sPrevPrevPrevChar == 'V') {
        DBGSerial.print("\nValue: ");
        DBGSerial.println(sChar & 0xff, DEC);
      } else {
        DBGSerial.write(sChar & 0xff);
      }
      if (sChar == '\r') {
        DBGSerial.write('\n');
      }
    }
  }
  DBGSerial.println("Exited SSC Forwarder mode");
}

#endif // OPT_SSC_FORWARDER


int SSCRead(byte *pb, int cb, word wTimeout, word wEOL) {
  int ich;
  byte *pbIn = pb;
  unsigned long ulTimeLastChar = micros();
  while (cb) {
    ich = SSCReadByte(wTimeout);
    if (ich < 0) {
      break;
    }
    *pb++ = (byte) ich;
    cb--;
    if ((word) ich == wEOL) {
      break;
    }
  }
  return (int) (pb - pbIn);
}

/**
 * Reads a byte from the serial with timeout
 * @param wTimeout timeout in microseconds
 * @return the byte or -1 if timeout exceeded
 */
int SSCReadByte(word wTimeout) {
  unsigned long ulTimeLastChar = micros();
  while (!SSCSerial.available()) {
    // check for timeout
    if ((word) (micros() - ulTimeLastChar) > wTimeout) {
      return -1;
    }
  }
  return SSCSerial.read();
}

