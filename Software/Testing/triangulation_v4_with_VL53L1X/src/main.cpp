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
#include "TCS_color_det.h"
#include "obj_detection.h"
// #define TEST_ALL_COMPONENTS
// #define TEST
#define ELIM
// #define TEST_ELIM
// #define MOTORS

// #define TEST_TURN_TO_HEADING

#define TURN_TO_ANGLE_DIFF 20
#define DRIVE_TO_ANGLE_DIFF 30

#define R_AMB 17
#define G_AMB 23
#define B_AMB 18
#define STATION_A 2, 1
#define STATION_B 6, 1
#define STATION_C 7, 2
#define STATION_D 7, 6
#define STATION_E 6, 7
#define STATION_F 2, 7
#define STATION_G 1, 6
#define STATION_H 1, 2

SFEVL53L1X lrf1_init;
SFEVL53L1X lrf2_init;

/*
 * README:
 * This code is used to find the position of the car based on the distance from the walls
 * The car has two LRFs that are used to find the distance from the walls
 * The car has a servo that is used to rotate the LRFs
 * The car also has an IMU that is used to find the heading of the car
 * You must position the car at 0 degrees (So that the IMU heading will be calibrated to the gamefield heading)
 */

/*
 * Global Variables for Motors
 */
const uint8_t MOTOR_A_IN_1 = 6;
const uint8_t MOTOR_A_IN_2 = 7;
int16_t A_LEFT_MOTOR_OFFSET = 0;

// RIGHT MOTOR
const uint8_t MOTOR_B_IN_3 = 5;
const uint8_t MOTOR_B_IN_4 = 4;
int16_t B_RIGHT_MOTOR_OFFSET = 2;

ESP32MotorControl motors;
bool direction = true;

/*
 * Global Variables for X and Y position

 */
float X_POS;
float Y_POS;

float OLD_X_POS = 60.96;
float OLD_Y_POS = 0;

float currentCardinalHeading;
/*
 * Global Variables for Servo
 */
Servo myservo = Servo();
uint8_t servoPin = 2;
uint16_t servoPosition = 90;
/*
 * Global Variables for IMU
 */
Adafruit_BNO08x bno08x;
sh2_SensorValue_t sensorValue;
float offsetForImu = 0;




void setup()
{
  Serial.begin(115200);
  Wire.begin(9, 8);
  Wire1.begin(20,21);
  initOPT3101();
  motors.attachMotors(MOTOR_B_IN_3, MOTOR_B_IN_4, MOTOR_A_IN_1, MOTOR_A_IN_2);
  // Wire1.begin(20, 21); //20 sda, 21 scl
  init_2_VL53L1X();
  // bno08x.hardwareReset();
  setupBNO085(&bno08x, 0x4A, &Wire1, 1); // Initialize the IMU
  initTCS(R_AMB, G_AMB, B_AMB, 0x29, &Wire1);
  
  Serial.println("*****TEST HEADING******\n\n");
  delay(3000);
  offsetForImu = getCurrentAngle(); // Get the offset of the IMU
  Serial.println("offset: ");
  Serial.println(offsetForImu);
  float currentAngle = getCurrentAngle();
  Serial.print("Current Angle: ");
  Serial.println(currentAngle);

  

  
  



}

void printCurrentAngle()
{
  float currentAngle = getCurrentAngle();
  Serial.print("CurrentAngle: ");
  Serial.println(currentAngle);
  // Serial.print("Cardinal heading: ");
  // Serial.println(findCardinalheading());
}

void printCurrentPos()
{
  // Serial.println("Position in CM:");
  // Serial.print("X: ");
  // Serial.print(X_POS);
  // Serial.print("\tY: ");
  // Serial.println(Y_POS);

  // Serial.println("Position in feet:");
  // Serial.print("X: ");
  // Serial.print(X_POS / 30.48); // Convert X_POS from cm to feet
  // Serial.print("\tY: ");
  // Serial.println(Y_POS / 30.48); // Convert Y_POS from cm to feet
}
// Function to find the cardinal heading based on the current angle
int16_t findCardinalheading()
{
  float currentAngle = getCurrentAngle();
  if (currentAngle >= 0 && currentAngle < 45 || currentAngle >= 315)
  {
    return 0; // North
  }
  else if (currentAngle >= 45 && currentAngle < 135)
  {
    return 90; // East
  }
  else if (currentAngle >= 135 && currentAngle < 225)
  {
    return 180; // South
  }
  else if (currentAngle >= 225 && currentAngle < 315)
  {
    return 270; // West
  }
  else
  {
    return -1; // Error
  }
}
void findPosition(uint16_t cardinalheading, float lrf1, float lrf2)
{
  switch (cardinalheading)
  {
  case 0:
    X_POS = 243 - lrf2; // Subtract the distance from the wall to the LRF
    Y_POS = 243 - lrf1; // Subtract the distance from the wall to the LRF
    break;
  case 90:
    X_POS = lrf1;
    Y_POS = 243 - lrf2;
    break;
  case 180:
    X_POS = lrf2;
    Y_POS = lrf1;
    break;
  case 270:
    X_POS = 243 - lrf1;
    Y_POS = lrf2;
    break;
  default:
    Serial.println("Error");
    break;
  }
  printCurrentPos();
}

