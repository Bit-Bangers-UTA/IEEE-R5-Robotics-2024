#include "map.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int local_max_index;
extern struct Coordinate *local_maxes[4];
/*
* This function is used to calculate the slope of a line.
* -------------------------------------------------------
* @param struct Coordinate *curr_reading: The current coordinate reading
* @return double: The slope of the line from the previous coordinate to the current coordinate
*/
// double calc_slope(struct Coordinate *curr_reading){
//     double y = curr_reading->y - prev_coordinate.y;
//     double x = curr_reading->x - prev_coordinate.x;

//     return y/x;
// }

/*
* Convert the distance from the sensor to inches.
* --------------------------------------------------------------------------
* @param double mm: the distance from the sensor in millimeters
* @return: the distance from the sensor in inches
*/
double mm2inches(double mm) { return mm / 25.4; }

/*
* Convert the distance from centimeters to inches.
* --------------------------------------------------------------------------
* @param double cm: the distance in centimeters
* @return: the distance in inches
*/
double cm2inches(double cm) {
    return cm / 2.54;
}

/*
* Convert angles from degrees to radians.
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
void map_point(int distance, int angle) {
  if (distance < 0) return;
  double rad = deg2rad(angle);
  int gridX = (int)(distance * cos(rad));
  int gridY = (int)(distance * sin(rad));

  printf("%d @ %d =  X: %d, Y: %d\n", distance, angle ,gridX, gridY);

  struct Coordinate *curr = (struct Coordinate *)malloc(sizeof(struct Coordinate));
  curr->x = gridX;
  curr->y = gridY;
  curr->h = distance;
  curr->angle = angle;
  time(&curr->created); // Add a timestamp


  if(local_max_index < 4){
    local_maxes[local_max_index] = curr;
    local_max_index++;
  } else {
    int min = find_local_min();
    if(local_maxes[min]->h < curr->h){
        local_maxes[min] = curr;
    }
  }
  //TODO: if curr is not a max, free it
}

/*
* This function is used to find the smallest local max in the local_maxes array
* ----------------------------------------------------------------------------
* @return int: The index of the smallest local max
*/
int find_local_min(){
  //find the smallest in local_maxes and return its index
  struct Coordinate *min = local_maxes[0];
  int min_index = 0;
  for(int i = 1; i < 4; i++){
    if(local_maxes[i]->h < min->h){
      min = local_maxes[i];
      min_index = i;
    }
  }
  return min_index;
  
}
