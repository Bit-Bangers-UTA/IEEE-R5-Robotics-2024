/*
 * lrf.h
 *
 *  Created on: Mar 3, 2024
 *      Author: Rodrigo Munoz
 */
#pragma once
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

//Circular buffer size is 180 because the LiDAR sensor has a 180 degree field of view
#define BUFFER_SIZE 180
#define MAP_SIZE 150
#define PI 3.14159265358979323846



struct ChargingStation {
  char name;
  int x;
  int y;
  int occupied;
  struct ChargingStation *next;
};
struct Coordinate {
  int x;
  int y;
  double h;
  double angle;
  time_t created;
};


double deg2rad(double deg);
double mm2in(double mm);
double calc_slope(struct Coordinate *curr_reading);
void map_point(int distance, int angle);
void clear_map();
void print_map();
int find_local_min();
