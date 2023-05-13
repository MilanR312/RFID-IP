#include "MotionSensor.h"
#include "tcpClient.hpp"

extern tcpClient website;

class Door
{
    MotionSensor m1;
    MotionSensor m2;

    int deurSensPin;
    int deurPin;
    struct timeval tv_now;
    struct timeval tim1;    // hoelang het duurt om deur te openen
    struct timeval tim2;    // hoelang de deur open is
    void (*callback)(void);

    int gemiddelde_opening_time;    // in milliseconden
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
    int tries = 0;
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
    void loop(){
        if (forcedOpen)
        {
            digitalWrite(deurPin, HIGH);
            return;
        }
        if (!isAllowedEntry)
        {
            digitalWrite(deurPin, LOW);
            lcd.print("Geen toegang");
            return;
        }
        m1.sense();
        m2.sense();

        if ((!digitalRead(m1.pin) || (!digitalRead(m2.pin) && isAllowedEntry)) && digitalRead(deurSensPin) == HIGH && !open) // wanneer iemand aant wachten is op opening vd deur
        {
            website.log("person detected at door");
            website.log("opening door");
            digitalWrite(deurPin, HIGH);
            isOpening = true;
            if (!tim1_running){
                gettimeofday(&tim1, NULL);
                tim1_running = true;
                lcd.print("De deur wordt geopend");
            }
        }

        else if (isOpening){
            // zeker zijn dat de deur blijft openen
            digitalWrite(deurPin, HIGH);
            // tijd calculations uitvoeren
            gettimeofday(&tv_now, NULL);
            timevalue1 = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
            timevalue2 = (int64_t)tim1.tv_sec * 1000000L + (int64_t)tim1.tv_usec;
            timedif = (timevalue1-timevalue2)/1000;
            // checken of de deur snel genoeg open ging
            if (aantal_openingen > 10 && timedif > (gemiddelde_opening_time + 10000)){
                if (digitalRead(m1.pin) || digitalRead(m2.pin)){
                    website.send(LOG, "Deur openen duurde minstens 10 seconden langer dan verwacht: kijk deur na");
                    website.send(ENABLED, "2");
                }
            }
            if (digitalRead(deurSensPin) == LOW){
                // bools juist zetten
                isOpening = false;
                open = true;
                tim1_running = false;
                opening_time_sum += timedif;
                aantal_openingen++;
                gemiddelde_opening_time = opening_time_sum/aantal_openingen;
                website.send(LOG, "Deur volledig geopend: start timer voor sluiten");
            }
        }

        else if (open){
            // all time in microSecons
            if (!tim2_running){
                gettimeofday(&tim2, NULL);
                tim2_running = true;
                lcd.print("De deur sluit binnen 15 seconden");
            }

            if (digitalRead(m1.pin) || digitalRead(m2.pin)){
                gettimeofday(&tim2, NULL);
                lcd.print("De deur sluit binnen 15 seconden");
            }

            gettimeofday(&tv_now, NULL);
            timevalue1 = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
            timevalue2 = (int64_t)tim2.tv_sec * 1000000L + (int64_t)tim2.tv_usec;
            timedif = (timevalue1-timevalue2)/1000;


            if (timedif > 15000){
                website.send(LOG, "closing door");
                lcd.print("Deur start met sluiten");
                digitalWrite(deurPin, LOW);
                isClosing = true;
            }

            if (timedif > (15000 + gemiddelde_opening_time + 10000)){
                if (digitalRead(deurSensPin) == LOW){
                    website.send(LOG, "Fatale error bij deur sluiten. Shutting down system");
                    website.send(NAME, "Door malfunction");
                    website.send(ENABLED, "2");
                    lcd.print("Error bij deur sluiten");
                    while (true){}
                }
                else{
                    isAllowedEntry = false;
                    open = false;
                    tim2_running = false;
                    website.send(LOG, "Deur zou nu gesloten moeten zijn");
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