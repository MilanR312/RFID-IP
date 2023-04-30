#include <Arduino.h>
#include "WiFi.h"
#include "tcpClient.hpp"
#include "UNIT_UHF_RFID.h"
#include "LiquidCrystal.h"
#include "sql.h"
#include "Door.h"
#include <cstdio>
//const char * ssid = "IoTdevices";
//const char * password = "FGwrdsa=ghaR";



//#define RFID
#define WEBSITE
#define SQL

//buttonLink[0] -> doorOverride
Array<bool, 2> buttonLink;
tcpClient website("192.168.1.59", 8090);

postgresESP sql(192,168,1,59,  5432);

Unit_UHF_RFID rfid;
Array<uint8_t, 4> read_buffer;

//Door dr(2,0,16,4);

LiquidCrystal lcd(23,22,21,19,18,5);

enum state: byte{
    LOCATION,
    NAME,
    BUTTON,
    LOGIN
};

void handleMessageV2(const char * message){
  char buffer[33] = {0};
  char sendBuffer[32] = {};
  strcpy(buffer, message);
  Serial.print("got message ");
  Serial.println(message);
  Serial.println(buffer[0]);
  Serial.println(buffer[1]);
  char instr = buffer[0];
  int rw = (buffer[1] - '0');
  if (rw == 0){ // read
    switch(instr -'0'){
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
        //read data byte
        int dev = buffer[2];
        int buttonIndex = dev - '0';
        Serial.print("index=");
        Serial.println(buttonIndex);
        int buttonData = buttonLink[buttonIndex];
        Serial.print("data =");
        Serial.println(buttonData);
        snprintf(sendBuffer, 32, "%c%c%-31c", instr, (char)buttonIndex+'0', (char)buttonData+'0');
        Serial.println(sendBuffer);
        website.send(sendBuffer);
    }
  } else {
    switch(instr - '0'){
      case BUTTON:
        int index = buffer[2];
        int newState = buffer[3];
        Serial.println("setting button");
        buttonLink[index - '0'] = newState - '0';
        snprintf(sendBuffer, 32, "%c%c%-31c", instr, (char)index, (char)newState);
        website.send(sendBuffer);
    }
  }
}
void doorMotion(){
  Serial.println("someone went trough the door");
}
//tx = 14 , rx = 12


void setup() {
  delay(5000);
  Serial.begin(115200);
  lcd.begin(16,2);

  buttonLink[0] = 1;

  WiFi.mode(WIFI_STA);
  WiFi.setMinSecurity(WIFI_AUTH_WPA_PSK);
  WiFi.begin(ssid, password);
  uint8_t tryAmount = 0;

  lcd.clear();
  lcd.print("wifi connect");
  lcd.setCursor(0,1); //goto second line
  while(WiFi.status() != WL_CONNECTED){
    Serial.println(WiFi.status());

    if (tryAmount%5 == 0){
      lcd.print(".");
      lcd.setCursor(tryAmount/5,1);
    }
    delay(500);
    tryAmount++;
    if (tryAmount > 50){
      Serial.println("reset due to infinite connection");
      ESP.restart();
    }
  }
  Serial.println("\nconnected");

  #ifdef RFID
  rfid.begin(&Serial2, 115200, 14, 12, false);

  String info;
  while (1){
    info = rfid.getVersion();
    Serial.println(info);
    if (info != "ERROR")
      break;  
  }
  Serial.println("connected RFID");
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

  //dr.registerCallback(doorMotion);


/*
  uint8_t write_buffer[4] = {0};
  write_buffer[3] = 0b00000001;
  write_buffer[0]  = 0b10000000;
  //hardcode for program to chip
  if (rfid.writeCard(write_buffer, sizeof(write_buffer), 0x04, 0,
                          0x00000000)) {
      Serial.println("write Succesfull");
  } else {
      Serial.println("Write ERROR");
  }
  delay(10000);*/
}
void loop() {
  lcd.clear();
  lcd.print("testing");

  #ifdef RFID
  //read a tag
  if (rfid.readCard(read_buffer.pdata(), read_buffer.length(), 0x04, 0, 0)){
    Serial.println("read succesfull");
    for (int i = 0; i < 4; i++){
      Serial.print(read_buffer[i]);
      Serial.print(" ");
    }
    if (read_buffer[0] == 0b10000000){
      Serial.println("user scanned checking dbs");
      char buff[4];
      snprintf(buff, 4, "%d%d%d", read_buffer[1], read_buffer[2], read_buffer[3]);
      #ifdef SQL
      if(char * user = sql.checkUser(buff)){
        //user is allowed entry
        Serial.print(user);
        Serial.println(" is allowed entry");
        strncpy(website.lastLoggedInUser, user, 32);
        
        // fix this
        //website.send(website.lastLoggedInUser);
      } else {
        Serial.println("no entry");
      }
      #endif
    }
  }
  #endif
  #ifdef WEBSITE
  website.mainLoop();
  #endif
  //dr.loop();
  //Serial.print("direction = ");
  //Serial.println(dr.getDirection());
  delay(1000);
}