#include <Arduino.h>
#include "WiFi.h"
#include "tcpClient.hpp"
#include "UNIT_UHF_RFID.h"
#include "LiquidCrystal.h"
#include "sql.h"
#include "Door.h"
#include <cstdio>
const char *ssid = "IoTdevices";
const char *password = "FGwrdsa=ghaR";

#define RFID
#define WEBSITE
#define SQL
// #define WRITE
// buttonLink[0] -> doorOverride
Array<bool, 2> buttonLink;

#ifdef WEBSITE
tcpClient website("172.22.31.194", 8090);
#endif

#ifdef SQL
postgresESP sql(172, 22, 31, 194, 5432);
Array<uint8_t, 4> read_buffer;
#endif

Unit_UHF_RFID rfid;

Door dr(32, 33, 26, 25);
// rs en D4-D7
LiquidCrystal lcd(4, 18, 19, 21, 22, 23);

// print text or new . and reset if > 16
void printBar(const char *message1, uint8_t &num)
{
  if (num == 0 || num > 16)
  {
    lcd.clear();
    lcd.print(message1);
    lcd.setCursor(0, 1);
    num = 0;
  }
  else
  {
    lcd.setCursor(num, 1);
  }
  lcd.print(".");
}

// receive incomming message and execute
void handleMessageV2(const char *message)
{
  char buffer[33] = {0};
  char sendBuffer[32] = {};
  strcpy(buffer, message);
  Serial.print("got message ");
  Serial.println(message);
  Serial.println(buffer[0]);
  Serial.println(buffer[1]);
  char instr = buffer[0];
  int rw = (buffer[1] - '0');
  if (rw == 0)
  { // read
    switch (instr - '0')
    {
    case NAME:
      snprintf(sendBuffer, 32, "%c%-32s", instr, website.name.c_str());
      website.send(sendBuffer);
      break;
    case LOCATION:
      snprintf(sendBuffer, 32, "%c%-32s", instr, website.location.c_str());
      website.send(sendBuffer);
      break;
    case LOGIN:
      snprintf(sendBuffer, 32, "%c%-32s", instr, website.lastLoggedInUser);
      website.send(sendBuffer);
      break;
    case BUTTON:
    {
      // read data byte
      int dev = buffer[2];
      int buttonIndex = dev - '0';
      Serial.print("index=");
      Serial.println(buttonIndex);
      int buttonData = buttonLink[buttonIndex];
      Serial.print("data =");
      Serial.println(buttonData);
      snprintf(sendBuffer, 32, "%c%c%-31c", instr, (char)buttonIndex + '0', (char)buttonData + '0');
      Serial.println(sendBuffer);
      website.send(sendBuffer);
      break;
    }
    case ENABLED:
      website.send(ENABLED, "0");
      break;
    }
  }
  else
  {
    switch (instr - '0')
    {
    case BUTTON:
      int index = buffer[2];
      int newState = buffer[3];
      Serial.println("setting button");
      buttonLink[index - '0'] = newState - '0';
      snprintf(sendBuffer, 32, "%c%c%-31c", instr, (char)index, (char)newState);
      website.send(sendBuffer);
      // index = 0 && newState == 1 => red (2)
      // index = 1 && newState == 1 => yellow (1)
      Serial.print("buttons:");
      Serial.print(buttonLink[0]);
      Serial.print(" ");
      Serial.println(buttonLink[1]);
      if (buttonLink[0])
        website.send(ENABLED, "2");
      else if (buttonLink[1])
        website.send(ENABLED, "1");
      else
        website.send(ENABLED, "0");
    }
  }
}

// tx = 14 , rx = 12

void checkUser(char *buff)
{
#ifdef SQL

  if (char *user = sql.checkUser(buff))
  {
    // copy name into lastloggedin
    strncpy(website.lastLoggedInUser, user, 32);

    website.send(LOGIN, website.lastLoggedInUser);

    lcd.clear();
    lcd.print(user);
    lcd.setCursor(0, 1);
    lcd.print("opened the door");
    website.log(user);
    if (!dr.forcedOpen)
      dr.isAllowedEntry = true;
  }
  else
  {
    Serial.println("no entry");
    lcd.clear();
    lcd.print("no entry");
  }

#else // als SQL server niet verbonden is laat iedereen toe
  if (!dr.forcedOpen)
    dr.isAllowedEntry = true;

#endif
  delay(2000);
}
void scanArtikel(char *buff)
{
#ifdef SQL
  int direction = dr.getDirection();
  char bf[25];
  snprintf(bf, 25, "product %s going %d", buff, direction);
  website.log(bf);
  if (direction != 0)
  {
    Serial.println("writing to dbs");
    sql.setArtikel(buff, direction < 0);

    delay(5000); // wait 5 seconds to let product get out of radius
    // fix this !! bad solution
  }
#endif
}

