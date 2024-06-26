/*
  Reading distance from the laser based VL53L1X
  By: Nathan Seidle
  SparkFun Electronics
  Date: April 4th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  SparkFun labored with love to create this code. Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/14667

  This example prints the distance to an object.

  Are you getting weird readings? Be sure the vacuum tape has been removed from the sensor.
*/

#include <Arduino.h>
#include <Wire.h>
#include "SparkFun_VL53L1X.h" //Click here to get the library: http://librarymanager/All#SparkFun_VL53L1X
#include "VL53L1X.h"

// Optional interrupt and shutdown pins.
#define LRF1_SHUTDOWN_PIN 13
#define LRF2_SHUTDOWN_PIN 14

// Uncomment the following line to use the optional shutdown and interrupt pins.

SFEVL53L1X lrf1;
SFEVL53L1X lrf2;

void setup(void)
{
  Wire.begin(9, 8);
  Serial.begin(115200);
  Serial.println("VL53L1X Qwiic Test");

  pinMode(LRF1_SHUTDOWN_PIN, OUTPUT);
  digitalWrite(LRF1_SHUTDOWN_PIN, LOW);

  pinMode(LRF2_SHUTDOWN_PIN, OUTPUT);
  digitalWrite(LRF2_SHUTDOWN_PIN, LOW);

  pinMode(LRF2_SHUTDOWN_PIN, INPUT);
  delay(10);
  if (lrf2.begin() != 0) // Begin returns 0 on a good init
  {
    Serial.println("lrf2 failed to begin. Please check wiring. Freezing...");
    while (1)
      ;
  }
  Serial.println("Sensor online!");

  lrf2.setI2CAddress(0x40); // Change address of sensor 2 to 0x40
  if (lrf2.begin() != 0)    // Begin returns 0 on a good init
  {
    Serial.println("lrf2 failed to begin. Please check wiring. Freezing...");
    while (1)
      ;
  }

  lrf2.setIntermeasurementPeriod(50);
  Serial.println(lrf2.getIntermeasurementPeriod());

  pinMode(LRF1_SHUTDOWN_PIN, INPUT);
  delay(10);
  if (lrf1.begin() != 0) // Begin returns 0 on a good init
  {
    Serial.println("lrf1 failed to begin. Please check wiring. Freezing...");
    while (1)
      ;
  }

  lrf1.setIntermeasurementPeriod(50);
  Serial.println(lrf1.getIntermeasurementPeriod());

  lrf1.startRanging(); // Start only once (and do never call stop)
  lrf2.startRanging(); // Start only once (and do never call stop)
}

float getLrfDistance(uint8_t lrfNum)
{
  if (lrfNum == 1)
  {
    while (!lrf1.checkForDataReady())
    {
      delay(1);
    }
    lrf1.clearInterrupt();

    return lrf1.getDistance() / 10.0; // Get the result of the measurement from the sensor
  }
  else if (lrfNum == 2)
  {
    while (!lrf2.checkForDataReady())
    {
      delay(1);
    }
    lrf2.clearInterrupt();

    return lrf2.getDistance() / 10.0; // Get the result of the measurement from the sensor
  }
}

void loop(void)
{
  // lrf1.startRanging();                      // Write configuration bytes to initiate measurement
  while (!lrf1.checkForDataReady())
  {
    delay(1);
  }
  float lrf1Cm = lrf1.getDistance() / 10.0; // Get the result of the measurement from the sensor
  lrf1.clearInterrupt();
  // lrf1.stopRanging();

  Serial.print("LRF1 (ft): ");
  Serial.print(lrf1Cm / 30.48, 2);

  // lrf2.startRanging();                      // Write configuration bytes to initiate measurement
  while (!lrf2.checkForDataReady())
  {
    delay(1);
  }
  float lrf2Cm = lrf2.getDistance() / 10.0; // Get the result of the measurement from the sensor
  lrf2.clearInterrupt();
  // lrf2.stopRanging();

  Serial.print("\tlrf2 (ft): ");
  Serial.print(lrf2Cm / 30.48, 2);

  Serial.println();
}