User space program to control RGB LED pattern & PWM

   Following project is used to control the PWM and the intensity of RGB led patterns.

Getting Started

    These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. 
    See deployment for notes on how to deploy the project on a live system.

Prerequisites

  Linux kernel (preferably linux 2.6.19 and above)
  GNU (preferably gcc 4.5.0 and above)

Installing

Download below files in user directory on your machine running linux distribution.

   1)RGBLed.c
   2)RGB_lib.h
   3)RGB_ctrl.sh
   4)Makefile


Deployment

   Open the terminal & go to directory where files in installing part have been downloaded. [cd <Directory name>] 

   In the make file we gave the path as "/opt/iot-devkit/1.7.2/sysroots/x86_64-pokysdk-linux/usr/bin/i586-poky-linux" for compiler

   if you have a different location then change it.
   
   Use below command to to compile 
 
   make TEST_TARGET=Galileo2

   Now send the shell script and object file to the galileo
   board using the follwing command (change IP & home
   accordingly)

   sudo scp RGBLed root@192.168.0.100:/home

   sudo scp RGB_ctrl.sh root@192.168.0.100:/home

   Connect to Galileo board with root login

   Mouse events are detected through /dev/input/eventX device
   file. Here header is written for /dev/input/event2 file
   Check on which file on your board & change in RGB_lib.h
   accordingly.

   On Galileo2 board, ensure that 666(rw- rw- rw-) file
   permissions exist for /dev/input/event2.

   You can check by the following command 
   ls -lrt /dev/input/event2. 

   Otherwise change using the following command 
   chmod 666 /dev/input/event2

   Change permission of shell script to 755
   chmod 755 /home/RGB_ctrl.sh

   Once above completed then run the below command to execute
   the program code
   
   cd /home
   sh RGB_ctrl.sh


Expected results

   It asks for input values of the duty cycle and values of pins where the red,green and green are attached on Arduino shield.

   and upon entering those values program displays a sequence of intensities i.e. {R, G, B, R&G, R&B , G&B, R&G&B}

   and when a left click or right click of a mouse is observed the program terminates.
  

Built With

  Linux 4.10.0-28-generic
  x86_64 GNU/Linux
  64 bit x86 machine

Authors

Sarvesh Patil 
Nagarjun chinnari 

License

This project is licensed under the ASU License

