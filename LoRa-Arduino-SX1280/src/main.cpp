#include <Arduino.h>
#include "registerTest.h"
#include "mySX1280.h"

RegisterTest myRegister;

// mySX1280 myLora; //Uncomment to test transmission
// uint8_t message[] = "Aamir benches more than Kush"; //Uncomment to test transmission


void setup(){
  Serial.begin(9600);
  myRegister.setup(); // Comment during transmission test
  // myLora.setup(); //Uncomment to test transmission
}
void loop(){
  myRegister.loop(); // Comment during transmission test
  // myLora.transmit(message, sizeof(message)); //Uncomment to test transmission
}