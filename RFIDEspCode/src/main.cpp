#include <Arduino.h>
#include "WiFi.h"
#include "tcpClient.hpp"
#include "SimplePgSQL.h"
#include <cstdio>
const char * ssid = "IoTdevices";
const char * password = "FGwrdsa=ghaR";

Array<bool, 2> buttonLink;


tcpClient website("172.22.31.194", 8090);
WiFiClient client;

//Database related defining en setup
char databasebuffer[1024];
PGconnection conn(&client, 0, 1024, databasebuffer);
IPAddress PGIP(172,22,31,194); 
char query[256];

void connectToDB(){
  conn.setDbLogin(PGIP, "postgres", "", "RFID", "UTF8", 5432 );
  while (conn.status() != CONNECTION_OK){
    if (conn.status() == CONNECTION_NEEDED){
      conn.setDbLogin(PGIP, "postgres", "", "RFID", "UTF8", 5432 );
    }
    else if (conn.status() == CONNECTION_AWAITING_RESPONSE){
      Serial.println("Awaiting connection status");
    }
  }
  Serial.println("Connection to Database succesful");
}

// uint8_t*
void processData(int data, int length, bool add){
  // PRODUCT
  // -> if motion high: UPDATE - 1 querry
  // -> if motion low: UPDATE + 1 querry

  // USER
  // -> if SELECT name querry works: open door + display name on LCD
  // -> if SELECT name querry fails: display "no acces"

  char type[16] = {"PRODUCT"};
  char pkey[16] = {"abc"};

  //PRODUCT
  if (strcmp(type, "PRODUCT") == 0){
    if (add == true){
      sprintf(query, "UPDATE artikel SET beschikbaar = beschikbaar + 1 WHERE code like \"%s\"", pkey);
    }
    else{
      sprintf(query, "UPDATE artikel SET beschikbaar = beschikbaar - 1 WHERE code like \"%s\"", pkey);
    }
    conn.execute(query);
  }

  //GEBRUIKER
  else if (strcmp(type, "USER") == 0){
    sprintf(query, "SELECT naam FROM gebruiker WHERE id like \"%s\"", pkey);
    conn.execute(query);
    if (conn.getData() < 0 ){
      Serial.println("No acces, or an error occured");
      connectToDB();
    }
    else{
      Serial.println(conn.getValue(1));
    }
  }

  //INVALID RFID TAG
  else{
    Serial.println("No valid RFID-tag");
  }
}

void handleMessage(const char * message){
  char buffer[33] = {0};
  strcpy(buffer, message);
  Serial.print("got message ");
  Serial.println(message);
  if (strstr(buffer, "GET") != NULL){
    //request data
    if (strstr(buffer, "NAME") != NULL)
      return website.send(website.name);
    if (strstr(buffer, "LOCATION") != NULL)
      return website.send(website.location);
    if (char * loc = strstr(buffer, "BUTTON")){
      //goto number after
      loc += 6;
      int num = *loc - '0';
      char toSend[] = "0";
      toSend[0] = buttonLink[num] + '0';
      website.send(toSend);
    }
  }
  if (strstr(buffer, "SET") != NULL){
    if (char * loc = strstr(buffer, "BUTTON")){
      //goto number after
      loc += 6;
      int num = *loc - '0';
      bool received = *(loc+1) - '0';
      Serial.print("setting button ");
      Serial.print(num);
      Serial.print(" to value ");
      Serial.println(received);

      buttonLink[num] = received;
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("logging in with");
  Serial.println(ssid);
  Serial.println(password);

  WiFi.mode(WIFI_STA);
  WiFi.setMinSecurity(WIFI_AUTH_WPA_PSK);
  WiFi.begin(ssid, password);
  uint8_t tryAmount = 0;
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    tryAmount++;
    Serial.println(WiFi.status());
    //Serial.print(".");
    
    if (tryAmount > 50){
      Serial.println("reset due to infinite connection");
      ESP.restart();
    }
  }

  
  Serial.println("\nconnected");

  website.connect();
  website.name = "ESP32Test";
  website.location = "testingLocation";
  website.registerCallback(handleMessage);

  int test = 0;
  int testlen = 0;
  connectToDB();
  processData(test, testlen, true);
}

void loop() {
  //Serial.println(website.receive());
  website.mainLoop();
}