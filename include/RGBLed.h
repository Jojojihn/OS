#ifndef RGBLed_h
#define RGBLed_h

#include <Arduino.h>

#include "Utils.h"

class RGBLed {
    private:
        ///The last created instance of an RGBLed
        static RGBLed instance;

        bool state;

        ///The pins this RGB led is using. [0: Red; 1: Green; 2: Blue]
        byte pinsRGB[3]; 

        ///The last set color. Used when turning the led back on.
        byte lastColor[3];

        ///Applies the RGB values to the LED
        void apply(byte r, byte g, byte b);

        

    public:
        RGBLed(int pinR, int pinB, int pinG);

        /**
         * Sets the color of the RGB led and turns it on if it is off.
         *
         * @param r Red component [0-255]
         * @param g Green component [0-255]
         * @param b Blue component [0-255]
         */
        void setColor(byte r, byte g, byte b);

        /**
         * Sets the color of the RGB led and turns it on if it is off.
         *
         * @param index 0,1,2 - R,G,B
         * 
         * @param value The value to set this color to 
         */
        void setColor(int i, byte value);

        /**
         * Gets the last set color of the RGB led
         *
         * @return The Color
         */
        Color getColor();

        

        static RGBLed *getRGBLed();

        ///Turns off the RGB led.
        void off();

        ///Turns on the RGB led with the last used color.
        void on();
};



#endif