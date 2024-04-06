#include <Arduino.h>
#include <stdbool.h>
#include <Adafruit_BNO08x.h>
#include <Wire.h>
#include <math.h>
#include <ESP32MotorControl.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>

#include "BNO085_heading.h"
#include "motor_control_v2.h"
// #include "printToLcd.h"

extern ESP32MotorControl motors;
// extern int16_t A_LEFT_MOTOR_OFFSET;
// extern int16_t B_RIGHT_MOTOR_OFFSET;
extern Adafruit_BNO08x bno08x;
extern sh2_SensorValue_t sensorValue;
extern float offsetForImu;

void setReports(Adafruit_BNO08x *bno08x, sh2_SensorId_t reportType, uint32_t interval)
{
  // Serial.println("Setting desired reports");
  if (!bno08x->enableReport(reportType, interval)) // Top frequency is about 250Hz but this report is more accurate
  {
    Serial.println("Could not enable stabilized remote vector");
  }
  Serial.println("Reports set");
  // int status = sh2_setSensorConfig(sensorId, &config);
  // if (status != 0)
  // {
  //   Serial.print("Error setting sensor config: ");
  //   Serial.println(status);
  // }
}

void reports(Adafruit_BNO08x *bno08x)
{
  setReports(bno08x, SH2_ROTATION_VECTOR, 5000);
}

void setupBNO085(Adafruit_BNO08x *bno08x, uint8_t i2c_address, TwoWire *wire,
                                int32_t sensor_ids)
{
  //Wire1.begin(20, 21);
  delay(1000);
  while (!bno08x->begin_I2C(i2c_address, wire, sensor_ids))
  {
    ESP.restart();
    // delay(1000);
    Serial.println("BNO085 Not initialized\n");
    // bno08x->hardwareReset();


    /*
    while (1)
    {
      delay(10);
    }
    */
  }
  Serial.println("BNO085 Initialized\n");

  reports(bno08x);
  delay(1000);
}

// Function to calculate the heading of the robot from quaternion values
float calculateHeading(float i, float j, float k, float real)
{
  float yaw = atan2(2.0f * (i * j + real * k), real * real + i * i - j * j - k * k);
  yaw *= 180.0f / M_PI;
  // Normalize heading angle
  if (yaw < 0)
  {
    yaw += 359.99f;
  }
  return yaw;
}

void checkReset(Adafruit_BNO08x *bno08x)
{
  if (bno08x->wasReset())
  {
    // Serial.print("sensor was reset ");
    reports(bno08x);
  }
}

// Function to get the current heading of the robot
float getHeading()
{
  float retVal = -1;

  if (bno08x.getSensorEvent(&sensorValue))
  {
    retVal = calculateHeading(sensorValue.un.arvrStabilizedRV.i, sensorValue.un.arvrStabilizedRV.j, sensorValue.un.arvrStabilizedRV.k, sensorValue.un.arvrStabilizedRV.real);

    // Serial.println(retVal);

    retVal -= offsetForImu;
    if (retVal < 0)
    {
      retVal += 359.99;
    }
  }
  if (retVal == -1)
  {
    // ESP.restart();
    // Serial.println();
    // Serial.println("ret val is -1!");
    /*
    bno08x.hardwareReset();
    if (bno08x.wasReset())
    {
      Serial.println("Hardware reset successful");
    }
    Wire1.flush();
    Wire1.end();
    Serial.println();
    ESP.restart();
    */
  }
  return retVal;
}
// Helper function to get the current angle of the robot
float getCurrentAngle()
{
  float currentAngle = -1;
  while (currentAngle == -1)
  {

    currentAngle = getHeading();
  }
  return currentAngle;
}
// //  This function is used to turn the robot to a desired heading (car is not moving)
// void turnToHeading(float goal, uint8_t speed)
// {
//   float absVal;
//   float angleDiff;
//   uint8_t i = 0;

//   float currentAngle = getCurrentAngle();

//   // printToLcd("Current Angle: ", currentAngle);
//   angleDiff = goal - currentAngle;
//   absVal = abs(angleDiff);
//   if (absVal > 350)
//   {
//     absVal = 359.99 - absVal;
//     angleDiff = 359.99 - angleDiff;
//   }
//   // Serial.println(angleDiff);
//   // Serial.println(absVal);
//   // Serial.println();

//   while (absVal > 10)
//   {
//     // Serial.print("abs: ");
//     // Serial.print(absVal);
//     // Serial.print(" angle: ");
//     // Serial.print(currentAngle);

//     if ((angleDiff >= 0) && (absVal <= 180))
//     {
//       // Serial.print(" case 1: ");
//       turn(COUNTER_CLOCKWISE, speed);
//     }
//     else if ((angleDiff < 0) && (absVal <= 180))
//     {
//       // Serial.print(" case 2: ");
//       turn(CLOCKWISE, speed);
//     }
//     else if ((angleDiff >= 0) && (absVal >= 180))
//     {
//       // Serial.print(" case 3: ");
//       turn(CLOCKWISE, speed);
//     }
//     else
//     {
//       // Serial.print(" case 4: ");
//       turn(COUNTER_CLOCKWISE, speed);
//     }

//     currentAngle = getCurrentAngle();

