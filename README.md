# Hexabot

## Centering the servos

- install [Sequencer Utility](https://www.robotshop.com/en/ssc-32-servo-sequencer-utility.html) on VMWare Fusion
- install [Virtual Com Drivers](https://www.ftdichip.com/Drivers/VCP.htm) on Mac
- connect SSCU-32 to mac and check devices:
```
$ ls -al /dev/tty.usb*
crw-rw-rw-  1 root  wheel  -  18,  12 Apr  1 16:33 /dev/tty.usbmodem001NTVSAG9592
crw-rw-rw-  1 root  wheel  -  18,  16 Apr  1 17:34 /dev/tty.usbserial-8
crw-rw-rw-  1 root  wheel  -  18,  14 Apr  1 17:33 /dev/tty.usbserial-AK06IHBU
```
- install socat: `$ brew install socat`
- edit vm vmx:
```
serial1.present = "TRUE"
serial1.fileType = "pipe"
serial1.fileName = "/tmp/serial1"
serial1.startConnected = "TRUE"
serial1.tryNoRxLoss = "FALSE"
serial1.autodetect = "FALSE"
serial1.pipe.endPoint = "client"
```
- start VM
- start socat:
```
socat /dev/tty.usbserial-AK06IHBU,cread=1,clocal=1,cs8,nonblock=1 unix-listen:/tmp/serial1
```
- install Sequencer Utility in windows and start it
- attach battery to VS1 and set VS=VL jumper
- attach servo
- turn on power
- Utility should now autodetect the SSCU-32 on com2

### Links

- http://www.lynxmotion.com/c-154-a-pod.aspx
- http://www.lynxmotion.com/driver.aspx?Topic=assem04#apod

