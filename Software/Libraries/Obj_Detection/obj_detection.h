#include <Arduino.h>
#include "SparkFun_VL53L1X.h"
#include "OPT3101_whisker.h"
#include "VL53L1X.h"



double optBuffer(double val);
void fillFirstBuffer();


float getVL53L1XDistanceCm();

// void wallDetection();



void wallDetection(float nextX, float nextY);

