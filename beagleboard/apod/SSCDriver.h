#ifndef _SSC_Driver_h_
#define _SSC_Driver_h_

#include <cstdint>
#include "ssc_config.h"

static const uint8_t cCoxaPin[]  = {cRRCoxaPin, cRMCoxaPin, cRFCoxaPin, cLRCoxaPin, cLMCoxaPin, cLFCoxaPin};
static const uint8_t cFemurPin[] = {cRRFemurPin, cRMFemurPin, cRFFemurPin, cLRFemurPin, cLMFemurPin, cLFFemurPin};
static const uint8_t cTibiaPin[] = {cRRTibiaPin, cRMTibiaPin, cRFTibiaPin, cLRTibiaPin, cLMTibiaPin, cLFTibiaPin};

class SSCDriver {
public:
  int Init();

  float ReadVoltage();

  void OutputServoInfoForLeg(int legIndex, short sCoxaAngle1, short sFemurAngle1, short sTibiaAngle1) const;

  void OutputServoInfoHead(short pan, short tilt, short rot);

  void OutputServoInfoTail(short pan, short tilt);

  void OutputServoInfoMandibles(short left, short right);

  void CommitServoDriver(uint16_t wMoveTime) const;

  void FreeServos() const;

  int Read(char *pb, int cb, uint8_t timeout);

  int ReadByte(uint8_t timeout);

  int Write(const char *data) const;

  [[nodiscard]] int Write(char data) const;

  void Forwarder();
private:
  int fd;

};

#endif //_SSC_Driver_h_
