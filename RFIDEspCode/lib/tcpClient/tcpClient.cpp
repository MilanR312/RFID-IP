#include "tcpClient.hpp"



void tcpClient::connect(const char * host, const uint16_t port) {
    //wait for wifi if not already connected
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
    }
    Serial.println("connecting to website");
    //wait for the client to connect to the server
    while(!client.connect(host,port)){
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nconnected");
}
tcpClient::~tcpClient(){
    Serial.println("disconnecting client");
    client.stop();
}


void tcpClient::send(const char * message){
    if(!isConnected) return;
    client.print(message);
}

const char * const tcpClient::receive(){
    if(!isConnected) return;
    //wait indefinitly for data to become available
    while(!client.available()){
        delay(10);
    }
    int amountRead = client.readBytesUntil('\0',message,32);
    message[amountRead+1] = 0; //add null netminator
    return message;
}

void tcpClient::mainLoop(){
    if(!isConnected) return;

    static int stringindex = 0;
    //if char is available read it
    while (client.available()){
        char r = client.read();
        message[stringindex] = r;
        stringindex++;
        //if the char is \0 or the buffer is full, send it out and restart
        if (r == '\0' || stringindex == 33){
            message[stringindex] = '\0';
            callback(message);
            stringindex = 0;
        }
    }
}

void tcpClient::registerCallback(void (*callback)(const char *)){
    if(!isConnected) return;
    this->callback = callback;
}