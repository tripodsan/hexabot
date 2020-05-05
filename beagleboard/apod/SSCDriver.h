#ifndef _SSC_Driver_h_
#define _SSC_Driver_h_

#include <cstdint>
#include "ssc_config.h"

const static uint8_t cCoxaPin[]  = {cRRCoxaPin, cRMCoxaPin, cRFCoxaPin, cLRCoxaPin, cLMCoxaPin, cLFCoxaPin};
const static uint8_t cFemurPin[] = {cRRFemurPin, cRMFemurPin, cRFFemurPin, cLRFemurPin, cLMFemurPin, cLFFemurPin};
const static uint8_t cTibiaPin[] = {cRRTibiaPin, cRMTibiaPin, cRFTibiaPin, cLRTibiaPin, cLMTibiaPin, cLFTibiaPin};
const static uint8_t ALL_SERVOS_PINS[] = {
    cRRCoxaPin, cRRFemurPin, cRRTibiaPin, cRMCoxaPin, cRMFemurPin, cRMTibiaPin, cRFCoxaPin, cRFFemurPin, cRFTibiaPin,
    cLRCoxaPin, cLRFemurPin, cLRTibiaPin, cLMCoxaPin, cLMFemurPin, cLMTibiaPin, cLFCoxaPin, cLFFemurPin, cLFTibiaPin,
    cLMandPin, cRMandPin, cHeadPanPin, cHeadTiltPin, cHeadRotPin, cTailPanPin, cTailTiltPin
};

class SSCDriver {
public:
  int Init();

  float ReadVoltage();

  void OutputServo(int idx, uint16_t duty);

  void OutputServoAngle(int idx, short angle);

  void OutputServoInfoForLeg(int legIndex, short sCoxaAngle1, short sFemurAngle1, short sTibiaAngle1) const;

  void OutputServoInfoHead(short pan, short tilt, short rot);

  void OutputServoInfoTail(short pan, short tilt);

  void OutputServoInfoMandibles(short left, short right);

  void Commit(uint16_t wMoveTime) const;

  void FreeServos() const;

  int Read(char *pb, int cb, uint8_t timeout);

  int ReadByte(uint8_t timeout);

  int Write(const char *data) const;

  [[nodiscard]] int Write(char data) const;

  int WriteOffset(int idx, int offset);

  int ReadRegister(int reg);

  int WriteRegister(int reg, int value);

  void Forwarder();

  void Reboot();

private:
  int fd;

};

#endif //_SSC_Driver_h_
