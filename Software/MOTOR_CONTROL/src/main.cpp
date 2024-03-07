#include <Arduino.h>

#define LIGHT 3
#define MOTORA_IN_1 3
#define MOTORA_IN_2 2
#define MOTORB_IN_3 6
#define MOTORB_IN_4 7

#define TIMER_INTERVAL_MS 5000
#define FORWARD 1
#define BACKWARD 0
#define STOP 0

bool direction = true;

void move(bool direction)
{
  if(direction == FORWARD)
  {
    digitalWrite(MOTORA_IN_1, HIGH);
    digitalWrite(MOTORA_IN_2, LOW);
    digitalWrite(MOTORB_IN_3, HIGH);
    digitalWrite(MOTORB_IN_4, LOW);

  }
  else
  {
    digitalWrite(MOTORA_IN_1, LOW);
    digitalWrite(MOTORA_IN_2, HIGH);
    digitalWrite(MOTORB_IN_3, LOW);
    digitalWrite(MOTORB_IN_4, HIGH);
  }
}

void move(bool direction, uint16_t speed)
{
  if(direction == FORWARD)
  {
    analogWrite(MOTORA_IN_1, speed);
    analogWrite(MOTORA_IN_2, LOW);
    analogWrite(MOTORB_IN_3, speed);
    analogWrite(MOTORB_IN_4, LOW);

  }
  else
  {
    analogWrite(MOTORA_IN_1, LOW);
    analogWrite(MOTORA_IN_2, speed);
    analogWrite(MOTORB_IN_3, LOW);
    analogWrite(MOTORB_IN_4, speed);
  }
}

void stop()
{
  digitalWrite(MOTORA_IN_1, HIGH);
  digitalWrite(MOTORB_IN_3, HIGH);
  digitalWrite(MOTORA_IN_2, HIGH);
  digitalWrite(MOTORB_IN_4, HIGH);
}

void standby()
{
  digitalWrite(MOTORA_IN_1, LOW);
  digitalWrite(MOTORB_IN_3, LOW);
  digitalWrite(MOTORA_IN_2, LOW);
  digitalWrite(MOTORB_IN_4, LOW);
}


void setup() {
  pinMode(MOTORA_IN_1, OUTPUT);
  pinMode(MOTORA_IN_2, OUTPUT);
  pinMode(MOTORB_IN_3, OUTPUT);
  pinMode(MOTORB_IN_4, OUTPUT);

}

void loop() {
  static uint16_t i = 0;
  for(i = 100; i < 256; i++)
  {
    move(FORWARD, i);
    delay(50);
  }
  stop();
  delay(5000);
  for(i = 100; i < 256; i++)
  {
    move(BACKWARD, i);
    delay(50);
  }
  stop();
  delay(5000);
}
