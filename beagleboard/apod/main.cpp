#include <unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<termios.h>
#include<string.h>
#include<stdlib.h>
#include <robotcontrol.h>

#include "PS2X_lib.h"
#include "SSCDriver.h"
#include "ServoOffsets.h"
#include "HexaPod.h"

using namespace std;

int ps2_test(){
  SPIDevice spi(1,1);
  spi.setSpeed(100000);
  spi.setMode(SPIDevice::MODE3);
  spi.setLSBFirst(1);
  spi.debugDump();

  PS2X ps2x(&spi);

  ps2x.SetADMode(true, false);

  ps2x.Poll();
  ps2x.GetKeyState();
  ps2x.dump();

  ps2x.Poll();
  ps2x.GetKeyState();
  ps2x.dump();

  ps2x.SetEnableMotor(false, true);
  ps2x.SetMotorLevel(0, 255);
  while (true) {
    ps2x.Poll();
    ps2x.GetKeyState();
    ps2x.dump();
    usleep(100*1000);
    ps2x.SetMotorLevel(0, 0);
  }
}

// Sends a message to the client and displays the message on the console
int message(int client, const char *message){
  int size = strlen(message);
  printf("Server>>>%s\n", (message+1));   // print message with new line
  if (write(client, message, size)<0){
    perror("Error: Failed to write to the client\n");
    return -1;
  }
  write(client, "\n\rEBB>", 7);           // display a simple prompt
  return 0;                               // \r for a carriage return
}
int uart_main(int argc, char *argv[]){
  int client, count=0;
  unsigned char c;
  char *command = (char*) malloc(255);

  if ((client = open("/dev/ttyO1", O_RDWR | O_NOCTTY | O_NDELAY))<0){
    perror("UART: Failed to open the file.\n");
    return -1;
  }
  struct termios options;
  tcgetattr(client, &options);
  options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
  options.c_iflag = IGNPAR | ICRNL;
  tcflush(client, TCIFLUSH);
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);  // make reads non-blocking
  tcsetattr(client, TCSANOW, &options);
  if (message(client, "ver\r")<0){
    perror("UART: Failed to start server.\n");
    return -1;
  }

  // Loop forever until the quit command is sent from the client or
  //  Ctrl-C is pressed in the server's terminal window
  do {
    if(read(client,&c,1)>0){
      write(STDOUT_FILENO,&c,1);
      command[count++]=c;
      if(c=='\n'){
        command[count]='\0';  // replace /n with /0
        printf("response: %s", command);
        //processCommand(client, command);
        count=0;                // reset the command string
      }
    }
    if(read(STDIN_FILENO,&c,1)>0){ // can send from stdin to client
      write(client,&c,1);
      if (c == '\n') {
        c = '\r';
        write(client, &c, 1);
      }
    }
  }
  while(strcmp(command,"quit")!=0);
  close(client);
  return 0;
}

int fwd_main() {
  SSCDriver driver;
  driver.Init();
  driver.Forwarder();
  return 0;
}

int calibrate() {
  SPIDevice spi(1,1);
  spi.setSpeed(100000);
  spi.setMode(SPIDevice::MODE3);
  spi.setLSBFirst(1);
  spi.debugDump();
  PS2X ps2x(&spi);
  ps2x.SetADMode(true, true);

  SSCDriver driver{};
  if (driver.Init() < 0) {
    return -1;
  }

  ServoOffsets offsets(&driver, &ps2x);
  offsets.Run();
  return 0;
}

