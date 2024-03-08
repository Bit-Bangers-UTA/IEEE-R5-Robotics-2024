/*
  Bit Bangers ESP-NOW "Kill Switch" Receiver Library
  github.com/munozr1/Senior-Design

  Original Code:
  
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

/*
 ----------------------------------- Libraries ---------------------------------
*/
#include <esp_now.h>
#include <WiFi.h>
#include "Kill_SW_RX.h"

/*
------------------------------------- Defines ----------------------------------
*/

/*
------------------------------------ Structures --------------------------------
*/

/*
--------------------------------  Global Variables -----------------------------
*/
bool toggleLED = false;
char msg[32];

/*
------------------------------------ Functions ---------------------------------
*/
// callback function that will be executed when data is received
void cb(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&msg, incomingData, sizeof(msg));

  if (!strcmp(msg, "kill")) {
    // This code simply turns on the builtin LED
    // Todo: Power off all peripherals and ESP32 here
    if (toggleLED) {
      digitalWrite(LED_BUILTIN, LOW);
      toggleLED = false;
    }
    else {
      digitalWrite(LED_BUILTIN, HIGH);
      toggleLED = true;

    }
  }
}
 
// initializes the kill switch receiver
void initKillRX() {
  // Disable Bluetooth
  btStop();
  
  // Disable Wi-Fi Access Point (AP) mode
  WiFi.mode(WIFI_STA);
  // WiFi.softAPdisconnect(true); // copilot suggest to lower power, test later

  // Init ESP-NOW
  esp_now_init();
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(cb);
}