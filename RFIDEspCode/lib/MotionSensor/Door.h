#include "MotionSensor.h"
#include "tcpClient.hpp"

extern tcpClient website;

class Door
{
    MotionSensor m1;
    MotionSensor m2;

    int deurSensPin;
    int deurPin;
    struct timeval door_last_opened;
    void (*callback)(void);

    int prev;

public:
    bool open = false;

    bool forcedOpen = false;
    bool isAllowedEntry = false;

    Door(int pin1, int pin2, int deurSens, int deur)
        : m1(pin1), m2(pin2), deurPin(deur), deurSensPin(deurSens)
    {
        pinMode(deurSens, INPUT);
        pinMode(deur, OUTPUT);
    }
    void loop()
    {
        if (forcedOpen)
        {
            digitalWrite(deurPin, HIGH);
            return;
        }
        if (!isAllowedEntry)
        {
            digitalWrite(deurPin, LOW);
            return;
        }
        m1.sense();
        m2.sense();
        getDirection();
        int tries = 0;
        if (!digitalRead(m1.pin) || !digitalRead(m2.pin))
        {
            website.log("person detected at door");
            lcd.print("De deur wordt geopend");
            website.log("opening door");
            digitalWrite(deurPin, HIGH);
            open = true;
            while (digitalRead(deurPin) == LOW) // zolang de deur toch dicht blijft
            {
                if (tries >= 250)
                { // na zoveel tijd komt er een error
                    lcd.print("ERROR: deur kan niet open");
                    open = false;
                    break;
                }
                delay(500);
                tries++;
                digitalWrite(deurPin, HIGH);           // blijven proberen om de deur te openen
                gettimeofday(&door_last_opened, NULL); // telkens opvragen wanneer de deur geopend zou worden
            }
        }

        if (open)
        {
            struct timeval tv_now;
            gettimeofday(&tv_now, NULL);
            // all time in microSecons
            int64_t time_curr = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
            int64_t time_old = (int64_t)door_last_opened.tv_sec * 1000000L + (int64_t)door_last_opened.tv_usec;
            if (time_curr - time_old > 10000000)
            { // 10 seconds
                website.send(LOG, "closing door");
                digitalWrite(deurPin, LOW);
                open = false;
                while (digitalRead(deurPin) == HIGH) // zolang de deur toch open blijft
                {
                    if (tries >= 250)
                    { // na zoveel tijd komt er een error
                        lcd.print("ERROR: deur kan niet dicht");
                        open = true;
                        break;
                    }
                    delay(500);
                    tries++;
                    digitalWrite(deurPin, LOW); // blijven proberen om de deur te sluiten
                }
                isAllowedEntry = false;
            }
        }
    }

    int getDirection()
    {
        int out = 0;
        if (m1.getTime() > m2.getTime())
            out = 1;
        if (m2.getTime() > m1.getTime())
            out = -1;
        return out;
    }
};