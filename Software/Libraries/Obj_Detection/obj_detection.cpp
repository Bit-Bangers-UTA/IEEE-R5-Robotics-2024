#include <Arduino.h>
#include <stdbool.h>
#include <Adafruit_BNO08x.h>
#include <Wire.h>
#include <math.h>
#include <ESP32MotorControl.h>
#include "motor_control_v2.h"
#include <LiquidCrystal_I2C.h>
#include <string.h>
#include <Servo.h>
#include "SparkFun_VL53L1X.h"
#include "BNO085_heading.h"
#include "VL53L1X_MULTIPLE.h"
#include "lrf.h"
#include "OPT3101_whisker.h"
#include "obj_detection.h"

SFEVL53L1X distanceSensor;
OPT3101 opt3101;

#define MAX_SIZE 10
uint16_t optAverage[MAX_SIZE] = {0};
uint16_t indexDistanceSensor= 0;
uint16_t sumOPT = 0;

double optBuffer(double val)
{
  double avg;
  sumOPT -= optAverage[indexDistanceSensor];
  sumOPT += val;
  optAverage[indexDistanceSensor] = val;
  avg = (double)(sumOPT/MAX_SIZE);
  indexDistanceSensor = (indexDistanceSensor + 1) % MAX_SIZE;
  return avg;
}

void fillFirstBuffer()
{
  for(int i = 0; i < MAX_SIZE; i++)
  {
    optBuffer(getWhiskerDistanceCm());
  }
}

float getVL53L1XDistanceCm()
{
  while(!distanceSensor.checkForDataReady())
  {
    delay(1);
  }
  float distance = distanceSensor.getDistance()/10;
  distanceSensor.clearInterrupt();
  return distance;
}


/*
This function is for debugging purposes only. It will print out the distance
*/
void wallDetection()
{
  float distance = getWhiskerDistanceCm();
  if(distance < 20)
  {
    if(getVL53L1XDistanceCm() < 25)
    {
      Serial.println("Wall detected");
    }
    else
    {
      Serial.println("Object detected");
    }
  }
  else
  {
    Serial.println("No wall or object detected");
  }
}

extern float X_POS;
extern float Y_POS ;


// X_POS = 10;
// Y_POS = 10;
// nextX = 0;
// nextY = 0;


/*
* @param pseudo: Must always be set, but is not used
* @description: acts as a blocking function to goToCoordinates in order to avoid obstacles, then returns
* to the calling goToCoordinates function
*/
void wallDetection(float nextX, float nextY)
{
  float optDistance; // in cm
  if(X_POS < 60.48 && Y_POS < 60.48)// if x and y are less than 2ft
  {
    optDistance = getWhiskerDistanceCm();
    if (optDistance < 15)
    {
    //   optDistance = getWhiskerDistanceCm() --> redundant?
    //   if(optDistance < 15)
    //   {
    while(optDistance < 15)// turn until we face a direction with no obstacles
    {
        Serial.printf("wallDetection => optDistance < 15 : %f\n", optDistance);
        turn(CLOCKWISE, 65);
        delay(500);
        optDistance = getWhiskerDistanceCm();// update distance
    }
    //   }
      stopMotors();
      delay(500); // delay to ensure the robot has stopped (robot has wobble when stopping)
      move(FORWARD, 255);
      delay(500);
      stopMotors();
    //   goToCoordinates(COORDINATES); no garbage collector, stack gets full, void returns to parent function
    }
  }
  else
  {
    distanceSensor.stopRanging();
  }
}


