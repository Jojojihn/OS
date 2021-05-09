#ifndef RGBLed_h
#define RGBLed_h

#include <Arduino.h>

class RGBLed {
    private:
        ///The pins this RGB led is using. [0: Red; 1: Green; 2: Blue]
        byte pinsRGB[3]; 

        ///The last set color. Used when turning the led back on.
        byte lastColor[3];

        ///Applies the RGB values to the LED
        void apply(byte r, byte g, byte b);

    public:
        RGBLed(byte pinR, byte pinB, byte pinG);

        /**
         * Sets the color of the RGB led and turns it on if it is off.
         *
         * @param r Red component [0-255]
         * @param g Green component [0-255]
         * @param b Blue component [0-255]
         */
        void setColor(byte r, byte g, byte b);

        ///Turns off the RGB led.
        void off();

        ///Turns on the RGB led with the last used color.
        void on();
};



#endif