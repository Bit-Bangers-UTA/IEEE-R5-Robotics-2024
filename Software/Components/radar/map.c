#include "map.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct ChargingStation H = {0, 0, 'H', 0, NULL};
struct ChargingStation G = {0, 0, 'G', 0, &H};
struct ChargingStation F = {0, 0, 'F', 0, &G};
struct ChargingStation E = {0, 0, 'E', 0, &F};
struct ChargingStation D = {0, 0, 'D', 0, &E};
struct ChargingStation C = {0, 0, 'C', 0, &D};
struct ChargingStation B = {0, 0, 'B', 0, &C};
struct ChargingStation A = {0, 0, 'A', 0, &B};
struct Coordinate current_location = {0, 0};


/*
* This function is used to convert the distance from the sensor to inches.
* --------------------------------------------------------------------------
* @param dobule mm: the distance from the sensor in millimeters
* @return: the distance from the sensor in inches
*/
double mm2inches(double mm) { return mm / 25.4; }

/*
* This function is used to convert angles from degrees to radians.
* --------------------------------------------------------------------------
* @param double deg: Angle in degrees
* @return: Angle in radians
*/
double deg2rad(double deg) { return deg * (PI / 180); }


/*
* This function is used to map a point to the map.
* -------------------------------------------------
* @param char *polarCoord: The polar coordinate string to be mapped
*/
void map_point(char *polarCoord) {
  double distance, angle;

  // parse the polar coordinate input string
  sscanf(polarCoord, "%lf@%lf", &angle, &distance);
  // if we read a distance bigger than we can accomidate just ignore it. This is more used for testing purposes. Its a good saftey though.
  if (distance > MAP_SIZE)
    return;

  angle = deg2rad(angle);
  int x = (int)(distance * cos(angle));
  int y = (int)(distance * sin(angle));

  int gridX = x + current_location.x; // Add the current location to the point to get the grid location of the point
  int gridY = y + current_location.y;

  if (gridX < 0 || gridX >= MAP_SIZE || gridY < 0 || gridY >= MAP_SIZE)
    return;

  if (gridX >= 0 && gridX < MAP_SIZE && gridY >= 0 && gridY < MAP_SIZE) {
    map_ring_buffer[buffer_head].x = gridX; // Add the point to the ring buffer
    map_ring_buffer[buffer_head].y = gridY;
    time(&map_ring_buffer[buffer_head].created); // Add a timestamp
    map[gridX][gridY] = 1;// Add the point to the map
  } else {
    printf("Point map[%d][%d] (%lf, %lf) is out of bounds\n", gridX, gridY,
           distance, angle);
  }
}

/*
* This function is used to print the map to the console.
* ------------------------------------------------------
*/
void print_map() {
    for (int i = 0; i < MAP_SIZE; i++) {
      for (int j = 0; j < MAP_SIZE; j++) {
        if (map[i][j] == 1) {
          printf("X");
        } else {
          printf(" ");
        }
      }
      printf("\n");
    }
  }
