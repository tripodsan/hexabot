#include <fcntl.h>
#include <cstdio>
#include <unistd.h>
#include <sys/termios.h>
#include <cstring>
#include <cstdlib>
#include "SSCDriver.h"

int SSCDriver::Init() {
  printf("Init servo driver\n");

  if ((fd = open("/dev/ttyO1", O_RDWR | O_NOCTTY | O_NDELAY))<0){
    perror("UART: Failed to open the file.\n");
    return -1;
  }

  struct termios termios{};
  tcgetattr(fd, &termios);
  termios.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
  termios.c_iflag = IGNPAR | ICRNL;
  termios.c_lflag &= ~ICANON;
  termios.c_cc[VTIME] = 100;
  tcsetattr(fd, TCSANOW, &termios);
  tcflush(fd, TCIFLUSH);

  if (Write("ver\r") < 0){
    return -1;
  }
  usleep(1000 * 200);

  char buffer[256];
  int cbRead = Read(buffer, sizeof(buffer), 100);
  if (cbRead <= 0) {
    printf("SSC Version: n/a\n");
    return -1;
  }

  printf("SSC Version: %s\n", buffer);
  return 0;
}

int SSCDriver::Write(const char *data) const {
  if (write(fd, data, strlen(data)) < 0){
    perror("Error: Failed to write to the SSC");
    return -1;
  }
  return 0;
}

int SSCDriver::Write(char data) const {
  if (write(fd, &data, 1) < 0){
    perror("Error: Failed to write to the SSC");
    return -1;
  }
  return 0;
}

float SSCDriver::ReadVoltage() {
  const char data[] = { '\r', 'V', cSSC_ADC_VOLT, '\r', 0 };
  if (Write(data) < 0) {
    return 0;
  };
  int rd = ReadByte(100);
  if (rd < 0) {
//    DBGSerial.println(F("Timeout."));
    return 0;
  }
  float v = rd / 10.2f;
//  DBGSerial.print(v);
//  DBGSerial.println(F("v\n"));
  return v;
}

void SSCDriver::WiggleServo(int idx, float angle) {
  int pwm = DEG2PWM(angle);
  OutputServo(idx, pwm + 100);
  Commit(100);
  usleep(100 * 1000);
  OutputServo(idx, pwm - 100);
  Commit(200);
  usleep(200 * 1000);
  OutputServo(idx, pwm);
  Commit(100);
  usleep(100 * 1000);
}

void SSCDriver::OutputServoLeg(int legIdx, float coxa, float femur, float tibia) const {
  uint16_t coxaPWM  = DEG2PWM(coxa);
  uint16_t femurPWM = DEG2PWM(femur);
  uint16_t tibiaPWM = DEG2PWM(tibia);

  uint8_t data[] = {
      static_cast<uint8_t>(cCoxaPin[legIdx] + 0x80),
      static_cast<uint8_t>(coxaPWM >> 8u),
      static_cast<uint8_t>(coxaPWM & 0xffu),
      static_cast<uint8_t>(cFemurPin[legIdx] + 0x80),
      static_cast<uint8_t>(femurPWM >> 8u),
      static_cast<uint8_t>(femurPWM & 0xffu),
      static_cast<uint8_t>(cTibiaPin[legIdx] + 0x80),
      static_cast<uint8_t>(tibiaPWM >> 8u),
      static_cast<uint8_t>(tibiaPWM & 0xffu),
  };
  if (write(fd, data, sizeof(data)) < 0){
    perror("Error: Failed to write to the SSC");
  }
}

void SSCDriver::OutputServoHead(float pan, float tilt, float rot) {
  uint16_t panPWM = DEG2PWM(pan);
  uint16_t tltPWM = DEG2PWM(tilt);
  uint16_t rotPWM = DEG2PWM(rot);

  uint8_t data[] = {
      cHeadPanPin  + 0x80,
      static_cast<uint8_t>(panPWM >> 8u),
      static_cast<uint8_t>(panPWM & 0xffu),
      cHeadTiltPin  + 0x80,
      static_cast<uint8_t>(tltPWM >> 8u),
      static_cast<uint8_t>(tltPWM & 0xffu),
      cHeadRotPin  + 0x80,
      static_cast<uint8_t>(rotPWM >> 8u),
      static_cast<uint8_t>(rotPWM & 0xffu),
  };
  if (write(fd, data, sizeof(data)) < 0){
    perror("Error: Failed to write to the SSC");
  }
}

void SSCDriver::OutputServoTail(float pan, float tilt) {
  uint16_t panPWM = DEG2PWM(pan);
  uint16_t tltPWM = DEG2PWM(tilt);
  uint8_t data[] = {
      cHeadPanPin  + 0x80,
      static_cast<uint8_t>(panPWM >> 8u),
      static_cast<uint8_t>(panPWM & 0xffu),
      cHeadTiltPin  + 0x80,
      static_cast<uint8_t>(tltPWM >> 8u),
      static_cast<uint8_t>(tltPWM & 0xffu),
  };
  if (write(fd, data, sizeof(data)) < 0){
    perror("Error: Failed to write to the SSC");
  }
}

/**
 * updates the mandibles
 * @param left Left mandible angle in degrees.
 * @param right Right mandible angle in degrees.
 */
