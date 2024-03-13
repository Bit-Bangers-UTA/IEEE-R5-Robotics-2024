/*
 * Bit Bangers Dummy Robot (v2) Project
 * github.com/Bit-Bangers-UTA/Senior-Design
 *
 * Authors:
 * Rolando Rosales
 *
 * Hardware setup:
 * ESP32 GPIO 7 -> Motor Controller IN4
 * ESP32 GPIO 6 -> Motor Controller IN3
 * ESP32 GPIO 5 -> Motor Controller IN2
 * ESP32 GPIO 4 -> Motor Controller IN1
 * MOTORA both right wheels
 * MOTORB both left wheels
 * 
 * Xbox Controller required for remote operation
 * 
 * asukiaaa/XboxSeriesXControllerESP32_asukiaaa@^1.0.9 required
*/

// Libraries ------------------------------------------------------------------

#include <Arduino.h>
#include <XboxSeriesXControllerESP32_asukiaaa.hpp>

// Defines --------------------------------------------------------------------

#define A_IN_4 7
#define A_IN_3 6
#define A_IN_2 5
#define A_IN_1 4

// Variables & Constants ------------------------------------------------------

const uint8_t deadzone = 100;

// Structures & Classes -------------------------------------------------------

XboxSeriesXControllerESP32_asukiaaa::Core xboxController;

// Functions ------------------------------------------------------------------

void forward()
{
  digitalWrite(A_IN_1, 1);
  digitalWrite(A_IN_2, 0);
  digitalWrite(A_IN_3, 1);
  digitalWrite(A_IN_4, 0);
}

void reverse()
{
  digitalWrite(A_IN_1, 0);
  digitalWrite(A_IN_2, 1);
  digitalWrite(A_IN_3, 0);
  digitalWrite(A_IN_4, 1);
}

void stop()
{
  digitalWrite(A_IN_1, 0);
  digitalWrite(A_IN_2, 0);
  digitalWrite(A_IN_3, 0);
  digitalWrite(A_IN_4, 0);
}

void left()
{
  digitalWrite(A_IN_1, 1);
  digitalWrite(A_IN_2, 1);
  digitalWrite(A_IN_3, 0);
  digitalWrite(A_IN_4, 0);
}

void right()
{
  digitalWrite(A_IN_1, 0);
  digitalWrite(A_IN_2, 0);
  digitalWrite(A_IN_3, 1);
  digitalWrite(A_IN_4, 1);
}

void stall()
{
  digitalWrite(A_IN_1, 1);
  digitalWrite(A_IN_2, 1);
  digitalWrite(A_IN_3, 1);
  digitalWrite(A_IN_4, 1);
}

void setup() {
  pinMode(A_IN_1, OUTPUT);
  pinMode(A_IN_2, OUTPUT);
  pinMode(A_IN_3, OUTPUT);
  pinMode(A_IN_4, OUTPUT);

  Serial.begin(115200);

  xboxController.begin();
}

void loop() {
  xboxController.onLoop();

  if (xboxController.isConnected()) {
    Serial.print("D-pad Left: ");
    Serial.println(xboxController.xboxNotif.btnDirLeft);

    Serial.print("D-pad Right: ");
    Serial.println(xboxController.xboxNotif.btnDirRight);

    Serial.print("Right Trigger: ");
    Serial.println(xboxController.xboxNotif.trigRT);

    Serial.print("Left Trigger: ");
    Serial.println(xboxController.xboxNotif.trigLT);

    Serial.println();
  }
  else {
    Serial.println("Not connected");
    
    if (xboxController.getCountFailedConnection() > 2) {
      delay(1000);
      ESP.restart();
    }
  }

  if (xboxController.xboxNotif.btnDirLeft) {
    left();
  }
  else if (xboxController.xboxNotif.btnDirRight) {
    right();
  }
  else if ((xboxController.xboxNotif.trigRT > deadzone) && (xboxController.xboxNotif.trigLT > deadzone)) {
    stall();
  }
  else if (xboxController.xboxNotif.trigRT > deadzone) {
    forward();
  }
  else if (xboxController.xboxNotif.trigLT > deadzone) {
    reverse();
  }
  else {
    stop();
  }
}
