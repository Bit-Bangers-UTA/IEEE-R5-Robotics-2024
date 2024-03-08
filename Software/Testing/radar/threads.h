#pragma once
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <pthread.h>

extern pthread_mutex_t map_mutex;
extern pthread_mutex_t daq_fd_mutex;
extern char daq_buffer[15];
extern int daq_fd;

void daq_thread_read();
void update_thread_buffer();
