#include <Arduino.h>
class MotionSensor
{
public:

    int pin;
    int currenttime;
    bool pass = false;
    struct timeval current_time;

    MotionSensor(int pin)
    {
        this->pin = pin;
        init();
    }
    void init()
    {
        pinMode(pin, INPUT);

        sense();
    }

    void sense()
    {

        if (digitalRead(pin) == HIGH)
        {
            pass = true;
            currenttime = xx_time_get_time();
        }
    }
    int64_t xx_time_get_time()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
    }
    int getPass()
    {
        return pass;
    }
    int getTime()
    {
        return currenttime;
    }
};