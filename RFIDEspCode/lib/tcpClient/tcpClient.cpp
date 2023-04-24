#include "tcpClient.hpp"



tcpClient::tcpClient(const char * host, const uint16_t port)
:connection(host, port)
{
}

void tcpClient::connect()
 {
    //wait for wifi if not already connected
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
    }
    Serial.println("connecting to website");
    //wait for the client to connect to the server
    Serial.println(client.available());
    Serial.println(client.connected());
    int res = client.connect(connection.host, connection.port);
    while(!res){
        Serial.println(connection.host);
        Serial.println(connection.port);
        Serial.println(res);
        res = client.connect(connection.host, connection.port);
        delay(500);
    }
    isConnected = true;
    Serial.println("\nconnected");
}

tcpClient::~tcpClient(){
    Serial.println("disconnecting client");
    client.stop();
}


void tcpClient::send(const char * message){
    Serial.print("sending ");
    Serial.println(message);
    if(!isConnected) return;
    client.print(message);
}
void tcpClient::send(const std::string & message){
    tcpClient::send(message.c_str());
}
const char * const tcpClient::receive(){
    if(!isConnected) return 0;
    //wait indefinitly for data to become available
    while(!client.available()){
        delay(10);
    }
    int amountRead = client.readBytesUntil('\n',message,32);
    message[amountRead+1] = 0; //add null netminator
    return message;
}

void tcpClient::mainLoop(){
    if(!isConnected) return;

    static int stringindex = 0;
    //if char is available read it
    while (client.available()){
        char r = client.read();
        if (r == 0) continue;
        message[stringindex] = r;
        /*Serial.print(r);
        Serial.print(" ");
        Serial.print((int) r);
        Serial.print(" ");
        Serial.println(stringindex);*/
        stringindex++;
        //if the char is \0 or the buffer is full, send it out and restart
        if (r == '\n' || stringindex == 33){
            message[stringindex] = '\0';
            callback(message);
            stringindex = 0;
        }
    }
}

void tcpClient::registerCallback(void (*callback)(const char *)){
    this->callback = callback;
}