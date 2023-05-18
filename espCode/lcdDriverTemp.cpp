#ifndef lcd_H
#define lcd_H

#include <stdint.h>
#include <initializer_list>
#include <iostream>
#include <array>
using namespace std;
// https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
class lcd
{
private:
    uint8_t dataPinList[8] = {0};
    uint8_t EPin;
    uint8_t RS; // Reset
    uint8_t RW; // Read Write
public:
    lcd(uint8_t *dataPins, uint8_t E, uint8_t RS, uint8_t RW)
    {
        for (int i = 0; i < 8; i++)
        {
            // pinmode
            dataPinList[i] = dataPins[i];
        }
        EPin = E;
        this->RS = RS;
        this->RW = RW;
        functionSet();
        setup_pin();
    }

private:
    void setup_pin()
    {
        pinMode(13, OUTPUT);
    }

    void send(uint8_t byte)
    {
        // writepin
        for (auto part : byte)
        {
            if (part == 1)
                digitalWrite(13, HIGH);
            else
                digitalWrite(13, LOW);
        }
        returnHome();
    }

    void setupWrite()
    {
        // set E high
        this->EPin = 1; // veranderd
        // set RW low;
        this->RW = 0; // veranderd
    }

    void confirm()
    {
        // set everything low
        send(0b0000'0000); // veranderd
    }
    // add function set, anders werkt niet
    void functionSet(bool dataLength8 = true, bool twoLine = true, bool font5x10 = false)
    {
        setupWrite();
        send(0b0010'0000 | (dataLength8 << 4) | (twoLine << 3) | (font5x10 << 2));
        confirm();
    }
    void returnHome()
    {
        setupWrite();
        send(0b0000'0010);
        confirm();
    }
    void entryModeSet(bool increment = true, bool shift = false)
    {
        setupWrite();
        send(0b0000'0100 | (increment << 1) | shift);
        confirm();
    }
    void displayControl(bool DisplayOn = true, bool CursorOn = true, bool CursorBlink = false)
    {
        setupWrite();
        send(0b0000'1000 | (DisplayOn << 2) | (CursorOn << 1) | CursorBlink);
        confirm();
    }
    void write(uint8_t byte)
    {
        setupWrite();
        this->RS = 1; // set rs high, veranderd
        send(byte);
        this->RS = 0; // set rs low, veranderd
        confirm();
    }

    void write(uint8_t *bytes, int length)
    {
        for (int i = 0; i < length; i++)
        {
            write(bytes[i]);
        }
    }
    void write(const std::string &message)
    {
        displayControl(); // Display moet aangezet worden, veranderd
        entryModeSet();   // In entry mode treden en dan telkens sturen, veranderd
        for (const char &x : message)
        {
            write(x);
        }
    }

public:
    void clearDisplay()
    {
        setupWrite(); //veranderd
        send(0b0000'0001);
        confirm();
    }

    friend std::ostream &operator<<(std::ostream &os, const lcd &l)
    {
        os << "Datapin list: ";
        for (int i = 0; i < 8; i++)
        {
            os << int(l.dataPinList[i]) << " ";
        }
        os << "\n"
           << "EPin/RS/RW :" << int(l.EPin) << " " << int(l.RS) << " " << int(l.RW);
        return os;
    }
};

int main()
{
    uint8_t t[] = {1, 2, 3, 4, 5, 6, 7, 8};
    lcd x(t, 14, 15, 16);
    std::cout << x;
}

#endif