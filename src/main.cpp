#include <Arduino.h>
#include <mySX1280.h>

mySX1280 myLora;

void setup()
{
  Serial.begin(9600);
  myLora.Setup();
}

void loop()
{
  // tone(buzzer, 800);
  // delay(500);
  // noTone(buzzer);
  // delay(500);
}