#include <Arduino.h>
// #include <mySX1280.h>

const int buzzer = 10; // buzzer to arduino pin 9

void setup()
{
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
}

void loop()
{
  tone(buzzer, 800);
  delay(500);
  noTone(buzzer);
  delay(500);
}