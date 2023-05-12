#pragma once

#include "WiFi.h"
#include "SimplePgSQL.h"
#include "UART.h"
#include "LiquidCrystal.h"
#include "tcpClient.hpp"
extern LiquidCrystal lcd;
extern tcpClient website;

class postgresESP{
    IPAddress ip;
    char databasebuffer[1024];
    WiFiClient client;
    PGconnection connection;

    char queryBuffer[256];
    int port;
    public:

    postgresESP(int a, int b, int c, int d, int port)
    : connection(&client, 0, 1024, databasebuffer),
    ip(a,b,c,d),
    port(port)
    {
    }

    void connect(const char * username, const char * pass, const char * dbs){
        Serial.println("attempting connection to dbs");
        Serial.print("user:");
        Serial.println(username);
        Serial.print("pass:");
        Serial.println(pass);
        Serial.print("dbs:");
        Serial.println(dbs);

        lcd.clear();
        lcd.home();
        lcd.print("sql connect");
        lcd.setCursor(0,1);
        int lcdpos = 0;
        connection.setDbLogin(ip, username, pass, dbs, "utf8",port);
        while (connection.status() != CONNECTION_OK){
            Serial.println(connection.status());
            Serial.println(connection.getMessage());
            lcd.print(".");
            lcd.setCursor(++lcdpos, 1);
            if (lcdpos > 16){
                lcd.clear();
                lcd.home();
                lcd.print("sql connect");
                lcd.setCursor(0,1);
                lcdpos = 0;
            }
            delay(500);
        }

        Serial.println("\nConnection to database succesfull");
    }


    void setArtikel(char * pkey, int add){
        if (add)
            sprintf(queryBuffer, "UPDATE artikel SET beschikbaar = beschikbaar + 1 where code like '%s';", pkey);
        else 
            sprintf(queryBuffer, "UPDATE artikel SET beschikbaar = beschikbaar - 1 WHERE code like '%s';", pkey);
        website.log(queryBuffer+60);
        connection.execute(queryBuffer);
    }

    char *  checkUser(char * userId){
        website.log(userId);
        memset(queryBuffer,0,256);
        sprintf(queryBuffer, "SELECT naam FROM gebruiker WHERE id like '%s';", userId);
        int a = connection.execute(queryBuffer);
        Serial.println(queryBuffer);
        website.log(queryBuffer + 20);
        return handleReturn();
    }
    char outBuffer[20] = {0};

    char * handleReturn(){
        char * out;
        while (1){
            int rc = connection.getData();
            if (rc & PG_RSTAT_HAVE_COLUMNS){
                Serial.println("col");
                Serial.println(connection.nfields());
                Serial.println(connection.getColumn(0));
            } else if (rc & PG_RSTAT_HAVE_SUMMARY){
                Serial.println("message");
                Serial.println(connection.ntuples());
            } else if (rc & PG_RSTAT_HAVE_ROW){
                Serial.println("row");
                Serial.println(connection.getValue(0));
                //out = connection.getValue(1);
                strcpy(outBuffer, connection.getValue(0));
                out = outBuffer;
            }
            if (rc & PG_RSTAT_READY)
                return out;
            delay(50);
        }
    }
    char * latestLogin(){
        if (strcmp(connection.getColumn(0), "naam") == 0 )
            return connection.getValue(0);
        return 0;
    }
};