#include <Arduino.h>
#include <mySX1280.h>

mySX1280 myLora;

void transmit(String message)
{
  if (message.length() == 0)
  {
    return;
  }

  String msg_inc_uid = myLora.GetUID() + "#" + message;

  uint8_t dataArray[msg_inc_uid.length() + 1];
  msg_inc_uid.toCharArray(dataArray, msg_inc_uid.length() + 1);

  myLora.Transmit(dataArray, sizeof(dataArray));
}

// https://stackoverflow.com/questions/9072320/split-string-into-string-array
String getSplitStringValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void update_testcase()
{
  String data = Serial.readStringUntil('\n');

  String newBW, newSF, newCR;

  newBW = getSplitStringValue(data, ',', 0);
  newSF = getSplitStringValue(data, ',', 1);
  newCR = getSplitStringValue(data, ',', 2);

  uint8_t BW, SF, CR;

  if (newBW == "4")
  {
    BW = 0x26;
  }
  else if (newBW == "8")
  {
    BW = 0x18;
  }
  else
  {
    BW = 0x26;
  }

  if (newSF == "7")
  {
    SF = 0x70;
  }
  else if (newSF == "8")
  {
    SF = 0x80;
  }
  else if (newSF == "9")
  {
    SF = 0x90;
  }
  else if (newSF == "10")
  {
    SF = 0xA0;
  }
  else
  {
    SF = 0x70;
  }

  if (newCR == "6")
  {
    CR = 0x01;
  }
  else if (newCR == "7")
  {
    CR = 0x03;
  }
  else
  {
    CR = 0x02;
  }

  myLora.UpdateSettings(SF, BW, CR);
}

void setup()
{
  Serial.begin(9600);
  Serial.setTimeout(300);
  myLora.Setup();
  Serial.println(F("Enter a UID"));
  while (1)
  {
    if (Serial.available() > 0)
    {
      String uid = Serial.readStringUntil('\n');
      myLora.SetUID(uid);
      break;
    }
  }
  Serial.print(F("UID is: "));
  Serial.println(myLora.GetUID());

  Serial.println(F("Ready"));
}

void loop()
{
  while (Serial.available())
  {
    String data = Serial.readStringUntil('\n');
    transmit(data);
    delay(100);
    break;
  }

  myLora.Receive();
  delay(100);
}