void ik_loop(PS2X *ps2, SSCDriver *driver) {
  HexaPod pod{};

  int idx = 1;
  int lastIdx = 0;
  uint16_t buttons = 0;
  while (true) {
    ps2->Poll();
    ps2->GetKeyState();
    if (ps2->state.btnStt) {
      break;
    }
    if (~ps2->state.buttons && !buttons) {
      if (ps2->state.btnCir) {
        idx = (idx + 1) % 6;
      }
      if (ps2->state.btnSqr) {
        idx = (idx + 5) % 6;
      }
    }
    buttons = ~ps2->state.buttons;

    if (idx != lastIdx) {
      lastIdx = idx;
      driver->WiggleServo(ALL_SERVOS_PINS[idx*3], pod.legs[idx].ac);
    }

    pod.legs[idx].x += (ps2->state.joyLX - 128) / 100.0f;
    pod.legs[idx].y -= (ps2->state.joyLY - 127) / 100.0f;
    pod.legs[idx].z -= (ps2->state.joyRY - 127) / 100.0f;

    pod.legs[idx].IK();

    driver->OutputServoLeg(idx, pod.legs[idx].ac, pod.legs[idx].af, pod.legs[idx].at);
    driver->Commit(10);
    usleep(10*1000);
  }

  // power off
  pod.PowerOff();
  for (int i = 0; i < 6; i++) {
    driver->OutputServoLeg(i, pod.legs[i].ac, pod.legs[i].af, pod.legs[i].at);
  }
  driver->Commit(600);
  usleep(800*1000);
  driver->FreeServos();
}

void gait_loop(PS2X *ps2, SSCDriver *driver) {
  HexaPod pod{};

  while (rc_get_state() != EXITING) {
    pod.Step();
    for (int i=0; i < 6; i++) {
      driver->OutputServoLeg(i, pod.legs[i].ac, pod.legs[i].af, pod.legs[i].at);
    }
    driver->Commit(pod.gait.gait->stepTime);
    usleep(pod.gait.gait->stepTime*1000);

    char data;
    while (read(STDIN_FILENO, &data, 1) <= 0) {
      printf("read: %c\n", data);
    }
    if (data >= '0' && data <= '9') {
      pod.gait.Select(data - '0');
    }
    if (data == '$') {
      rc_set_state(EXITING);
    }
  }

  // power off
  pod.PowerOff();
  for (int i = 0; i < 6; i++) {
    driver->OutputServoLeg(i, pod.legs[i].ac, pod.legs[i].af, pod.legs[i].at);
  }
  driver->Commit(600);
  usleep(800*1000);
  driver->FreeServos();
}

int ik() {
  SPIDevice spi(1,1);
  spi.setSpeed(100000);
  spi.setMode(SPIDevice::MODE3);
  spi.setLSBFirst(1);
  spi.debugDump();

  PS2X ps2x(&spi);
  ps2x.SetADMode(true, true);

  SSCDriver driver{};
  if (driver.Init() < 0) {
    return -1;
  }

  ik_loop(&ps2x, &driver);
  return 0;
}

int gait() {
  SPIDevice spi(1,1);
  spi.setSpeed(100000);
  spi.setMode(SPIDevice::MODE3);
  spi.setLSBFirst(1);
  spi.debugDump();

  PS2X ps2x(&spi);
  ps2x.SetADMode(true, true);

  SSCDriver driver{};
  if (driver.Init() < 0) {
    return -1;
  }

  gait_loop(&ps2x, &driver);
  return 0;
}

int run(int argc, char *argv[]){
  if (argc > 1 && !strcmp(argv[1], "ps2")) {
    return ps2_test();
  }
  if (argc > 1 && !strcmp(argv[1], "fwd")) {
    return fwd_main();
  }
  if (argc > 1 && !strcmp(argv[1], "uart")) {
    return uart_main(argc, argv);
  }
  if (argc > 1 && !strcmp(argv[1], "cali")) {
    return calibrate();
  }
  if (argc > 1 && !strcmp(argv[1], "ik")) {
    return ik();
  }
  if (argc > 1 && !strcmp(argv[1], "gait")) {
    return gait();
  }
  fprintf(stderr, "usage: %s (ps2|fwd|uart|cali|ik|gait)\n", argv[0]);
  return -1;
}

int main(int argc, char *argv[]) {
  if (rc_kill_existing_process(2.0) < -2) return -1;

  // start signal handler so we can exit cleanly
  if (rc_enable_signal_handler() == -1) {
    fprintf(stderr, "ERROR: failed to start signal handler\n");
    return -1;
  }
  rc_make_pid_file();
  rc_set_state(RUNNING);
  int ret = run(argc, argv);
  rc_remove_pid_file();  // remove pid file LAST
  return ret;
}