void jiggle()
{
  float lrf1;
  float lrf2;
  float currentHeading = getCurrentAngle();
  uint16_t cardinalHeading = findCardinalheading();
  float servoPosition = cardinalHeading - currentHeading; // Calculate the difference between the cardinal heading and the current heading
  if (abs(servoPosition) > 90)
  {
    servoPosition = 359.99 - abs(servoPosition); // 359.99 is the max value for the servo
  }

  servoPosition += 90;                    // Add 90 because 90 degress is the middle position for the servo (therefore it is the current heading of the car)
  myservo.write(servoPin, servoPosition); // tell servo to go to position in variable 'pos'
  delay(100);                             // waits 15ms for the servo to reach the position
  lrf1 = getLrfDistanceCm(1);
  lrf2 = getLrfDistanceCm(2);
  // Serial.print("LRF1: ");
  // Serial.print(lrf1);
  // Serial.print("\tLRF2: ");
  // Serial.println(lrf2);
  findPosition(cardinalHeading, lrf1, lrf2); // Find the position of the car
}

float getNextAngle(float currentX, float currentY, float goalX, float goalY)
{
  // Serial.print("goal x ");
  // Serial.println(goalX);
  // Serial.print("gaol y ");
  // Serial.println(goalY);
  Serial.print("Current x ");
  Serial.print(currentX);
  Serial.print("\tCurrent Y ");
  Serial.println(currentY);
  float diff_X = goalX - currentX;
  float diff_Y = goalY - currentY;
  Serial.print("Diff x ");
  Serial.print(diff_X);
  Serial.print("\tDiff y ");
  Serial.println(diff_Y);

  float angle;
  if (diff_X == 0 && diff_Y > 0)
  {
    angle = 0;
  }
  else if (diff_X == 0 && diff_Y < 0)
  {
    angle = 180;
  }
  else if (diff_Y == 0 && diff_X > 0)
  {
    angle = 270;
  }
  else if (diff_Y == 0 && diff_X < 0)
  {
    angle = 90;
  }
  else
  {
    angle = atan(diff_Y / diff_X);
    // atan2()
    angle *= 180.0f / M_PI;
    // Serial.printf("Raw Angle degrees: %f\n", angle);

    if (diff_X > 0 && diff_Y > 0) // x+ y+
    {
      angle += 270;
    }
    else if (diff_X < 0 && diff_Y > 0) // x- y+
    {
      angle += 90;
    }
    else if (diff_X < 0 && diff_Y < 0) // x- y-
    {
      angle += 90;
    }
    else // x+ y-
    {
      angle += 270;
    }
  }
  // diff_X = -41.52;
  // diff_Y = 212.84;

  if (angle > 359.99)
    angle -= 359.99;
  else if (angle < 0)
    angle += 359.99;
  Serial.printf("next angle = %f\n\n", angle);

  Serial.printf("Degrees away from Current Angle = %f\n\n", angle - getCurrentAngle());

  return angle;
}

void turnToHeading(float goal, uint8_t speed)
{
  // stopMotors();
  //  speed = 60;
  printf("Goal: %f\n", goal);
  // delay(3000);
  float absVal;
  float angleDiff;
  uint8_t i = 0;

  float currentAngle = getCurrentAngle();

  // printToLcd("Current Angle: ", currentAngle);
  angleDiff = goal - currentAngle;
  absVal = abs(angleDiff);
  uint32_t turnDiff = TURN_TO_ANGLE_DIFF;
  // Serial.println(absVal);
  if (absVal > 360 - turnDiff)
  {
    absVal = 359.99 - absVal;
    angleDiff = 359.99 - angleDiff;
  }
  Serial.println(angleDiff);
  Serial.println(absVal);
  Serial.println();

  while (absVal > turnDiff)
  {
    // jiggle();

    // if (abs(X_POS - OLD_X_POS) > 30.48 * 3 || abs(Y_POS - OLD_Y_POS) > 30.48 * 3)
    // {

    //   // X_POS = OLD_X_POS;
    //   // Y_POS = OLD_Y_POS;
    //   stopMotors();
    //   uint8_t i = 0;
    //   for (i = 0; i < 5; i++)
    //   {
    //     jiggle();
    //   }
    // }
    //   Serial.print("abs: ");
    //   Serial.print(absVal);
    //   Serial.print(" angle: ");
    //   Serial.println(currentAngle);

    if ((angleDiff >= 0) && (absVal <= 180))
    {
      // Serial.print(" case 1: ");
      turn(COUNTER_CLOCKWISE, speed);
    }
    else if ((angleDiff < 0) && (absVal <= 180))
    {
      // Serial.print(" case 2: ");
      turn(CLOCKWISE, speed);
    }
    else if ((angleDiff >= 0) && (absVal >= 180))
    {
      // Serial.print(" case 3: ");
      turn(CLOCKWISE, speed);
    }
    else
    {
      // Serial.print(" case 4: ");
      turn(COUNTER_CLOCKWISE, speed);
    }

    currentAngle = getCurrentAngle();

    // Serial.println(goal);
    angleDiff = goal - currentAngle;
    absVal = abs(angleDiff);
    if (absVal > (360 - turnDiff)) // was (absVal > 360 - turnDiff)
    {
      absVal = 359.99 - absVal;
      angleDiff = 359.99 - angleDiff;
    }
    // Serial.println(angleDiff);
    // Serial.println(absVal);
    // Serial.println();
  }
  // stopMotors();
}

