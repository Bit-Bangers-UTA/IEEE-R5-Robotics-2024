/*
  Bit Bangers Modified SparkFun_TB6612.cpp for ELEGOO SmartCar-Shield-V1.1
  github.com/munozr1/Senior-Design

  Changed:
  Remove In2pin as already provided by shield inverter chip
  Remove references to motor2 (we're only using one motor)
  Remove left and right functions (just fwd and bckwd)
*/

/******************************************************************************
TB6612.cpp
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

#include "TB6612_SmartCar.h"
#include <Arduino.h>

Motor::Motor(int In1pin, int PWMpin, int offset, int STBYpin)
{
  In1 = In1pin;
  PWM = PWMpin;
  Standby = STBYpin;
  Offset = offset;

  pinMode(In1, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(Standby, OUTPUT);
}

void Motor::drive(int speed)
{
  digitalWrite(Standby, HIGH);
  speed = speed * Offset;
  if (speed >= 0)
    fwd(speed);
  else
    rev(-speed);
}

void Motor::drive(int speed, int duration)
{
  drive(speed);
  delay(duration);
}

void Motor::fwd(int speed)
{
  digitalWrite(In1, HIGH);
  analogWrite(PWM, speed);
}

void Motor::rev(int speed)
{
  digitalWrite(In1, LOW);
  analogWrite(PWM, speed);
}

void Motor::brake()
{
  digitalWrite(In1, HIGH);
  analogWrite(PWM, 0);
}

void Motor::standby()
{
  digitalWrite(Standby, LOW);
}

void forward(Motor motor1, int speed)
{
  motor1.drive(speed);
}
void forward(Motor motor1)
{
  motor1.drive(DEFAULTSPEED);
}

void back(Motor motor1, int speed)
{
  int temp = abs(speed);
  motor1.drive(-temp);
}
void back(Motor motor1)
{
  motor1.drive(-DEFAULTSPEED);
}

void brake(Motor motor1)
{
  motor1.brake();
}