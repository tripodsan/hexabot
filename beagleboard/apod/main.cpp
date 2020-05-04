#include <iostream>
#include <sstream>
#include <unistd.h>
#include "PS2X_lib.h"
#include "SSCDriver.h"


#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>
#include<stdlib.h>

using namespace std;

int spi_test_main(){
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


int main(int argc, char *argv[]){
  if (argc > 1 && !strcmp(argv[1], "spi")) {
    return spi_test_main();
  }
  if (argc > 1 && !strcmp(argv[1], "fwd")) {
    return fwd_main();
  }
  if (argc > 1 && !strcmp(argv[1], "uart")) {
    return uart_main(argc, argv);
  }
  fprintf(stderr, "usage: %s [spi|fwd|uart", argv[0]);
  return -1;
}
