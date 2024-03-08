/*
  Bit Bangers I2C Library for the VL53L4CX sensor
  github.com/Bit-Bangers-UTA/Senior-Design

  Authors:
  Rolando Rosales
*/

#include <Arduino.h>
#include <Wire.h>
#include "VL53L4CX_i2c.h"

const uint8_t w_addr = 0x52;
const uint8_t r_addr = 0x53;

void initVL53L4CX(uint8_t SDA, uint8_t SCL, uint8_t XSHUT) {
    // Take sensor out of "shutdown" mode (active low)
    pinMode(XSHUT, OUTPUT);
    digitalWrite(XSHUT, HIGH);

    // Sets up the I2C communication
    Wire.begin(SDA, SCL);
}

uint8_t getVL53L4CXData()
{
  
}