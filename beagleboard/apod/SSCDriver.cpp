#include <fcntl.h>
#include <cstdio>
#include <unistd.h>
#include <sys/termios.h>
#include <cstring>
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
  usleep(1000 * 100);

  char buffer[256];
  int cbRead = Read(buffer, sizeof(buffer), 100);
  if (cbRead <= 0) {
    printf("SSC Version: n/a\n");
  } else {
    printf("SSC Version: %s\n", buffer);
  }
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

void SSCDriver::OutputServoInfoForLeg(int legIdx, short sCoxaAngle1, short sFemurAngle1, short sTibiaAngle1) const {
  uint16_t wCoxaSSCV;
  uint16_t wFemurSSCV;
  uint16_t wTibiaSSCV;

  //Update Right Legs
  if (legIdx < 3) {
    wCoxaSSCV  = DEG2PWM(-sCoxaAngle1);
    wFemurSSCV = DEG2PWM(-sFemurAngle1);
    wTibiaSSCV = DEG2PWM(-sTibiaAngle1);
  } else {
    wCoxaSSCV  = DEG2PWM(sCoxaAngle1);
    wFemurSSCV = DEG2PWM(sFemurAngle1);
    wTibiaSSCV = DEG2PWM(sTibiaAngle1);
  }

  uint8_t data[] = {
      static_cast<uint8_t>(cCoxaPin[legIdx] + 0x80),
      static_cast<uint8_t>(wCoxaSSCV >> 8u),
      static_cast<uint8_t>(wCoxaSSCV & 0xffu),
      static_cast<uint8_t>(cFemurPin[legIdx] + 0x80),
      static_cast<uint8_t>(wFemurSSCV >> 8u),
      static_cast<uint8_t>(wFemurSSCV & 0xffu),
      static_cast<uint8_t>(cTibiaPin[legIdx] + 0x80),
      static_cast<uint8_t>(wTibiaSSCV >> 8u),
      static_cast<uint8_t>(wTibiaSSCV & 0xffu),
  };
  if (write(fd, data, sizeof(data)) < 0){
    perror("Error: Failed to write to the SSC");
  }
}

void SSCDriver::OutputServoInfoHead(short pan, short tilt, short rot) {
  uint16_t wPanSSCV = DEG2PWM(pan);
  uint16_t wTiltSSCV = DEG2PWM(tilt);
  uint16_t wRotSSCV = DEG2PWM(rot);

  uint8_t data[] = {
      cHeadPanPin  + 0x80,
      static_cast<uint8_t>(wPanSSCV >> 8u),
      static_cast<uint8_t>(wPanSSCV & 0xffu),
      cHeadTiltPin  + 0x80,
      static_cast<uint8_t>(wTiltSSCV >> 8u),
      static_cast<uint8_t>(wTiltSSCV & 0xffu),
      cHeadRotPin  + 0x80,
      static_cast<uint8_t>(wRotSSCV >> 8u),
      static_cast<uint8_t>(wRotSSCV & 0xffu),
  };
  if (write(fd, data, sizeof(data)) < 0){
    perror("Error: Failed to write to the SSC");
  }
}

void SSCDriver::OutputServoInfoTail(short pan, short tilt) {
  uint16_t wPanSSCV = DEG2PWM(pan);
  uint16_t wTiltSSCV = DEG2PWM(tilt);
  uint8_t data[] = {
      cHeadPanPin  + 0x80,
      static_cast<uint8_t>(wPanSSCV >> 8u),
      static_cast<uint8_t>(wPanSSCV & 0xffu),
      cHeadTiltPin  + 0x80,
      static_cast<uint8_t>(wTiltSSCV >> 8u),
      static_cast<uint8_t>(wTiltSSCV & 0xffu),
  };
  if (write(fd, data, sizeof(data)) < 0){
    perror("Error: Failed to write to the SSC");
  }
}

/**
 * updates the mandibles
 * @param left Left mandible angle in degrees. (1 decimal)
 * @param right Right mandible angle in degrees. (1 decimal)
 */
void SSCDriver::OutputServoInfoMandibles(short left, short right) {
  uint16_t wLeftSSCV = DEG2PWM(-left);
  uint16_t wRightSSCV = DEG2PWM(-right);

  uint8_t data[] = {
      cHeadPanPin  + 0x80,
      static_cast<uint8_t>(wLeftSSCV >> 8u),
      static_cast<uint8_t>(wLeftSSCV & 0xffu),
      cHeadTiltPin  + 0x80,
      static_cast<uint8_t>(wRightSSCV >> 8u),
      static_cast<uint8_t>(wRightSSCV & 0xffu),
  };
  if (write(fd, data, sizeof(data)) < 0){
    perror("Error: Failed to write to the SSC");
  }
  this->CommitServoDriver(wMandibleTime);
}

void SSCDriver::CommitServoDriver(uint16_t wMoveTime) const {
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
  CommitServoDriver(200);
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


