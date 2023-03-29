#ifndef lcd_H
#define lcd_H

#include <stdint.h>
#include <initializer_list>
#include <iostream>
#include <array>
//https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
class lcd{
    private:
        uint8_t dataPinList[8] = {0};    
        uint8_t EPin;
        uint8_t RS;
        uint8_t RW;
    public:
        lcd(uint8_t *dataPins, uint8_t E, uint8_t RS, uint8_t RW){
            for (int i = 0; i < 8; i++){
                //pinmode
                dataPinList[i] = dataPins[i];
            }
            EPin = E;
            this->RS = RS;
            this->RW = RW;
            functionSet();
        }

    private:
        void send(uint8_t byte){
            //writepin
        }
        void setupWrite(){
            //set E high
            //set RW low
        }
        void setupRead(){
            //set all data pins to read
            
        }
        void confirm(){
            //set everything low
        }
        //add function set, anders werkt niet
        void functionSet(bool dataLength8 = true, bool twoLine = true, bool font5x10 = false){
            setupWrite();
            send(0b0010'0000 | (dataLength8 << 4) | (twoLine << 3) | (font5x10 << 2));
            confirm();
        }
        void returnHome(){
            setupWrite();
            send(0b0000'0010);
            confirm();
        }
        void entryModeSet(bool increment = true,bool shift = false){
            setupWrite();
            send(0b0000'0100 | (increment << 1) | shift);
            confirm();
        }
        void displayControl(bool DisplayOn = true, bool CursorOn = true, bool CursorBlink = false){
            setupWrite();
            send(0b0000'1000 | (DisplayOn << 2) | (CursorOn << 1) | CursorBlink);
            confirm();
        }
        void write(uint8_t byte){
            setupWrite();
            //set rs high
            send(byte);
            //set rs low
            confirm();
        }
        void write(uint8_t * bytes, int length){
            for (int i = 0; i < length; i++){
                write(bytes[i]);
            }
        }
        void write(const std::string & message){
            for (const char & x: message){
                write(x);
            }
        }

    public:

        void clearScreen(){
            send(0b0000'0001);
            confirm();
        }
        


    friend std::ostream & operator<<(std::ostream & os, const lcd & l){
        for (int i = 0; i < 8; i++){
            os << int(l.dataPinList[i]) << " ";
        }
        os << "\n" << int(l.EPin) << " " << int(l.RS) << " " << int(l.RW);
        return os;    
    }


};

int main(){
    uint8_t t[] = {1,2,3,4,5,6,7,8};
    lcd x(t,14,15,16);
    std::cout << x;
}


#endif