void driveToHeading(float goalHeading)
{
  float currentAngle = -1;
  float absVal;
  float angleDiff;
  unsigned long currentMillis;
  unsigned long previousMillis = 0;
  uint32_t interval = 1000;
  uint16_t i = 0;

  // bool goToHeading = true;
  // while (goToHeading)
  // {
  currentAngle = getCurrentAngle();

  angleDiff = goalHeading - currentAngle;
  absVal = abs(angleDiff);
  uint32_t turnDiff = DRIVE_TO_ANGLE_DIFF;
  // Serial.println(absVal);
  if (absVal > (360 - turnDiff))
  {
    absVal = 359.99 - absVal;
    angleDiff = 359.99 - angleDiff;
  }
  printf("Angle Diff: %f", angleDiff);
  printf("\tAbsVal: %f", absVal);

  // Serial.println(absVal);
  // Serial.println();

  if (absVal > turnDiff)
  {
    // stopMotors();
    // delay(20);
    // turnToHeading(goalHeading - 10, 60);
    turnToHeading(goalHeading, 60);
    printf("************************TURNING***********************\n");
  }
  else if (absVal >= 5)
  {
    printf("************************GREATTER THAN 5***********************\n");

    if ((angleDiff >= 0) && (absVal <= 180))
    {
      turn2(COUNTER_CLOCKWISE, 65, 30);
    }
    else if ((angleDiff < 0) && (absVal <= 180))
    {
      turn2(CLOCKWISE, 65, 30);
    }
    else if ((angleDiff >= 0) && (absVal >= 180))
    {
      turn2(CLOCKWISE, 65, 30);
    }
    else
    {
      turn2(COUNTER_CLOCKWISE, 65, 30);
    }
  }
  else if (absVal < 5 && absVal >= 2)
  {
    if ((angleDiff >= 0) && (absVal <= 180))
    {
      turn2(COUNTER_CLOCKWISE, 65, 10);
    }
    else if ((angleDiff < 0) && (absVal <= 180))
    {
      turn2(CLOCKWISE, 65, 10);
    }
    else if ((angleDiff >= 0) && (absVal >= 180))
    {
      turn2(CLOCKWISE, 65, 10);
    }
    else
    {
      turn2(COUNTER_CLOCKWISE, 65, 10);
    }
  }
  else
  {
    move(FORWARD, 65);
  }
  // }
}

/*
void wallDetection(bool pseudo)
{
  float optDistance;
  if(X_POS < 60.48 && Y_POS < 60.48)
  {
    optDistance = getWhiskerDistanceCm();
    if (optDistance < 15)
    {
      optDistance = getWhiskerDistanceCm()
      if(optDistance < 15)
      {
        while(optDistance < 15)
        {
          turn(CLOCKWISE, 65);
        }
      }
      stopMotors();
      move(FORWARD, 255);
      delay(500);
      stopMotors();
      goToCoordinates(COORDINATES);
    }
  }
  else
  {
    distanceSensor.stopRanging();
  }
}
*/

