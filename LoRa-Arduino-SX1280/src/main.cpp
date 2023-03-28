#include <Arduino.h>
#include <mySX1280.h>
#include <Thread.h>
#include <ThreadController.h>

ThreadController control = ThreadController();
Thread receiving_thread = Thread();
Thread transmitting_thread = Thread();

mySX1280 myLora;
uint8_t message[] = "Aamir benches more than Kush";
unsigned short int mode; // 0 is receiving and 1 is transmitting

void receive()
{
  myLora.Receive();
}

void transmit()
{
  myLora.Transmit(message, sizeof(message));
}

void setup()
{
  Serial.begin(9600);
  // Serial.print("Enter a mode (0=receiving, 1=transmitting): ");
  // while (1)
  // {
  //   if (Serial.available())
  //   {
  //     char input_mode = Serial.read();
  //     Serial.println(input_mode);
  //     if (input_mode == '0')
  //     {
  //       Serial.println("\033[0;32mLora set to receiving mode\033[0m");
  //       mode = 0; // receiving
  //       break;
  //     }
  //     else if (input_mode == '1')
  //     {
  //       Serial.println("\033[0;32mLora set to transmitting mode\033[0m");
  //       mode = 1; // transmitting
  //       break;
  //     }
  //     else
  //     {
  //       Serial.println("Enter a mode (0=receiving, 1=transmitting): ");
  //     }
  //   }
  // }
  Serial.println();
  myLora.Setup();
  Serial.println();
  myLora.UpdateSettings(160, 10, 4);

  transmitting_thread.onRun(transmit);
  transmitting_thread.setInterval(1000);
  receiving_thread.onRun(receive);

  control.add(&receiving_thread);    // & to pass the pointer to it
  control.add(&transmitting_thread); // & to pass the pointer to it
}

void loop()
{
  control.run();
  delay(10);
}