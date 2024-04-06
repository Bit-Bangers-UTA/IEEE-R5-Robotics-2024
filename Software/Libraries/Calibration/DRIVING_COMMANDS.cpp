#include <Arduino.h>
#include <stdbool.h>
#include <Adafruit_BNO08x.h>
#include <Wire.h>
#include <math.h>
#include <ESP32MotorControl.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>
#include "DRIVING_COMMANDS.h"

#include "BNO085_heading_acceleration.h"
#include "motor_control_v2.h"
#include "printToLcd.h"

// #define CALIBRATE_STRAIGHT_LINE
//  #define TEST_TURNS

// After starting/resetting the robot you have 5 seconds to align it with 0 degress
// We need to implement a blovking function that waits unitl button pressed before locking in 0
// Also we need to implement a lcd screen to show us our angle without uart

/*
 * Global Variables for LCD Display
 */
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
char lcdStr[16];
/*
 *Global Variable for the Motors
 */
ESP32MotorControl motors;
// LEFT MOTOR
const uint8_t MOTOR_A_IN_1 = 41;
const uint8_t MOTOR_A_IN_2 = 40;
int16_t A_LEFT_MOTOR_OFFSET = 0;

// RIGHT MOTOR
const uint8_t MOTOR_B_IN_3 = 39;
const uint8_t MOTOR_B_IN_4 = 38;
int16_t B_RIGHT_MOTOR_OFFSET = 2;

/*
 * Global Variable for IMU
 */
Adafruit_BNO08x bno08x;
sh2_SensorValue_t imuValue;

float offset = 0;

// float getHeading(sh2_SensorValue_t sensorValue)
// {

//   float retVal = -1;

//   if (bno08x.getSensorEvent(&sensorValue))
//   {
//     retVal = calculateHeading(sensorValue.un.arvrStabilizedRV.i, sensorValue.un.arvrStabilizedRV.j, sensorValue.un.arvrStabilizedRV.k, sensorValue.un.arvrStabilizedRV.real);

//     retVal -= offset;
//     if (retVal < 0)
//     {
//       retVal += 359.99;
//     }
//   }

//   return retVal;
// }
// float getCurrentAngle(sh2_SensorValue_t sensorValue)
// {
//   float currentAngle = -1;
//   while (currentAngle == -1)
//   {
//     currentAngle = getHeading(sensorValue);
//   }
//   return currentAngle;
// }
void printCurrentAngle(sh2_SensorValue_t sensorValue)
// {
//   float currentAngle = getCurrentAngle(sensorValue);
//   printToLcd("Current Angle: ", currentAngle);
// }
// void turnToHeading(sh2_SensorValue_t sensorValue, float goal, uint8_t speed)
// {
//   float absVal;
//   float angleDiff;
//   uint8_t i = 0;

//   float currentAngle = getCurrentAngle(sensorValue);

//   printToLcd("Current Angle: ", currentAngle);
//   angleDiff = goal - currentAngle;
//   absVal = abs(angleDiff);
//   if (absVal > 350)
//   {
//     absVal = 359.99 - absVal;
//     angleDiff = 359.99 - angleDiff;
//   }
//   Serial.println(angleDiff);
//   Serial.println(absVal);
//   Serial.println();

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

//     currentAngle = getCurrentAngle(sensorValue);
//     i++;
//     if (i >= 30)
//     {
      printCurrentAngle(sensorValue);
//       i = 0;
//     }

//     // Serial.println(goal);
//     angleDiff = goal - currentAngle;
//     absVal = abs(angleDiff);
//     if (absVal > 350)
//     {
//       absVal = 359.99 - absVal;
//       angleDiff = 359.99 - angleDiff;
//     }
//     Serial.println(angleDiff);
//     Serial.println(absVal);
//     Serial.println();
//   }
//   stop();
// }

void testTurns(sh2_SensorValue_t sensorValue)
{
  turnToHeading(sensorValue, 90, 85);
  // printCurrentAngle(sensorValue);
  delay(1000);

  turnToHeading(sensorValue, 0, 85);
  // printCurrentAngle(sensorValue);
  delay(1000);

  turnToHeading(sensorValue, 180, 85);
  // printCurrentAngle(sensorValue);
  delay(1000);

  turnToHeading(sensorValue, 0, 85);
  // printCurrentAngle(sensorValue);
  delay(1000);

  turnToHeading(sensorValue, 270, 85);
  // printCurrentAngle(sensorValue);
  delay(1000);

  turnToHeading(sensorValue, 45, 85);
  // printCurrentAngle(sensorValue);
  delay(1000);

  turnToHeading(sensorValue, 225, 85);
  // printCurrentAngle(sensorValue);
  delay(1000);

  turnToHeading(sensorValue, 135, 85);
  // printCurrentAngle(sensorValue);
  delay(1000);

  turnToHeading(sensorValue, 315, 85);
  // printCurrentAngle(sensorValue);
  delay(1000);
}

