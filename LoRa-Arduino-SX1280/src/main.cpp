#include <Arduino.h>
#include <mySX1280.h>

mySX1280 myLora;
uint8_t message[] = "Aamir benches more than Kush";
unsigned short int mode; // 0 is recieving and 1 is transmitting
void setup()
{
  Serial.begin(9600);
  Serial.print("Enter a mode (0=receiving, 1=transmitting): ");
  while (1)
  {
    if (Serial.available())
    {
      char input_mode = Serial.read();
      Serial.println(input_mode);
      if (input_mode == '0')
      {
        Serial.println("\033[0;32mLora set to recieving mode\033[0m");
        mode = 0; // recieving
        break;
      }
      else if (input_mode == '1')
      {
        Serial.println("\033[0;32mLora set to transmitting mode\033[0m");
        mode = 1; // transmitting
        break;
      }
      else
      {
        Serial.println("Enter a mode (0=receiving, 1=transmitting): ");
      }
    }
    
  }
  Serial.println();
  myLora.Setup();
}
void loop()
{
  if (mode == 0)
  { // If Recieving
    myLora.Receive();
  }
  else if (mode == 1)
  { // If Transmitting
    myLora.Transmit(message, sizeof(message));
  }
}