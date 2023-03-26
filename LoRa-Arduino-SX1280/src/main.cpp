#include <Arduino.h>
#include "registerTest.h"
#include "mySX1280.h"

RegisterTest myRegister;

mySX1280 myLora;
uint8_t message[] = "Aamir benches more than Kush";


void setup(){
  Serial.begin(9600);
  // myRegister.setup();
  myLora.Setup();
}
void loop(){
  // myRegister.loop();
  // myLora.Receive();
  myLora.Transmit(message, sizeof(message));
}