#ifndef VL53L4CX_I2C_H_
#define VL53L4CX_I2C_H_

#include <Arduino.h>

void initVL53L4CX(uint8_t SDA, uint8_t SCL, uint8_t XSHUT);
uint8_t getVL53L4CXData();

#endif