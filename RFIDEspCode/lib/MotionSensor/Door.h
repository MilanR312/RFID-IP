#include "MotionSensor.h"

class Door{
    bool open = false;
    MotionSensor m1;
    MotionSensor m2;

    int deurSensPin;
    int deurPin;
    struct timeval door_last_opened;
    void (*callback)(void);

    int prev;
    public:
    Door(int pin1, int pin2, int deurSens, int deur)
    : m1(pin1), m2(pin2), deurPin(deur), deurSensPin(deurSens)
    {
        pinMode(deurSens, INPUT);
        pinMode(deur, OUTPUT);
    }
    void loop(){
        m1.sense();
        m2.sense();
        getDirection();
        if (digitalRead(m1.pin) || digitalRead(m2.pin)){
            Serial.println("opening door");
            open = true;
            digitalWrite(deurPin, HIGH);
            gettimeofday(&door_last_opened, NULL);
        }

        if (open){
            struct timeval tv_now;
            gettimeofday(&tv_now, NULL);
            //all time in microSecons
            int64_t time_curr = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
            int64_t time_old = (int64_t)door_last_opened.tv_sec * 1000000L + (int64_t)door_last_opened.tv_usec;
            if (time_curr - time_old > 10000000){ //10 seconds
                Serial.println("closing door");
                digitalWrite(deurPin, LOW);
                open = false;
            }
        }
        
    }
    /**
     * @brief function that gets triggered when something goes trough the door
    */
    void registerCallback(void (*callback)(void)){
        this->callback = callback;
    }
    int getDirection(){
        int out = 0;
        if (m1.getTime() > m2.getTime()) out = 1;
        if (m2.getTime() > m1.getTime()) out =  -1;
        
        //max 1 second to get trough door
        if (prev != out && open){
            callback();
        }
        prev = out;
        return out;
    }
};