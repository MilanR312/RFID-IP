#include <Arduino.h>
#include "WiFi.h"
#include "tcpClient.hpp"
const char * ssid = "IoTdevices";
const char * password = "FGwrdsa=ghaR";

Array<bool, 2> buttonLink;


tcpClient website("172.22.31.194", 8090);


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
}

void loop() {
  //Serial.println(website.receive());
  website.mainLoop();
}