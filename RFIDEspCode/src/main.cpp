#include <Arduino.h>
#include "WiFi.h"
#include "tcpClient.hpp"
const char * ssid = "SSID";
const char * password = "Pass";

Array<bool, 2> buttonLink;

tcpClient website("ip address here", 8090);



void handleMessage(const char * message){
  char buffer[33] = {0};
  strcpy(buffer, message);
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
      char * toSend = "0";
      toSend[0] = buttonLink[num] + '0';
      website.send(toSend);
    }
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nconnected");

  website.connect();
  website.name = "ESP32Test";
  website.location = "testingLocation";
  website.registerCallback(handleMessage);
}

void loop() {
  website.mainLoop();
}