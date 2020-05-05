#include <fcntl.h>
#include <unistd.h>
#include "PS2X_lib.h"
#include "SSCDriver.h"
#include "ServoOffsets.h"

ServoOffsets::ServoOffsets(SSCDriver *driver, PS2X *ps2) : driver(driver), ps2(ps2) { }

const char * ServoOffsets::ServoName(int idx) {
  switch (idx) {
    case  0: return "RR Coxa ";
    case  1: return "RR Femur";
    case  2: return "RR Tibia";
    case  3: return "RM Coxa ";
    case  4: return "RM Femur";
    case  5: return "RM Tibia";
    case  6: return "RF Coxa ";
    case  7: return "RF Femur";
    case  8: return "RF Tibia";
    case  9: return "LR Coxa ";
    case 10: return "LR Femur";
    case 11: return "LR Tibia";
    case 12: return "LM Coxa ";
    case 13: return "LM Femur";
    case 14: return "LM Tibia";
    case 15: return "LF Coxa ";
    case 16: return "LF Femur";
    case 17: return "LF Tibia";
    case 18: return "HD LMand";
    case 19: return "HD RMand";
    case 20: return "HD Pan  ";
    case 21: return "HD Tilt ";
    case 22: return "HD Rot  ";
    case 23: return "TL Pan  ";
    case 24: return "TL Tilt ";
    default: return "Unknown ";
  }
}

void ServoOffsets::PrintOffsets(int offsets[]) {
  for (int i = 0; i < NUMSERVOS; i++) {
    printf("Servo: %s (%d)\n", ServoName(i), offsets[i]);
  }
}

void ServoOffsets::Run() {
  int bodyPart = 0;
  int offsets[NUMSERVOS];
  int angles[NUMSERVOS];

  uint8_t idx, prevIdx;      // which servo number
  uint16_t buttons = 0;
  char data;
  bool fExit = false;  // when to exit

  for (idx = 0; idx < NUMSERVOS; idx++) {
    offsets[idx] = driver->ReadRegister(32 + ALL_SERVOS_PINS[idx]);
  }
  PrintOffsets(offsets);

  for (idx = 0; idx < NUMSERVOS; idx++) {
    driver->OutputServoAngle(ALL_SERVOS_PINS[idx], 0);
    angles[idx] = 0;
  }
  driver->Commit(200);

  // make reads non-blocking
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

  printf("Find Servo Zeros.\n$-Exit, +- changes, *-change servo, <> - angles\n");
  printf("    0-5 Chooses a leg, C-Coxa, F-Femur, T-Tibia\n");
  printf("    6 Chooses a mandible, L-Left, R-Right\n");
  printf("    7 Chooses head, P-Pan, T-Tilt, R-Rot\n");
  printf("    8 Chooses tail, P-Pan, T-Tilt\n");
  printf("    ps2: []-prev, O-next, pad +/-, R1+L angles\n");

  idx = 0;
  prevIdx = -1;
  while (!fExit) {
    int offset = offsets[idx];
    int angle = angles[idx];
    if (idx != prevIdx) {
      prevIdx = idx;
      printf("Servo: %s (%d)\n", ServoName(idx), offsets[idx]);

      int pwm = DEG2PWM(angle);

      // Now lets wiggle the servo
      driver->OutputServo(ALL_SERVOS_PINS[idx], pwm + 100);
      driver->Commit(100);
      usleep(100 * 1000);
      driver->OutputServo(ALL_SERVOS_PINS[idx], pwm - 100);
      driver->Commit(200);
      usleep(200 * 1000);
      driver->OutputServo(ALL_SERVOS_PINS[idx], pwm);
      driver->Commit(100);
      usleep(100 * 1000);
    }

    if (read(STDIN_FILENO, &data, 1) > 0){
      printf("read: %c\n", data);
      if (data == '$') {
        fExit = true;  // not sure how the keypad will map so give NL, CR, LF... all implies exit
      } else if ((data == '+') || (data == '-')) {
        if (data == '+') {
          offset += 5;    // increment by 5us
        } else {
          offset -= 5;    // increment by 5us
        }
      } else if ((data == '<') || (data == '>')) {
        if (data == '<') {
          angle += 10;
        } else {
          angle -= 10;    // increment by 5us
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
      } else if (data == 't') {
        if (bodyPart == 0) {
          idx = (idx / NUMSERVOSPERLEG) * NUMSERVOSPERLEG + 2;
        } else {
          idx = bodyPart + 1;
        }
      } else if (data == 'f') {
        idx = (idx / NUMSERVOSPERLEG) * NUMSERVOSPERLEG + 1;
      } else if (data == 'r') {
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

    ps2->Poll();
    ps2->GetKeyState();
    if (ps2->state.btnStt) {
      driver->FreeServos();
      return;
    }
    // debounce all buttons
    if (~ps2->state.buttons && !buttons) {
      if (ps2->state.btnCir) {
        idx = (idx + 1) % NUMSERVOS;
      }
      if (ps2->state.btnSqr) {
        idx = (idx - 1 + NUMSERVOS) % NUMSERVOS;
      }
      if (ps2->state.padLeft) {
        offset--;
      }
      if (ps2->state.padRight) {
        offset++;
      }
      if (ps2->state.padUp) {
        offset+= 5;
      }
      if (ps2->state.padDown) {
        offset-= 5;
      }
    }
    buttons = ~ps2->state.buttons;

    if (ps2->state.btnR1) {
      angle += (ps2->state.joyLY - 128) / 8;
      usleep(20 * 1000);
    } else {
      usleep(100 * 1000);
    }

    offset = std::min(std::max(offset, -100), 100);
    angle = std::min(std::max(angle, -900), 900);
    if (prevIdx == idx && offset != offsets[idx]) {
      offsets[idx] = offset;
      printf("    %d\n", offset);
      driver->WriteOffset(idx, offset);
    }
    if (prevIdx == idx && angle != angles[idx]) {
      angles[idx] = angle;
      int pwm = DEG2PWM(angle);
      printf("    %d° (%d)\n", angle, pwm);
      driver->OutputServo(ALL_SERVOS_PINS[idx], pwm);
      driver->Commit(10);
      usleep(10 * 1000);
    }
  }

  printf("Calibrate Servo exit\n");
  PrintOffsets(offsets);
  printf("\nSave Changes? Y/N: \n");

  //get user entered data
  while (((read(STDIN_FILENO, &data, 1)) == -1) || ((data >= 10) && (data <= 15)));

  if ((data == 'Y') || (data == 'y')) {
    for (idx = 0; idx < NUMSERVOS; idx++) {
      driver->WriteRegister(32 + ALL_SERVOS_PINS[idx], offsets[idx]);
    }
    // bit 2 Initial Pulse: Offset Enable; If '1', enables the Initial Pulse Offset register
    driver->Write("R0=4");
    usleep(100 * 1000);

    // reboot
    driver->Reboot();
  }
  driver->FreeServos();
}
