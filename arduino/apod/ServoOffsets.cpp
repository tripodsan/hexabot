#include <Arduino.h>
#include "globals.h"
#include "PS2Support.h"

#ifdef OPT_FIND_SERVO_OFFSETS

#define USE_PS2_CONTROLLER

void printOffsets(int8_t asOffsets[]) {
  for (int i = 0; i < NUMSERVOS; i++) {
    Serial.print("Servo: ");
    Serial.print(ALL_SERVOS_NAMES[i]);
    Serial.print("(");
    Serial.print(asOffsets[i], DEC);
    Serial.println(")");
  }
}

void ServoDriver::FindServoOffsets() {
  PS2X *ps2x = PS2X::Instance();
  byte bodyPart = 0;
  int8_t asOffsets[NUMSERVOS];

  uint8_t idx, prevIdx;      // which servo number
  int data;
  boolean fExit = false;  // when to exit

  if (CheckVoltage()) {
    // Voltage is low...
    Serial.println("Low Voltage: fix or hit $ to abort");
    while (CheckVoltage()) {
      if (Serial.read() == '$') return;
    }
  }

  // now lets loop through and get information and set servos to 1500
  byte szTemp[5];
  for (idx = 0; idx < NUMSERVOS; idx++) {
    SSCSerial.print("R");
    SSCSerial.println(32 + ALL_SERVOS_PINS[idx], DEC);
    // now read in the current value...  Maybe should use atoi...
    byte cbRead = SSCRead((byte *) szTemp, sizeof(szTemp), 10000, 13);
    if (cbRead > 0) {
      asOffsets[idx] = atoi((const char *) szTemp);
    }
    SSCSerial.print("#");
    SSCSerial.print(ALL_SERVOS_PINS[idx], DEC);
    SSCSerial.println("P1500");
  }

  printOffsets(asOffsets);

  // OK lets move all of the servos to their zero point.
  Serial.println("Find Servo Zeros.\n$-Exit, +- changes, *-change servo");
  Serial.println("    0-5 Chooses a leg, C-Coxa, F-Femur, T-Tibia");
  Serial.println("    6 Chooses a mandible, L-Left, R-Right");
  Serial.println("    7 Chooses head, P-Pan, T-Tilt, R-Rot");
  Serial.println("    8 Chooses tail, P-Pan, T-Tilt");
#ifdef USE_PS2_CONTROLLER
  Serial.println("    ps2: []-prev, O-next, pad +/-");
#endif

  idx = 0;
  prevIdx = -1;
  while (!fExit) {
    if (idx != prevIdx) {
      prevIdx = idx;
      Serial.print("Servo: ");
      Serial.print(ALL_SERVOS_NAMES[idx]);
      Serial.print("(");
      Serial.print(asOffsets[idx], DEC);
      Serial.println(")");

      // Now lets wiggle the servo
      SSCSerial.print("#");
      SSCSerial.print(ALL_SERVOS_PINS[idx], DEC);
      SSCSerial.print("P");
      SSCSerial.print(1500 + 100, DEC);
      SSCSerial.println("T100");
      delay(100);

      SSCSerial.print("#");
      SSCSerial.print(ALL_SERVOS_PINS[idx], DEC);
      SSCSerial.print("P");
      SSCSerial.print(1500 - 200, DEC);
      SSCSerial.println("T200");
      delay(200);

      SSCSerial.print("#");
      SSCSerial.print(ALL_SERVOS_PINS[idx], DEC);
      SSCSerial.print("P");
      SSCSerial.print(1500, DEC);
      SSCSerial.println("T100");
      delay(200);
    }
    int8_t offset = asOffsets[idx];

    if (Serial.available()) {
      data = Serial.read();
      if (data == '$') {
        fExit = true;  // not sure how the keypad will map so give NL, CR, LF... all implies exit
      }
      else if ((data == '+') || (data == '-')) {
        if (data == '+') {
          offset += 5;    // increment by 5us
        } else {
          offset -= 5;    // increment by 5us
        }
      } else if ((data >= '0') && (data <= '5')) {
        // direct enter of which servo to change
        idx = (idx % NUMSERVOSPERLEG) + (data - '0') * NUMSERVOSPERLEG;
        bodyPart = 0;
        continue;
      } else if (data == '6') {
        bodyPart = NUMSERVOSPERLEG * 6;
        idx = bodyPart;
      } else if (data == '7') {
        bodyPart = NUMSERVOSPERLEG * 6 + 2;
        idx = bodyPart;
      } else if (data == '8') {
        bodyPart = NUMSERVOSPERLEG * 6 + 2 + 3;
        idx = bodyPart;
      } else if ((data == 'c') || (data == 'l') || (data == 'p')) {
        if (bodyPart == 0) {
          idx = (idx / NUMSERVOSPERLEG) * NUMSERVOSPERLEG + 0;
        } else {
          idx = bodyPart;
        }
      } else if ((data == 't')) {
        if (bodyPart == 0) {
          idx = (idx / NUMSERVOSPERLEG) * NUMSERVOSPERLEG + 2;
        } else {
          idx = bodyPart + 1;
        }
      } else if ((data == 'f')) {
        idx = (idx / NUMSERVOSPERLEG) * NUMSERVOSPERLEG + 1;
      } else if ((data == 'r')) {
        if (bodyPart == NUMSERVOSPERLEG * 6) {
          idx = bodyPart + 1;
        } else {
          idx = bodyPart + 2;
        }
      } else if (data == '*') {
        // direct enter of which servo to change
        idx = (idx + 1) % NUMSERVOS;
      }
    }
#ifdef USE_PS2_CONTROLLER
    if (ps2x->read_gamepad()) {
      if (ps2x->ButtonPressed(PSB_CIRCLE)) {
        idx = (idx + 1) % NUMSERVOS;
      }
      if (ps2x->ButtonPressed(PSB_SQUARE)) {
        idx = (idx - 1 + NUMSERVOS) % NUMSERVOS;
      }
      if (ps2x->ButtonPressed(PSB_PAD_LEFT)) {
        offset--;
      }
      if (ps2x->ButtonPressed(PSB_PAD_RIGHT)) {
        offset++;
      }
      if (ps2x->ButtonPressed(PSB_PAD_UP)) {
        offset+= 5;
      }
      if (ps2x->ButtonPressed(PSB_PAD_DOWN)) {
        offset-= 5;
      }
    } else {
      Serial.println("no ps2");
    }
    delay(50);
#endif
    offset = min(max(offset, -100), 100);
    if (prevIdx == idx && offset != asOffsets[idx]) {
      asOffsets[idx] = offset;
      Serial.print("    ");
      Serial.println(asOffsets[idx], DEC);

      SSCSerial.print("#");
      SSCSerial.print(ALL_SERVOS_PINS[idx], DEC);
      SSCSerial.print("PO");
      SSCSerial.println(asOffsets[idx], DEC);
    }
  }

  Serial.print("Find Servo exit ");
  printOffsets(asOffsets);
  Serial.print("\nSave Changes? Y/N: ");

  //get user entered data
  while (((data = Serial.read()) == -1) || ((data >= 10) && (data <= 15)));

  if ((data == 'Y') || (data == 'y')) {
    for (idx = 0; idx < NUMSERVOS; idx++) {
      SSCSerial.print("R");
      SSCSerial.print(32 + ALL_SERVOS_PINS[idx], DEC);
      SSCSerial.print("=");
      SSCSerial.println(asOffsets[idx], DEC);
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
