#include "MotionSensor.h"
#include "tcpClient.hpp"

extern tcpClient website;

class Door
{
    MotionSensor m1; // sensor aan buitenkant
    MotionSensor m2; // sensor aan binnenkant

    int deurSensPin;
    int deurPin;
    struct timeval tv_now;
    struct timeval tim1; // hoelang het duurt om deur te openen
    struct timeval tim2; // hoelang de deur open is
    void (*callback)(void);

    int gemiddelde_opening_time; // in milliseconden
    int aantal_openingen;
    int opening_time_sum;

    int64_t timevalue1;
    int64_t timevalue2;
    int64_t timedif;

    int prev;

public:
    bool open = false;
    bool isOpening = false;
    bool isClosing = false;
    bool forcedOpen = false;
    bool isAllowedEntry = false;
    bool tim1_running = false;
    bool tim2_running = false;

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
        /*
        if (!isAllowedEntry)
        {
            digitalWrite(deurPin, LOW);
            lcd.print("Geen toegang");
            return;
        }
        */

        m1.sense();
        m2.sense();

        if ((!digitalRead(m1.pin) || (!digitalRead(m2.pin) && isAllowedEntry)) && digitalRead(deurSensPin) == LOW && !open && !isOpening) // wanneer iemand aant wachten is op opening vd deur
        {
            website.log("person detected at door");
            website.log("opening door");
            digitalWrite(deurPin, HIGH);
            isOpening = true;
            if (!tim1_running)
            {
                website.log("timer 1 started");
                isAllowedEntry = true;
                gettimeofday(&tim1, NULL);
                tim1_running = true;
                lcd.clear();
                lcd.print("De deur wordt   geopend");
                delay(1000);
            }
        }

        else if (isOpening)
        {
            // zeker zijn dat de deur blijft openen
            digitalWrite(deurPin, HIGH);
            // tijd calculations uitvoeren
            gettimeofday(&tv_now, NULL);
            timevalue1 = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
            timevalue2 = (int64_t)tim1.tv_sec * 1000000L + (int64_t)tim1.tv_usec;
            timedif = (timevalue1 - timevalue2) / 1000;
            // checken of de deur snel genoeg open ging
            if (aantal_openingen > 10 && timedif > (gemiddelde_opening_time + 10000))
            {
                if (digitalRead(m1.pin) || digitalRead(m2.pin))
                {
                    website.send(LOG, "Deur openen duurde te lang");
                    website.send(ENABLED, "2");
                }
            }
            if (digitalRead(deurSensPin) == HIGH)
            {
                // bools juist zetten
                isOpening = false;
                open = true;
                tim1_running = false;
                opening_time_sum += timedif;
                aantal_openingen++;
                gemiddelde_opening_time = opening_time_sum / aantal_openingen;
                website.log("Deur volledig geopend");
            }
        }

        else if (open)
        {
            // all time in microSecons
            if (!tim2_running)
            {
                website.log("Timer 2 gestart");
                gettimeofday(&tim2, NULL);
                tim2_running = true;
                lcd.clear();
                lcd.print("Deur sluit in 15 seconden");
                delay(1000);
            }

            gettimeofday(&tv_now, NULL);
            timevalue1 = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
            timevalue2 = (int64_t)tim2.tv_sec * 1000000L + (int64_t)tim2.tv_usec;
            timedif = (timevalue1 - timevalue2) / 1000;

            if ((!digitalRead(m1.pin) || !digitalRead(m2.pin)) && timedif > 2500) // als een van de motion sensoren iets opvangt en na 5 sec
            {
                gettimeofday(&tim2, NULL);
                website.log("Nieuwe persoon gescand");
                lcd.clear();
                lcd.print("Deur sluit in 15 seconden");
                delay(1000);
                digitalWrite(deurPin, HIGH);
                isClosing = false;
            }

            if (timedif > 15000 && !isClosing)
            {
                website.send(LOG, "closing door");
                lcd.clear();
                lcd.print("Deur start met  sluiten");
                delay(1000);
                digitalWrite(deurPin, LOW);
                isClosing = true;
            }

            if (timedif > (15000 + gemiddelde_opening_time + 10000))
            {
                if (digitalRead(deurSensPin) == HIGH)
                {
                    website.send(LOG, "Fatale error bij deur sluiten");
                    website.send(NAME, "Door malfunction");
                    website.send(ENABLED, "2");
                    lcd.clear();
                    lcd.print("Error bij deur  sluiten");
                    website.log("stuck in while");
                    delay(1000);
                    while (true)
                    {
                    }
                }
                else
                {
                    isAllowedEntry = false;
                    open = false;
                    isClosing = false;
                    tim2_running = false;
                    website.send(LOG, "Deur gesloten");
                }
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