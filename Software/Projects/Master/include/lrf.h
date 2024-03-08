/*
 * lrf.h
 *
 *  Created on: Mar 3, 2023
 *      Author: Velu Manohar
 */

#ifndef LRF_H_
#define LRF_H_

#include "Arduino.h"

#define packet_size 9



typedef struct _lrf_packet
{
    uint16_t frame_header;
    int16_t distance_cm;
    uint16_t strength;
    uint16_t temp;
    uint8_t checksum;
}lrf_packet;

int16_t getLrfDistance(uint8_t address);
void switch_tfmini_to_i2c();
#endif /* LRF_H_ */

