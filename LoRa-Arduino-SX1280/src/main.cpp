/*******************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 11/02/20

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

/*******************************************************************************************************
  Program Operation - This program is stand alone, it is not necessary to install the SX12XX-LoRa library
  to use it. This test program is for the SX128X LoRa devices.

  The program checks that a SX128X LoRa device can be accessed by doing a test register write and read.
  If there is no device found a message is printed on the serial monitor. The contents of the registers
  from 0x00 to 0x7F are printed, there is a copy of a typical printout below. Note that the read back
  changed frequency may be slightly different to the programmed frequency, there is a rounding error due
  to the use of floats to calculate the frequency.

  The Arduino pin numbers that the NSS and NRESET pins on the LoRa device are connected to must be
  specified in the hardware definitions section below. The LoRa device type in use, SX1280 or SX1281
  must be specified also.

  Typical printout;

  2_Register_Test Starting
  Reset device
  LoRa Device found
  Reset device
  Registers at reset
  Reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
  0x900  80 0C 7B 02 20 FA C0 00 00 80 00 00 00 00 00 FF
  0x910  FF FF 00 00 00 19 00 00 00 19 87 65 43 21 7F FF
  0x920  FF FF FF 0C 70 37 0A 50 D0 80 00 C0 5F D2 8F 0A
  0x930  00 C0 00 00 00 24 00 21 28 B0 30 09 1A 59 70 08
  0x940  58 0B 32 0A 14 24 6A 96 00 18 00 00 00 00 00 00
  0x950  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x960  00 00 00 00 00 00 00 00 00 00 FF FF FF FF FF FF
  0x970  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 04
  0x980  00 0B 18 70 00 00 00 4C 00 F0 64 00 00 00 00 00
  0x990  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x9A0  00 08 EC B8 9D 8A E6 66 06 00 00 00 00 00 00 00
  0x9B0  00 08 EC B8 9D 8A E6 66 06 00 00 00 00 00 00 00
  0x9C0  00 16 00 3F E8 01 FF FF FF FF 5E 4D 25 10 55 55
  0x9D0  55 55 55 55 55 55 55 55 55 55 55 55 55 00 00 00
  0x9E0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
  0x9F0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

   Frequency at reset 2495996672hz
  Change Frequency to 2445000000hz
        Frequency now 2444999936hz

  Reg    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
  0x900  80 0C 7B 02 20 FA BC 13 C1 80 00 00 00 00 00 61

Serial monitor baud rate is set at 9600.
*******************************************************************************************************/
#include <Arduino.h>
#include "registerTest.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello World!");
  delay(1000);
}