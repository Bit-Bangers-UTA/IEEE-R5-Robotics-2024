/*
  Bit Bangers Bluetooth LE Xbox Core Controller Dummy Robot Example
  github.com/Bit-Bangers-UTA/Senior-Design

  This isn't a fully fleshed out library just yet
  All this does is report the necessary values from the controller for control
  I (Rolando) used a Xbox 1914 controller specifically to test it
  Import asukiaaa/XboxSeriesXControllerESP32_asukiaaa@^1.0.9 for other projects
*/

#include <Arduino.h>
#include <XboxSeriesXControllerESP32_asukiaaa.hpp>

XboxSeriesXControllerESP32_asukiaaa::Core xboxController;

void setup() {
  Serial.begin(115200);
  xboxController.begin();
}

void loop() {
  xboxController.onLoop();

  if (xboxController.isConnected()) {
    if (xboxController.isWaitingForFirstNotification()) {
      Serial.println("Waiting for first notification");
    }
    else {
      // uses right stick to steer
      Serial.print("Steering: ");
      float joyRHoriNormalized = (float)xboxController.xboxNotif.joyRHori / XboxControllerNotificationParser::maxJoy;
      float joyRHoriMapped = joyRHoriNormalized * 2 - 1;
      Serial.println(joyRHoriMapped, 2);

      // uses right trigger to accelerate
      Serial.print("Accelerating at: ");
      Serial.println((uint16_t)xboxController.xboxNotif.trigRT / 4);

      // uses left trigger to reverse
      Serial.print("Reversing at: ");
      Serial.println((uint16_t)xboxController.xboxNotif.trigLT / 4);

      Serial.println();
    }
  }
  else {
    Serial.println("Not connected");
    
    if (xboxController.getCountFailedConnection() > 2) {
      ESP.restart();
    }
  }
  delay(50);
}