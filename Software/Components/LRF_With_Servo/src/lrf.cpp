
#include "Arduino.h"
#include "Wire.h"
#include "lrf.h"

uint8_t trig[] = {0x5A, 0x04, 0x04, 0x62};
uint8_t FR_0[] = {0x5A, 0x06, 0x03, 0x00, 0x00, 0x5A + 0x06 + 0x03};  		// Sets frame rate to 0.
uint8_t save[] = {0x5A, 0x04, 0x11, 0x6F};									// Saves config
uint8_t req_data_cm[] = {0x5a, 0x05, 0x00, 0x01, 0x60};
uint8_t req_data_mm[] = {0x5a, 0x05, 0x00, 0x06, 0x65};
uint8_t system_reset[] = {0x5A, 0x04,0x02, 0x60};


int16_t getLrfDistance(uint8_t address)
{
  uint8_t data[9];
	lrf_packet * lrf_packet_ptr;
	lrf_packet_ptr = (lrf_packet*)data;

  uint8_t i = 0;
  Wire.beginTransmission(address);
  Wire.write(req_data_cm, 5);
  Wire.endTransmission();
  delay(10);
  Wire.requestFrom(address, 9);
  for (i = 0; i < 9; i++)
  {
    data[i] = Wire.read();
  }
  
  //printf("0x%02X: %d\n", address, lrf_packet_ptr->distance_cm);
  return lrf_packet_ptr->distance_cm;

}