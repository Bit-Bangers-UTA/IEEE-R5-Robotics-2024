/*
  Bit Bangers ESP-NOW "Kill Switch" Sender Code
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

/*
------------------------------------- Defines ----------------------------------
*/
#define PUSH_BUTTON 8

/*
------------------------------------ Structures --------------------------------
*/
typedef struct struct_message { // Structure example to send data
  char a[32];
} struct_message;

/*
--------------------------------  Global Variables -----------------------------
*/
uint8_t esp32MacAddr[] = {0x84, 0xfc, 0xe6, 0x01, 0x02, 0xb0}; // Receiver MAC Address
bool ledState = HIGH; // the current state of the output pin
bool buttonState; // the current reading from the input pin
bool lastButtonState = LOW; // the previous reading from the input pin
struct_message myData; // Struct that holds sender data
esp_now_peer_info_t peerInfo;

/*
------------------------------------ Functions ---------------------------------
*/
// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Setup function
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for
  // Send CB to get the status of Transmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, esp32MacAddr, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // Set GPIO 8 as input with pull-up resistor
  pinMode(PUSH_BUTTON, INPUT_PULLUP);

  // Set values to send
  strcpy(myData.a, "kill");
}
 
void loop() {
  if (digitalRead(PUSH_BUTTON) == LOW) {
    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(esp32MacAddr, (uint8_t *) &myData, sizeof(myData));
     
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }

    delay(200);
  }
}