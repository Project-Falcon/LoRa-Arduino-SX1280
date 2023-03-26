#include <Arduino.h>
#include "registerTest.h"

RegisterTest myRegister;

void setup(){
  myRegister.setup();
}
void loop(){
  myRegister.loop();
}