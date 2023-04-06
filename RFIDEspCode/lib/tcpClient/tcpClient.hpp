#pragma once

#include "WiFi.h"
//credit
//https://gist.github.com/fazaio/50c7f62c10c21e3947c8319cc5142ed1



class tcpClient{
    WiFiClient client;
    //max message length is 32
    char message[33] = {0};

    bool isConnected = false;
    //allocate space for callback
    bool isFinished = false;
    void (*callback)(const char *) = 0;
    public:

    void connect(const char * host, const uint16_t port);
    ~tcpClient();
    /**
     * send the message to the server
    */
    inline void send(const char message[]);
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
    inline void registerCallback(void (*callback)(const char *));

    /**
     * use in mainloop to allow for callback running
    */
    void mainLoop();
};