void setup()
{
  delay(5000);
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.home();
  pinMode(16, OUTPUT);
  digitalWrite(16, LOW);

  lcd.print("Starting up");

  delay(1000);
  lcd.clear();

  WiFi.mode(WIFI_STA);
  WiFi.setMinSecurity(WIFI_AUTH_WPA_PSK);
  WiFi.begin(ssid, password);

  uint8_t tryAmount = 0;

  printBar("WiFi connect", tryAmount);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(WiFi.status());
    tryAmount++;
    printBar("WiFi connect", tryAmount);
    delay(500);
  }
  lcd.clear();
  lcd.print("WiFi connected");
  delay(1000);
#ifdef RFID
  lcd.clear();
  lcd.print("Connecting RFID now");
  delay(5000);

  // rx tx
  rfid.begin(&Serial2, 115200, 14, 12, false);

  String info;
  tryAmount = 0;
  printBar("RFID connect", tryAmount);
  while (1)
  {
    info = rfid.getVersion();
    Serial.println(info);
    tryAmount++;
    printBar("RFID connect", tryAmount);
    if (info != "ERROR")
      break;
    delay(500);
  }
  lcd.clear();
  lcd.print("RFID connected");
  delay(2000);
  rfid.setTxPower(100);
#endif

#ifdef WEBSITE
  website.connect();
  website.name = "ESP32Test";
  website.location = "testingLocation";
  website.registerCallback(handleMessageV2);
#endif

#ifdef SQL
  sql.connect("ESP32", "ESP32", "RFID");
#endif

  lcd.clear();

#ifdef WRITE

  lcd.clear();
  lcd.print("writing in");
  lcd.setCursor(0, 1);
  for (int i = 3; i > 0; i--)
  {
    lcd.print(i);
    delay(1000);
  }
  uint8_t write_buffer[4] = {0};
  write_buffer[3] = '1';
  write_buffer[2] = '0';
  write_buffer[1] = '0';
  write_buffer[0] = 0b10000000; // user starts with 1
  // hardcode for program to chip
  lcd.clear();
  if (rfid.writeCard(write_buffer, sizeof(write_buffer), 0x04, 0,
                     0x00000000))
  {
    lcd.print("write Succesfull");
  }
  else
  {
    lcd.print("Write ERROR");
  }
  delay(1000);
#endif
  lcd.clear();
  lcd.print("Starting main   loop");
  delay(1000);
}

void loop()
{
#ifdef WEBSITE

  do
  {
    website.mainLoop();
    dr.loop();
    if (buttonLink[1])
    { // if esp is turned off
      lcd.clear();
      lcd.print("Door disabled");

      delay(500);
    }
    else if (buttonLink[0])
    { // if door forced open
      dr.forcedOpen = true;
      strncpy(website.lastLoggedInUser, "OVERRIDE", 32);

      lcd.clear();
      lcd.print("Forced open");
      // website.send(LOGIN, website.lastLoggedInUser);

      delay(1000);
    }
  } while (buttonLink[1] || buttonLink[0]); // turns off the esp32 (kinda)

  dr.forcedOpen = false;
  lcd.clear();

#endif

#ifdef RFID
  // read a tag if not forced open
  if (rfid.readCard(read_buffer.pdata(), read_buffer.length(), 0x04, 0, 0))
  {
    Serial.println("read succesfull");
    for (int i = 0; i < 4; i++)
    {
      Serial.print(read_buffer[i]);
      Serial.print(" ");
    }
    // card scanned is an user
    char buff[4];
    snprintf(buff, 4, "%c%c%c", read_buffer[1], read_buffer[2], read_buffer[3]);
    if (read_buffer[0] >> 7 == 1)
    {
      Serial.println("user scanned checking dbs");
      website.log("user has scanned card");
      checkUser(buff);
    }
    // card scanned is a product
    if (dr.open && read_buffer[0] >> 7 == 0)
    {
      Serial.println("product scanned getting direction");
      scanArtikel(buff);
    }
  }
#endif

  delay(50);
}