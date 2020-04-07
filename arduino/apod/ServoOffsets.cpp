//====================================================================
// Servo Driver - This version is setup to use the SSC-32 to control
// the servos.
//====================================================================
#include <Arduino.h>
#include "globals.h"
#include "ServoDriver.h"

//==============================================================================
//	FindServoOffsets - Find the zero points for each of our servos... 
// 		Will use the new servo function to set the actual pwm rate and see
//		how well that works...
//==============================================================================
#ifdef OPT_FIND_SERVO_OFFSETS

void printOffsets(char* apszNames[], int8_t asOffsets[]) {
  for (int i = 0; i < NUMSERVOS; i++) {
    Serial.print("Servo: ");
    Serial.print(apszNames[i]);
    Serial.print("(");
    Serial.print(asOffsets[i], DEC);
    Serial.println(")");
  }
}

void ServoDriver::FindServoOffsets() {
  // not clean but...
  byte abSSCServoNum[NUMSERVOS];
  byte bodyPart = 0;
  int8_t asOffsets[NUMSERVOS];

  static char *apszNames[] = {
      "RR Coxa", "RR Femr", "RR Tibi",
      "RM Coxa", "RM Femr", "RM Tibi",
      "RF Coxa", "RF Femr", "RF Tibi",
      "LR Coxa", "LR Femr", "LR Tibi",
      "LM Coxa", "LM Femr", "LM Tibi",
      "LF Coxa", "LF Femr", "LF Tibi",
      "HD LMan", "HD RMan",
      "HD Pan ", "HD Tilt", "HD Rot ",
      "TL Pan ", "TL Tilt",
  };

  byte szTemp[5];
  byte cbRead;

  int data;
  uint8_t sSN;      // which servo number
  boolean fNew = true;  // is this a new servo to work with?
  boolean fExit = false;  // when to exit

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
  abSSCServoNum[NUMSERVOSPERLEG * 6 + 0] = cLMandPin;
  abSSCServoNum[NUMSERVOSPERLEG * 6 + 1] = cRMandPin;
  abSSCServoNum[NUMSERVOSPERLEG * 6 + 2] = cHeadPanPin;
  abSSCServoNum[NUMSERVOSPERLEG * 6 + 3] = cHeadTiltPin;
  abSSCServoNum[NUMSERVOSPERLEG * 6 + 4] = cHeadRotPin;
  abSSCServoNum[NUMSERVOSPERLEG * 6 + 5] = cTailPanPin;
  abSSCServoNum[NUMSERVOSPERLEG * 6 + 6] = cTailTiltPin;

  // now lets loop through and get information and set servos to 1500
  for (sSN = 0; sSN < NUMSERVOS; sSN++) {
    SSCSerial.print("R");
    SSCSerial.println(32 + abSSCServoNum[sSN], DEC);
    // now read in the current value...  Maybe should use atoi...
    cbRead = SSCRead((byte *) szTemp, sizeof(szTemp), 10000, 13);
    if (cbRead > 0) {
      asOffsets[sSN] = atoi((const char *) szTemp);
    }
    SSCSerial.print("#");
    SSCSerial.print(abSSCServoNum[sSN], DEC);
    SSCSerial.println("P1500");
  }

  printOffsets(apszNames, asOffsets);

  // OK lets move all of the servos to their zero point.
  Serial.println("Find Servo Zeros.\n$-Exit, +- changes, *-change servo");
  Serial.println("    0-5 Chooses a leg, C-Coxa, F-Femur, T-Tibia");
  Serial.println("    6 Chooses a mandible, L-Left, R-Right");
  Serial.println("    7 Chooses head, P-Pan, T-Tilt, R-Rot");
  Serial.println("    8 Chooses tail, P-Pan, T-Tilt");

  sSN = 0;
  while (!fExit) {
    if (fNew) {
      Serial.print("Servo: ");
      Serial.print(apszNames[sSN]);
      Serial.print("(");
      Serial.print(asOffsets[sSN], DEC);
      Serial.println(")");

      // Now lets wiggle the servo
      SSCSerial.print("#");
      SSCSerial.print(abSSCServoNum[sSN], DEC);
      SSCSerial.print("P");
      SSCSerial.print(1500 + 250, DEC);
      SSCSerial.println("T250");
      delay(250);

      SSCSerial.print("#");
      SSCSerial.print(abSSCServoNum[sSN], DEC);
      SSCSerial.print("P");
      SSCSerial.print(1500 - 250, DEC);
      SSCSerial.println("T500");
      delay(500);

      SSCSerial.print("#");
      SSCSerial.print(abSSCServoNum[sSN], DEC);
      SSCSerial.print("P");
      SSCSerial.print(1500, DEC);
      SSCSerial.println("T250");
      delay(250);

      fNew = false;
    }

    //get user entered data
    data = Serial.read();
    //if data received
    if (data != -1) {
      if (data == '$') {
        fExit = true;  // not sure how the keypad will map so give NL, CR, LF... all implies exit
      }
      else if ((data == '+') || (data == '-')) {
        if (data == '+') {
          asOffsets[sSN] += 5;    // increment by 5us
        } else {
          asOffsets[sSN] -= 5;    // increment by 5us
        }

        Serial.print("    ");
        Serial.println(asOffsets[sSN], DEC);

        SSCSerial.print("#");
        SSCSerial.print(abSSCServoNum[sSN], DEC);
        SSCSerial.print("PO");
        SSCSerial.println(asOffsets[sSN], DEC);
      } else if ((data >= '0') && (data <= '5')) {
        // direct enter of which servo to change
        fNew = true;
        sSN = (sSN % NUMSERVOSPERLEG) + (data - '0') * NUMSERVOSPERLEG;
        bodyPart = 0;
      } else if (data == '6') {
        fNew = true;
        bodyPart = NUMSERVOSPERLEG * 6;
        sSN = bodyPart;
      } else if (data == '7') {
        fNew = true;
        bodyPart = NUMSERVOSPERLEG * 6 + 2;
        sSN = bodyPart;
      } else if (data == '8') {
        fNew = true;
        bodyPart = NUMSERVOSPERLEG * 6 + 2 + 3;
        sSN = bodyPart;
      } else if ((data == 'c') || (data == 'l') || (data == 'p')) {
        fNew = true;
        if (bodyPart == 0) {
          sSN = (sSN / NUMSERVOSPERLEG) * NUMSERVOSPERLEG + 0;
        } else {
          sSN = bodyPart;
        }
      } else if ((data == 't')) {
        fNew = true;
        if (bodyPart == 0) {
          sSN = (sSN / NUMSERVOSPERLEG) * NUMSERVOSPERLEG + 2;
        } else {
          sSN = bodyPart + 1;
        }
      } else if ((data == 'f')) {
        fNew = true;
        sSN = (sSN / NUMSERVOSPERLEG) * NUMSERVOSPERLEG + 1;
      } else if ((data == 'r')) {
        fNew = true;
        if (bodyPart == NUMSERVOSPERLEG * 6) {
          sSN = bodyPart + 1;
        } else {
          sSN = bodyPart + 2;
        }
      } else if (data == '*') {
        // direct enter of which servo to change
        fNew = true;
        sSN = (sSN + 1) % NUMSERVOS;
      }
    }
  }

  Serial.print("Find Servo exit ");
  printOffsets(apszNames, asOffsets);
  Serial.print("\nSave Changes? Y/N: ");

  //get user entered data
  while (((data = Serial.read()) == -1) || ((data >= 10) && (data <= 15)));

  if ((data == 'Y') || (data == 'y')) {
    for (sSN = 0; sSN < NUMSERVOS; sSN++) {
      SSCSerial.print("R");
      SSCSerial.print(32 + abSSCServoNum[sSN], DEC);
      SSCSerial.print("=");
      SSCSerial.println(asOffsets[sSN], DEC);
      delay(10);
    }
    // bit 2 Initial Pulse: Offset Enable; If '1', enables the Initial Pulse Offset register
    SSCSerial.println("R0=4");

    // Then I need to have the SSC-32 reboot in order to use the new values.
    delay(10);    // give it some time to write stuff out.
    SSCSerial.println("GOBOOT");
    delay(10);        // Give it a little time
    SSCSerial.println("g0000");    // tell it that we are done in the boot section so go run the normall SSC stuff...
    delay(500);                // Give it some time to boot up...
  }
  FreeServos();
}

#endif  // OPT_FIND_SERVO_OFFSETS
