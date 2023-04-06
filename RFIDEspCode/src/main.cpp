#include <Arduino.h>
#include "WiFi.h"
#include "tcpClient.hpp"
const char * ssid = "SSID";
const char * password = "Pass";

tcpClient website;


void handleMessage(const char * message){
  char buffer[33] = {0};
  strcpy(buffer, message);
  if (strstr(buffer, "GETNAME") != NULL){
    //enz
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

  website.connect("ip addres here", 8090);
  website.registerCallback(handleMessage);
}

void loop() {
  website.mainLoop();
}