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
    bool isOpeningOrClosing = false;
    int tries = 0;
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
        if (!digitalRead(m1.pin) || !digitalRead(m2.pin)) // wanneer iemand aant wachten is op opening vd deur
        {
            website.log("person detected at door");
            lcd.print("De deur wordt geopend");
            website.log("opening door");
            digitalWrite(deurPin, HIGH);
            if (digitalRead(deurSensPin) == HIGH && !isOpeningOrClosing) // Blijkt dat de deur toch nog dicht is en ze is nog niet aant openen
            {
                open = false;                // zeggen dat de deur niet open is
                isOpeningOrClosing = true;   // zeggen dat de deur aan het openen is
                digitalWrite(deurPin, HIGH); // de deur weer open zetten
                tries++;
                delay(1000); // seconde wachten
            }
            else if (digitalRead(deurSensPin) == LOW && isOpeningOrClosing)
            {              // als de deur uiteindelijk geopend is
                tries = 0; // aantal tries resetten
                open = true;
                isOpeningOrClosing = false;
                gettimeofday(&door_last_opened, NULL); // tijd van opening onthouden
            }
            else
            {
                tries++;
                if (tries >= 5)
                { // meer dan 5 keer ni gelukt -> error printen
                    lcd.print("deur wilt niet openen");
                }
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

                if (digitalRead(deurSensPin) == LOW && !isOpeningOrClosing) // Blijkt dat de deur toch nog open is en ze is nog aant dichtgaan is
                {
                    open = true;                // zeggen dat de deur niet open is
                    isOpeningOrClosing = true;  // zeggen dat de deur aan het sluiten
                    digitalWrite(deurPin, LOW); // de deur weer dicht doen
                    tries++;
                    delay(1000); // seconde wachten
                }
                else if (digitalRead(deurSensPin) == HIGH && isOpeningOrClosing)
                {              // als de deur uiteindelijk dicht is
                    tries = 0; // aantal tries resetten
                    open = false;
                }
                else
                {
                    tries++;
                    if (tries >= 5)
                    { // meer dan 5 keer ni gelukt -> error printen
                        lcd.print("deur wilt niet sluiten");
                    }
                }
                isAllowedEntry = false;
            }
        }
    }

    int
    getDirection()
    {
        int out = 0;
        if (m1.getTime() > m2.getTime())
            out = 1;
        if (m2.getTime() > m1.getTime())
            out = -1;
        return out;
    }
};