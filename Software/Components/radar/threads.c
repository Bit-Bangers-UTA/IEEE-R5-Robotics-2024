#include "threads.h"
#include "map.h"
#include <pthread.h>

/*
* Function: daq_thread_read
* -----------------------------
* This function is intended to be run as a thread responsible for reading data
* from a data acquisition system (DAQ). It continuously reads bytes from a serial
* port into a global buffer, daq_buffer, and processes the data by calling map_point.
* It ensures that the buffer does not overflow by checking if there is space before
* attempting to read more data. If the buffer is full, it waits before trying again.
* After successfully reading data, it locks a mutex to safely update shared resources
* with the map_point function and advances the buffer_head pointer. The mutex is then
* unlocked to allow other threads to access the shared resources. If compiled with the
* DEBUG flag, it prints messages upon acquiring and releasing the mutex.
*/
void daq_thread_read() {
  // Read and print data
    if ((buffer_head + 1) % BUFFER_SIZE != buffer_tail){ // if buffer if full
      int bytes_read = read(daq_fd, daq_buffer, sizeof(daq_buffer));
      if (bytes_read < 0) {
        perror("daq_thread_read_com => Error reading from serial port");
      } else {
        daq_buffer[bytes_read] = '\0'; // Null terminate the string
        pthread_mutex_lock(&map_mutex);
        #ifdef DEBUG
        printf("daq_thread_read_com => map_mutex obtained\n");
        #endif
        map_point(daq_buffer);
        buffer_head = (buffer_head + 1) % BUFFER_SIZE; // wrap around the ring buffer
        pthread_mutex_unlock(&map_mutex);
        #ifdef DEBUG
        printf("daq_thread_read_com => map_mutex released\n");
        #endif
    }
}
}

/*
* Function: update_thread_buffer
* ---------------------------------------
* This function is designed to run as a separate thread that maintains the integrity
* of a ring buffer used for storing points on a map. It aims to remove points from
* the buffer that are older than a certain age, ensuring that the buffer does not
* hold outdated information. The function sleeps for 1 second upon starting to give
* the map_thread a head start.
*
* The function enters an infinite loop where it first checks if the buffer is empty
* by comparing the buffer_tail and buffer_head indices. If the buffer is empty, it
* continues to the next iteration of the loop without performing any actions.
*
* If there are elements in the buffer, it acquires a mutex lock to ensure that
* shared resources are accessed safely. This prevents concurrent modification issues
* that could arise from other threads, such as the map_thread or daq_thread,
* accessing the same resources. The DEBUG flag allows for printing messages upon
* acquiring and releasing the mutex for debugging purposes.
*
* With the mutex acquired, the function retrieves the current time and iterates over
* the ring buffer, starting from the buffer_tail. It checks each point's timestamp
* against the current time and if the difference exceeds a certain threshold (1
* second in this case), it removes the point from the map and advances buffer_tail.
*
* The process of checking timestamps and removing points continues until the buffer
* is either empty or all remaining points are within the acceptable age range. Once
* finished, the function releases the mutex lock to allow other threads to access the
* shared resources and then continues to the next iteration of its loop after a
* brief pause to prevent excessive CPU usage from tight looping.
*/
void update_thread_buffer() {
  
    if (buffer_tail != buffer_head){ // if buffer is empty
      pthread_mutex_lock(&map_mutex);
      #ifdef DEBUG
      printf("update_thread_buffer => map_mutex obtained\n");
      #endif
      time_t currentTime; // Get the current time
      time(&currentTime);

      // Iterate while the head points to a point that's too old
      while(buffer_tail != buffer_head && difftime(currentTime, map_ring_buffer[buffer_tail].created) > 0.5) {
        map[map_ring_buffer[buffer_tail].x][map_ring_buffer[buffer_tail].y] = 0; // Remove the point from the map
        buffer_tail = (buffer_tail + 1) % BUFFER_SIZE; // Move the tail to the next point
      }

      pthread_mutex_unlock(&map_mutex);
      #ifdef DEBUG
      printf("update_thread_buffer => map_mutex released\n");
      #endif
    }
}
