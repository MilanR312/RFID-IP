#pragma once

#include "WiFi.h"
#include "assert.h"
#include <string>
#include "Array.hpp"
//credit
//https://gist.github.com/fazaio/50c7f62c10c21e3947c8319cc5142ed1




struct connectionDetails{
    const char * host;
    const uint16_t port;
    connectionDetails(const char * host, const uint16_t port)
    : host(host), port(port)
    {}
};

class tcpClient{
    connectionDetails connection;

    WiFiClient client;
    //max receiving message length is 32
    char message[33] = {0};
    //allocate space for callback
    bool isConnected = false;
    void (*callback)(const char *) = 0;
    
    public:

    //name of the device
    std::string name;
    //location of the device
    std::string location;

    char lastLoggedInUser[33] = "something long";

    tcpClient(const char * host, const uint16_t port);

    void connect();
    ~tcpClient();
    /**
     * send the message to the server
    */
    void send(const char * message);
    void send(const std::string & message);
    /**
     * receive a message that is saved in the message variable
     * code blocks until read
     * @return a pointer to the received c string
    */
    [[deprecated("try to use the callback version")]]
    const char * const receive();
    /**
     * receives a message and executes the callback when it is received
     * copy the string in the callback!!!
    */
    void registerCallback(void (*callback)(const char *));

    /**
     * use in mainloop to allow for callback running
    */
    void mainLoop();
};