void setup()
{
  Serial.begin(115200);
  Wire.begin(9, 8);

  lcd.init(); // initialize the lcd
  lcd.backlight();

  setupBNO085(&bno08x);
  motors.attachMotors(MOTOR_A_IN_1, MOTOR_A_IN_2, MOTOR_B_IN_3, MOTOR_B_IN_4);
  float currentAngle;
  uint8_t i = 0;
  Serial.println("*****TEST HEADING******\n\n");

  printToLcd("HEADING", "TEST");
  delay(1000);
  printToLcd("Position to 0", "degrees");

  for (i = 3; i > 0; i--)
  {
    printToLcd("Time left: ", i);
    delay(1000);
  }

  offset = getCurrentAngle(imuValue);

  Serial.println("offset: ");
  Serial.println(offset);

  currentAngle = getCurrentAngle(imuValue);

  Serial.print("Current Angle: ");
  Serial.println(currentAngle);

  printToLcd("OFFSET:", offset);
  delay(1000);
  // printCurrentAngle(imuValue);
  delay(1000);
}

// void driveToHeading(sh2_SensorValue_t sensorValue, float goalHeading)
// {
//   float currentAngle = -1;
//   float absVal;
//   float angleDiff;
//   unsigned long currentMillis;
//   unsigned long previousMillis = 0;
//   uint32_t interval = 1000;
//   uint16_t i = 0;

//   bool goToHeading = true;
//   while (goToHeading)
//   {
//     currentAngle = getCurrentAngle(sensorValue);

//     i++;
//     if (i == 30)
//     {
//       printToLcd("Current Angle: ", currentAngle);
//       i = 0;
//     }
//     angleDiff = goalHeading - currentAngle;
//     absVal = abs(angleDiff);
//     // Serial.println(absVal);
//     if (absVal > 345)
//     {
//       absVal = 359.99 - absVal;
//       angleDiff = 359.99 - angleDiff;
//     }
//     Serial.println(angleDiff);
//     Serial.println(absVal);
//     Serial.println();

//     if (absVal > 15)
//     {
//       turnToHeading(sensorValue, goalHeading, 65);
//       previousMillis = currentMillis = millis();
//     }
//     else if (absVal <= 15 && absVal >= 3)
//     {
//       if ((angleDiff >= 0) && (absVal <= 180))
//       {
//         // Serial.print(" case 1: ");
//         turn2(COUNTER_CLOCKWISE, 65, 30);
//       }
//       else if ((angleDiff < 0) && (absVal <= 180))
//       {
//         // Serial.print(" case 2: ");
//         turn2(CLOCKWISE, 65, 30);
//       }
//       else if ((angleDiff >= 0) && (absVal >= 180))
//       {
//         // Serial.print(" case 3: ");
//         turn2(CLOCKWISE, 65, 30);
//       }
//       else
//       {
//         // Serial.print(" case 4: ");
//         turn2(COUNTER_CLOCKWISE, 65, 30);
//       }
//       previousMillis = currentMillis = millis();
//     }
//     else if (absVal < 5 && absVal >= 2)
//     {
//       if ((angleDiff >= 0) && (absVal <= 180))
//       {
//         // Serial.print(" case 1: ");
//         turn2(COUNTER_CLOCKWISE, 65, 10);
//       }
//       else if ((angleDiff < 0) && (absVal <= 180))
//       {
//         // Serial.print(" case 2: ");
//         turn2(CLOCKWISE, 65, 10);
//       }
//       else if ((angleDiff >= 0) && (absVal >= 180))
//       {
//         // Serial.print(" case 3: ");
//         turn2(CLOCKWISE, 65, 10);
//       }
//       else
//       {
//         // Serial.print(" case 4: ");
//         turn2(COUNTER_CLOCKWISE, 65, 10);
//       }
//       unsigned long currentMillis = millis();

//       if (currentMillis - previousMillis >= interval)
//       {
//         previousMillis = currentMillis;
//         if (previousMillis != 0)
//         {
//           goToHeading = false;
//           stop();
//         }
//       }
//     }
//     else
//     {
//       move(FORWARD, 65);

//       if (currentMillis - previousMillis >= interval)
//       {
//         previousMillis = currentMillis;
//         if (previousMillis != 0)
//         {
//           goToHeading = false;
//           stop();
//         }
//       }
//     }
//   }
// }

// void calibrateStraightLine()
// {
//   move(FORWARD, 75);
//   delay(4000);
//   stop();
//   move(BACKWARD, 75);
//   delay(4000);
//   stop();
// }

void loop()
{
#ifdef CALIBRATE_STRAIGHT_LINE
  calibrateStraightLine(); // change, left and right motor offset untill car goes straight for 4 seconds
  while (1)
    ;
#endif
#ifdef TEST_TURNS
  turn(COUNTER_CLOCKWISE);
  delay(2000);
  turn(CLOCKWISE);
  delay(2000);
  stop();
  delay(2000);
  testTurns(); // fix turning speed until angle is within desired range of goal heading
#endif

  driveToHeading(imuValue, 270);
  delay(1000);
  driveToHeading(imuValue, 0);
  delay(1000);
  driveToHeading(imuValue, 90);
  delay(1000);
  driveToHeading(imuValue, 180);
  delay(1000);

  while (1)
  {
    static uint16_t i = 0;
    i++;
    if (i >= 15)
    {
      // printCurrentAngle(imuValue);
      i = 0;
    }
    delay(10);
  }
}
