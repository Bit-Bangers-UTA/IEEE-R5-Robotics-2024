/*
  Bit Bangers test project for the VL53L4CX i2c library
  github.com/Bit-Bangers-UTA/Senior-Design

  Authors:
  Rolando Rosales

  ESP32 to Adafruit VL53L4CX pinout:
  SDA: 0
  SCL: 1
  XSHUT: 2
*/

#include <Arduino.h>
#include <Wire.h>
#include <VL53L4CX_i2c.h>

#define SDA 9
#define SCL 8
#define XSHUT 3

void setup() {
  Serial.begin(115200);
  initVL53L4CX(SDA, SCL, XSHUT);
}

void loop() {
  Wire.requestFrom(0x52, 5, 0);
  delay(500);
}