void SSCDriver::OutputServoMandibles(float left, float right) {
  uint16_t leftPWM = DEG2PWM(-left);
  uint16_t rightPWM = DEG2PWM(-right);

  uint8_t data[] = {
      cHeadPanPin  + 0x80,
      static_cast<uint8_t>(leftPWM >> 8u),
      static_cast<uint8_t>(leftPWM & 0xffu),
      cHeadTiltPin  + 0x80,
      static_cast<uint8_t>(rightPWM >> 8u),
      static_cast<uint8_t>(rightPWM & 0xffu),
  };
  if (write(fd, data, sizeof(data)) < 0){
    perror("Error: Failed to write to the SSC");
  }
}

void SSCDriver::OutputServo(int idx, uint16_t duty) {
  uint8_t data[] = {
      static_cast<uint8_t>(idx  + 0x80),
      static_cast<uint8_t>(duty >> 8u),
      static_cast<uint8_t>(duty & 0xffu),
  };
  if (write(fd, data, sizeof(data)) < 0){
    perror("Error: Failed to write to the SSC");
  }
}

void SSCDriver::OutputServoAngle(int idx, float angle) {
  OutputServo(idx, DEG2PWM(angle));
}


void SSCDriver::Commit(uint16_t wMoveTime) const {
  uint8_t data[] = {
      0xA1,
      static_cast<uint8_t>(wMoveTime >> 8u),
      static_cast<uint8_t>(wMoveTime & 0xffu),
  };
  if (write(fd, data, sizeof(data)) < 0){
    perror("Error: Failed to write to the SSC");
  }
}

void SSCDriver::FreeServos() const {
  uint8_t data[32*3];
  memset(data, 0, sizeof(data));
  for (uint16_t idx = 0; idx < 32; idx++) {
    data[idx * 3] = idx + 0x80u;
  }
  if (write(fd, data, sizeof(data)) < 0){
    perror("Error: Failed to write to the SSC");
  }
  Commit(200);
}

int SSCDriver::ReadRegister(int reg) {
  char buff[16];
  sprintf(buff, "R%d\r", reg);
  if (Write(buff) < 0) {
    return 0;
  }
  usleep(10 * 1000);
  int rd = Read(buff, sizeof(buff), 100);
  if (rd < 0) {
    perror("Error: Reading from SSC");
    return 0;
  }
  return atoi(buff);
}

int SSCDriver::WriteRegister(int reg, int value) {
  char buff[16];
  sprintf(buff, "R%d=%d\r", reg, value);
  int rd = Write(buff);
  usleep(10 * 1000);
  return rd;
}

int SSCDriver::WriteOffset(int idx, int offset) {
  char buff[16];
  sprintf(buff, "#%dPO%d\r", idx, offset);
  return Write(buff);
}

int SSCDriver::Read(char *pb, int cb, uint8_t timeout) {
  memset(pb, 0, cb);
  struct termios termios;
  tcgetattr(fd, &termios);
  termios.c_cc[VTIME] = timeout;
  termios.c_cc[VMIN] = 0;
  tcsetattr(fd, TCSANOW, &termios);

  char *pbIn = pb;
  while (cb) {
    int rd = read(fd, pbIn, 1);
    if (rd < 0) {
      perror("Error: Reading from SSC");
      break;
    }
    cb--;
    if (*pbIn++ == '\n') {
      break;
    }
  }
  return (int) (pbIn - pb);
}

/**
 * Reads a byte from the serial with timeout
 * @param timeout timeout in 1/10 seconds
 * @return the byte or -1 if timeout exceeded
 */
int SSCDriver::ReadByte(uint8_t timeout) {
  struct termios termios;
  tcgetattr(fd, &termios);
  termios.c_cc[VTIME] = timeout;
  tcsetattr(fd, TCSANOW, &termios);
  uint8_t c;
  if (read(fd, &c, 1) < 0) {
    return -1;
  }
  return c;
}

void SSCDriver::Reboot() {
  Write("GOBOOT\r");
  usleep(10 * 100);
  Write("g0000\r");
  usleep(500 * 1000);
}


void SSCDriver::Forwarder() {
  bool maybeExit = false;
  bool maybeValue = false;
  unsigned char c = 0;

  // make reads non-blocking
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

  printf("SSC Forwarder mode - Enter $<cr> to exit\n");
  while (true) {
    if(read(STDIN_FILENO, &c, 1) > 0){
//      printf(">> %d", c);
      if ((c == '\n') || (c == '\r')) {
        c = '\r';
        if (maybeExit) {
          break;
        }
      } else if (c == '$') {
        maybeExit = true;
      } else if (c == 'V') {
        maybeValue = true;
        maybeExit = false;
      } else {
        maybeExit = false;
      }
//      printf(" (%d)\n", c);
      write(fd, &c, 1);
    }

    if(read(fd, &c, 1) > 0) {
//      printf("<< %d\n", c);
      if (maybeValue) {
        printf("\nValue: %d\n", c);
        // write extra <cr>
        c = 13;
        write(fd, &c, 1);
      } else {
        write(STDOUT_FILENO, &c, 1);
      }
      maybeValue = false;
    }
  }

  printf("Exited SSC Forwarder mode\n");
}


