/*
  Bit Bangers Modified MotorTestRun.ino for ELEGOO SmartCar-Shield-V1.1
  Use this as a test program. The library is in lib/TB6612_SmartCar
  github.com/munozr1/Senior-Design

  ESP32-C3 to SmartCar Pinout:
  STBY: 0 -> 3
  PWMA: 1 -> 5
  AIN1: 2 -> 7
*/

/******************************************************************************
TestRun.ino
TB6612FNG H-Bridge Motor Driver Example code
Michelle @ SparkFun Electronics
8/20/16
https://github.com/sparkfun/SparkFun_TB6612FNG_Arduino_Library

Uses 2 motors to show examples of the functions in the library.  This causes
a robot to do a little 'jig'.  Each movement has an equal and opposite movement
so assuming your motors are balanced the bot should end up at the same place it
started.

Resources:
TB6612 SparkFun Library

Development environment specifics:
Developed on Arduino 1.6.4
Developed with ROB-9457
******************************************************************************/

#include <Arduino.h>
#include <TB6612_SmartCar.h>

#define STBY 0
#define PWMA 1
#define AIN1 2

const int offsetA = 1;

Motor motor1 = Motor(AIN1, PWMA, offsetA, STBY);

void setup()
{
  // Nothing here
}

void loop()
{
  // Use of the drive function which takes as arguements the speed
  // and optional duration.  A negative speed will cause it to go
  // backwards.  Speed can be from -255 to 255.  Also use of the
  // brake function which takes no arguements.
  motor1.drive(255, 1000);
  motor1.drive(-255, 1000);
  motor1.brake();
  delay(1000);

  // Use of the forward function, which takes as arguements two motors
  // and optionally a speed.  If a negative number is used for speed
  // it will go backwards
  forward(motor1, 150);
  delay(1000);

  // Use of the back function, which takes as arguments two motors
  // and optionally a speed.  Either a positive number or a negative
  // number for speed will cause it to go backwards
  back(motor1, -150);
  delay(1000);

  // Use of the brake function which takes as arguments two motors.
  // Note that functions do not stop motors on their own.
  brake(motor1);
  delay(1000);

  // Use of brake again.
  brake(motor1);
  delay(1000);
}