void goToCoordinates(float nextX, float nextY)
{
  nextX *= 30.48;
  nextY *= 30.48;
  float diffX = abs(nextX - X_POS);
  float diffY = abs(nextY - Y_POS);
  bool goToCoordinates = true;
  while (goToCoordinates)
  {

    wallDetection(nextX, nextY); // have do X_POS and Y_POS update in wallDetection automatically? Or must we return COORDINATE strcut and update manually?

    jiggle();
     
    

    if (abs(X_POS - OLD_X_POS) > 30.48 * 3 || abs(Y_POS - OLD_Y_POS) > 30.48 * 3)
    {

      // X_POS = OLD_X_POS;
      // Y_POS = OLD_Y_POS;
      stopMotors();
      uint8_t i = 0;
      for (i = 0; i < 5; i++)
      {
        jiggle();
      }
    }
    OLD_X_POS = X_POS;
    OLD_Y_POS = Y_POS;

    // get angle for next coordinates
    float nextAngle = getNextAngle(X_POS, Y_POS, nextX, nextY);
    driveToHeading(nextAngle);
    diffX = abs(nextX - X_POS);
    diffY = abs(nextY - Y_POS);
    // Serial.print("Diff X: ");
    // Serial.println(diffX);
    // Serial.print("Diff Y: ");
    // Serial.println(diffY);
    // Serial.print("Current X: ");
    // Serial.println(X_POS);
    // Serial.print("Current Y: ");
    // Serial.println(Y_POS);

    if (diffX <= 10 && diffY <= 10)
    {
      stopMotors();
      goToCoordinates = false;
    }
    // if (X_POS > 7 * 30.48 || Y_POS > 7 * 30.48 || X_POS < 1 || Y_POS < 1)
    // {
    //   stopMotors();
    //   delay(2000);
    //   // move(BACKWARD,60);
    //   // delay(1000);
    //   // stopMotors();
    // }
  }
}

bool firstRun = true;

void loop()
{
#ifdef ELIM

  if (firstRun)
  {
    uint16_t i = 0;
    for (i = 0; i < 30; i++)
      jiggle();
    firstRun = false;
  }
  // set current coordinates
  goToCoordinates(STATION_D);
  delay(1500);
  goToCoordinates(STATION_H);
  delay(1500);
  goToCoordinates(STATION_F);
  delay(1500);
  goToCoordinates(STATION_B);
  delay(1500);
  goToCoordinates(STATION_G);
  delay(1500);
  goToCoordinates(STATION_E);
  delay(1500);
  goToCoordinates(STATION_C);
  delay(1500);
  goToCoordinates(STATION_A);
  delay(1500);
  // goToCoordinates(4, 4);
  while (1)
    ;

#endif

#ifdef TEST_TURN_TO_HEADING
  driveToHeading(90);
  delay(5000);
  driveToHeading(180);
  delay(5000);
  driveToHeading(270);
  delay(5000);
  driveToHeading(45);
  while (1)
    ;
#endif
#ifdef TEST_ELIM
  testHeading(6, 2);
  stop();
  delay(1000);

  while (1)
    ;
#endif

/*
  Test Motors
*/
#ifdef MOTORS
  turn(CLOCKWISE, 80);
  delay(1000);
  turn(COUNTER_CLOCKWISE, 80);
  delay(1000);
#endif

  /*
   * Test all components at the same time.
   */
#ifdef TEST_ALL_COMPONENTS
  turn(direction, 80);
  direction ^= 1;
  for (int pos = 0; pos <= 180; pos++)
  {                               // go from 0-180 degrees
    myservo.write(servoPin, pos); // set the servo position (degrees)
  }
  for (int pos = 180; pos >= 0; pos--)
  {                               // go from 180-0 degrees
    myservo.write(servoPin, pos); // set the servo position (degrees)
    delay(15);
  }
  Serial.println("Peri check: ");
  Serial.println(getLrfDistanceCm(1));
  Serial.println(getLrfDistanceCm(2));

  Serial.print("IMU check: ");
  Serial.println(getCurrentAngle());

  Serial.print("Whisker check: ");
  Serial.println(getWhiskerDistanceCm());
  delay(1000);
#endif

  /*
   * Test the position of the car based on the distance from the walls
   */
#ifdef TEST
  static uint16_t i = 0;
  i++;
  jiggle();

  if (i >= 15)
  {
    printCurrentAngle();
    printCurrentPos();
    i = 0;
  }
  delay(10);

  void testHeading(float x, float y)
  {
    uint16_t i = 0;

    float currentAngle = 0.0;
    float nextAngle = 0.0;
    for (i = 0; i < 30; i++)
      jiggle();

    printf("raw 1: %f\n", getLrfDistanceCm(1) / 30.48);
    printf("raw 2: %f\n", getLrfDistanceCm(2) / 30.48);
    jiggle();
    printCurrentPos();
    currentAngle = getCurrentAngle();
    Serial.println("Current Angle: ");
    Serial.println(currentAngle);
    // x = 121.92 y = 91.44

    Serial.println("**************");
    Serial.println("Next Angle: ");
    nextAngle = getNextAngle(X_POS, Y_POS, x * 30.48, y * 30.48);
    Serial.println(nextAngle);
    Serial.println(getHeading());
    //   delay(25);
    // turnToHeading(nextAngle, 50);
    // driveToHeading(nextAngle, x*30.48, y*30.48);
    Serial.print("Angle reached: ");
    Serial.println(getCurrentAngle());
  }

#endif
}
