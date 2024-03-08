/*
  Bit Bangers Dummy Robot Project
  github.com/Bit-Bangers-UTA/Senior-Design

  ** This code currently does not work **

  ESP32-C3 to SmartCar Pinout:
  STBY: 0 -> 3
  PWMA: 1 -> 5
  PWMB: 2 -> 6
  AINA: 3 -> 7
  BIN1: 10 -> 8
  3V3 -> 5V0
  GND -> GND
*/

#include <Arduino.h>
#include <XboxSeriesXControllerESP32_asukiaaa.hpp>
#include <SparkFun_TB6612.h>

#define AIN1 3
#define BIN1 10
#define PWMA 1
#define PWMB 2
#define STBY 0

XboxSeriesXControllerESP32_asukiaaa::Core xboxController;

const int offsetA = 1;
const int offsetB = 1;

Motor motor1 = Motor(AIN1, 0, PWMA, offsetA, STBY);
Motor motor2 = Motor(BIN1, 0, PWMB, offsetB, STBY);

void setup() {
  Serial.begin(115200);
  xboxController.begin();
}

void loop() {
  xboxController.onLoop();

  if (xboxController.isConnected()) {
    // at the moment, only goes forward and backward
    /*
    // uses right stick to steer
    Serial.print("Steering: ");
    float joyRHoriNormalized = (float)xboxController.xboxNotif.joyRHori / XboxControllerNotificationParser::maxJoy;
    float joyRHoriMapped = joyRHoriNormalized * 2 - 1;
    Serial.println(joyRHoriMapped, 2);
    */

    // uses right trigger to accelerate
    Serial.print("Accelerating at: ");
    Serial.println((uint16_t)xboxController.xboxNotif.trigRT / 4);

    // uses left trigger to reverse
    Serial.print("Reversing at: ");
    Serial.println((uint16_t)xboxController.xboxNotif.trigLT / 4);

    Serial.println();
  }
  else {
    Serial.println("Not connected");
    
    if (xboxController.getCountFailedConnection() > 2) {
      ESP.restart();
    }
  }

  uint16_t rev = (uint16_t)xboxController.xboxNotif.trigLT / 4;
  uint16_t fwd = (uint16_t)xboxController.xboxNotif.trigRT / 4;

  int speeeeed = fwd - rev;

  Serial.println(speeeeed);

  motor1.drive(speeeeed);
  motor2.drive(speeeeed);

  delay(10);
}