//     // Serial.println(goal);
//     angleDiff = goal - currentAngle;
//     absVal = abs(angleDiff);
//     if (absVal > 350)
//     {
//       absVal = 359.99 - absVal;
//       angleDiff = 359.99 - angleDiff;
//     }
//     // Serial.println(angleDiff);
//     // Serial.println(absVal);
//     // Serial.println();
//   }
//   stop();
// }
//  TODO: CLEAN UP THIS FUNCTION
//  This function is supposed to run in while loop
//  where you can call the stop() function to stop the robot when it reaches
// //  the desired position or if there is an obstacle in the way.
// void driveToHeading(float goalHeading)
// {
//   float currentAngle = -1;
//   float absVal;
//   float angleDiff;
//   unsigned long currentMillis;
//   unsigned long previousMillis = 0;
//   uint32_t interval = 1000;
//   uint16_t i = 0;

//   bool goToHeading = true;
//   // while (goToHeading)
//   // {
//   currentAngle = getCurrentAngle();

//   i++;
//   if (i == 30)
//   {
//     // printToLcd("Current Angle: ", currentAngle);
//     i = 0;
//   }
//   angleDiff = goalHeading - currentAngle;
//   absVal = abs(angleDiff);
//   // Serial.println(absVal);
//   if (absVal > 345)
//   {
//     absVal = 359.99 - absVal;
//     angleDiff = 359.99 - angleDiff;
//   }
//   // Serial.println(angleDiff);
//   // Serial.println(absVal);
//   // Serial.println();

//   if (absVal > 15)
//   {
//     turnToHeading(goalHeading, 65);            // turn to the desired heading because angle is to great to correct while driving
//     previousMillis = currentMillis = millis(); // this timer was used to stop the robot after one seconds when starts driving at the correct angle
//                                                // but this is not used anymore
//   }
//   else if (absVal <= 15 && absVal >= 3)
//   {
//     if ((angleDiff >= 0) && (absVal <= 180))
//     {
//       // Serial.print(" case 1: ");
//       turn2(COUNTER_CLOCKWISE, 65, 30); // Turn2 takes in the direction, speed, and an offset that will increase the turning wheel by the given amount
//     }
//     else if ((angleDiff < 0) && (absVal <= 180))
//     {
//       // Serial.print(" case 2: ");
//       turn2(CLOCKWISE, 65, 30);
//     }
//     else if ((angleDiff >= 0) && (absVal >= 180))
//     {
//       // Serial.print(" case 3: ");
//       turn2(CLOCKWISE, 65, 30);
//     }
//     else
//     {
//       // Serial.print(" case 4: ");
//       turn2(COUNTER_CLOCKWISE, 65, 30);
//     }
//     previousMillis = currentMillis = millis();
//   }
//   else if (absVal < 5 && absVal >= 2) // if the angle is less than 5 degrees, drive the turning wheel slightly faster than the straight wheel
//   {
//     if ((angleDiff >= 0) && (absVal <= 180))
//     {
//       // Serial.print(" case 1: ");
//       turn2(COUNTER_CLOCKWISE, 65, 20);
//     }
//     else if ((angleDiff < 0) && (absVal <= 180))
//     {
//       // Serial.print(" case 2: ");
//       turn2(CLOCKWISE, 65, 20);
//     }
//     else if ((angleDiff >= 0) && (absVal >= 180))
//     {
//       // Serial.print(" case 3: ");
//       turn2(CLOCKWISE, 65, 20);
//     }
//     else
//     {
//       // Serial.print(" case 4: ");
//       turn2(COUNTER_CLOCKWISE, 65, 20);
//     }
//     previousMillis = currentMillis = millis();
//   }
//   else if (absVal < 2 && absVal >= 1) // if the angle is less than 2 degrees, drive the turning wheel barely faster than the straight wheel
//   {
//     if ((angleDiff >= 0) && (absVal <= 180))
//     {
//       // Serial.print(" case 1: ");
//       turn2(COUNTER_CLOCKWISE, 65, 10);
//     }
//     else if ((angleDiff < 0) && (absVal <= 180))
//     {
//       // Serial.print(" case 2: ");
//       turn2(CLOCKWISE, 65, 10);
//     }
//     else if ((angleDiff >= 0) && (absVal >= 180))
//     {
//       // Serial.print(" case 3: ");
//       turn2(CLOCKWISE, 65, 10);
//     }
//     else
//     {
//       // Serial.print(" case 4: ");
//       turn2(COUNTER_CLOCKWISE, 65, 10);
//     }
//     unsigned long currentMillis = millis();

//     // if (currentMillis - previousMillis >= interval)
//     // {
//     //   previousMillis = currentMillis;
//     //   if (previousMillis != 0)
//     //   {
//     //     goToHeading = false;
//     //     stop();
//     //   }
//     // }
//   }
//   else
//   {
//     move(FORWARD, 255); // if the angle is less than 1 degree, drive straight

//     // if (currentMillis - previousMillis >= interval)
//     // {
//     //   previousMillis = currentMillis;
//     //   if (previousMillis != 0)
//     //   {
//     //     goToHeading = false;
//     //     stop();
//     //   }
//     // }
//   }
//   //}
// }
