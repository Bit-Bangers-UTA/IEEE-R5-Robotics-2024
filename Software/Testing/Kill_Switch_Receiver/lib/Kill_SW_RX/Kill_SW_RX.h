/*
  Bit Bangers ESP-NOW "Kill Switch" Receiver Library
  github.com/munozr1/Senior-Design
*/

#ifndef KILL_SW_RX_h
#define KILL_SW_RX_h

#include <stdint.h>

void cb(const uint8_t * mac, const uint8_t *incomingData, int len);
void initKillRX();

#endif
