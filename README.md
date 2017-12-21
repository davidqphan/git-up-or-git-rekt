## Git Up or Git Rekt

# Author - David Phan

## Overview
Git Up or Git Rekt is an alarm clock game that assists in waking you up completely.

The user has to fulfill two requirements to turn off the alarm clock:
* the user must score an X amount of points via a shooting board (i.e. 10 points) 
* However, in order for the points to count, the user must be stationary on a Wii balance board at all times

## Demo Video
https://www.youtube.com/watch?v=DCt4jXcL5Wg

## Technology
* Embedded systems programming AVR microcontrollers in C
* Universal Synchronous/Asynchronous Receiver/Transmitter (USART) communication

## Components
* Wii balance board
* Raspberry Pi 3

* Shooting board (Servant)
  * Atmega1284 microcontroller
  * IR sensors
  * HC-SR04 Ultrasonic sensor

* Alarm clock (Master)
  * Arduino Uno
  * DS 1307 RTC (real time clock)
  * 16x2 LCD display
  * Speaker
  * 2 10k resistors
  * 10k potentiometer
  
## High Level Diagram
![high_level_diagram](https://cloud.githubusercontent.com/assets/15112219/20460127/ce76a72a-ae8e-11e6-8f14-e1fbf3f7f55b.png)

