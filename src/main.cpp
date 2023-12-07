#include <Arduino.h>
#include <mySX1280.h>

#define COMMAND_BUFFER_SIZE 8
#define DATA_BUFFER_SIZE 256

mySX1280 myLora;

char cmd[COMMAND_BUFFER_SIZE], data[DATA_BUFFER_SIZE];

void transmit(String message)
{
  if (message.length() == 0)
  {
    return;
  }

  String msg_inc_uid = myLora.GetUID() + "#" + message;

  uint8_t dataArray[msg_inc_uid.length() + 1];
  msg_inc_uid.toCharArray((char *)dataArray, msg_inc_uid.length() + 1);

  myLora.Transmit(dataArray, sizeof(dataArray));
  Serial.println(message);
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


void sendSF() {
  uint8_t SF = myLora.GetSF();
  Serial.println(SF);  // (Human readable)
}

void sendBW() {
  uint32_t BW = myLora.GetBW();
  Serial.println(BW);  // (Human readable)
}

void sendCR() {
  uint8_t CR = myLora.GetCR();
  Serial.println(CR);  // (Human readable)
}

int get_config() {
  Serial.readBytesUntil('\n', cmd, DATA_BUFFER_SIZE);

  if (strcmp(cmd, "SF") == 0) {
        sendSF();
        Serial.println("OK");

    return 0;
  }

  if (strcmp(cmd, "BW") == 0) {
        sendBW();
        Serial.println("OK");

    return 0;
  }

  if (strcmp(cmd, "CR") == 0) {
        sendCR(); 
        Serial.println("OK");
    
    return 0;
  }

    Serial.println("ERROR");
  return -1;
}

int setSF(uint8_t SF) {
  if (SF < 5 || SF > 12) {
    Serial.println("ERROR INVALID SF (5-12)");
    return -1;
  }
  // Convert to actual SF
  int SF_val = SF * 16;

  // Update settings expects the big representation of SF:
  myLora.UpdateSettings(SF_val, myLora.GetCodedBW(), myLora.GetCodedCR());

  Serial.print("SET Spreading Factor: ");
  sendSF();
  return 0;
}

int setBW(long int BW) {
  // Convert to actual BW
  uint8_t coded_BW;
  switch (BW)
  {
    case 203125:
      coded_BW = 0x34;
      break;

    case 406250:
      coded_BW = 0x26;
      break;

    case 812500:
      coded_BW = 0x18;
      break;

    case 1625000:
      coded_BW = 0x0A;
      break;

    default:
      Serial.println("ERROR INVALID BANDWIDTH");
      return -1;
  }

  // Update settings does not expect the human readable inputs
  myLora.UpdateSettings(myLora.GetCodedBW(), coded_BW, myLora.GetCodedCR());
  Serial.print("SET BandWidth: ");
  sendBW();

  return 0;
}

int setCR(uint8_t CR) {
  if (CR < 5 || CR > 8) {
    Serial.println("ERROR INVALID CR (4-8)");
    return -1;
  }

  myLora.UpdateSettings(myLora.GetCodedSF(), myLora.GetCodedBW(), CR - 4);

  Serial.print("SET Coding Rate: ");
  sendCR();

  return 0;
}

int set_config() {
  // Read command
  Serial.readBytesUntil('=', cmd, COMMAND_BUFFER_SIZE);

  float val = Serial.parseFloat();  // Get number after the =

  // Do this
  Serial.readBytesUntil('\n', data, DATA_BUFFER_SIZE);

  // Switch case doesn't work for strings
  if (strcmp(cmd, "SF") == 0) {
    setSF(val);

        Serial.println("OK");

    return 0;
  }

  if (strcmp(cmd, "BW") == 0) {
    setBW(val);

        Serial.println("OK");

    return 0;
  }

  if (strcmp(cmd, "CR") == 0) {
    setCR(val);
        
        Serial.println("OK");
    
    return 0;
  }

    Serial.println("ERROR");

  return -1;
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

int handle_command() {
    memset(cmd, 0, COMMAND_BUFFER_SIZE);
    memset(data, 0, DATA_BUFFER_SIZE);

    char c = Serial.read();  // Read off the command type (eg !, ?, +)

    switch (c) {
        case '!': return set_config();
        case '?':  return get_config();
        //case '+': return parse_packet();
        default: 
            String data = Serial.readStringUntil('\n');
            transmit(c + data);  // Include the first character that was read off
    }

    return -1;
};

void loop()
{
  // When theres stuff on the serial, go handle it.
  if (Serial.available() > 0) handle_command();
  

  myLora.Receive();
  delay(25);
}
