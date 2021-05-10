#ifndef Displays_h
#define Display_h

#include <Arduino.h>

#include "List.h"
#include "Utils.h"
#include "LiquidCrystal.h"


class LcdDisplay {
    public:
        virtual ~LcdDisplay();

        /**
         * @return The size of the display in characters.
         */
        Vector2 getSize();


        /**
         * @param string Print something to the display
         */
        virtual void print(const String string) = 0;

        /**
         * Creates a character on the displays memory.
         * 
         * @param location The location in memory at which to store the character
         * 
         * @param charmap An array of 8 bytes to represent the character
         */
        virtual void createChar(uint8_t location, uint8_t charmap[]) = 0;

        /**
         * Prints a character from memory to a location. If the character in memory changes the printed character will too.
         * 
         * @param value The location in memory of the character to write to the display
         */
        virtual void write(uint8_t value) = 0;

        /**
         * Sets the cursor the the specified position
         * 
         * @param x The column to set the cursor to, starting at 0
         * 
         * @param y The row to set the cursor to, starting at 0 
         */
        virtual void setCursor(uint8_t x, uint8_t y) = 0;

        /**
         * Clears the display. Sets the cursor to 0,0
         */
        virtual void clear() = 0;

    protected:
        LcdDisplay(int sizeX, int sizeY);
        Vector2 size;

    private:
    
        LcdDisplay();
};


class LiquidCrystalDisplay : public LcdDisplay {
    public:
        LiquidCrystalDisplay(uint8_t rs, uint8_t enable,
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
            uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
        LiquidCrystalDisplay(uint8_t rs, uint8_t rw, uint8_t enable,
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
            uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
        LiquidCrystalDisplay(uint8_t rs, uint8_t rw, uint8_t enable,
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);
        LiquidCrystalDisplay(uint8_t rs, uint8_t enable,
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

  

        /**
         * @return A Pointer to the underlying lcd that this object is using.
         */
        LiquidCrystal *getLcd();

        /**
         * Begins the lcd. What that means? Ask whoever wrote that library.
         * 
         * @param cols The amount of colums the LCD has
         * 
         * @param rows The amount of rows the LCD has
         */
        void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);


        void print(const String string) override;

        void createChar(uint8_t location, uint8_t charmap[]) override;

        void write(uint8_t value) override;

        void setCursor(uint8_t x, uint8_t y) override;

        void clear() override;



    private:
        LiquidCrystalDisplay();

        LiquidCrystal lcd;

};



/**
 * Displays.
 */
class Displays {
    public:
        /**
         * @return A Pointer to The primary display
         */
        static LcdDisplay *getPrimaryDisplay();

        /**
         * Sets the primary display. If it is not in the list of displays, add it.
         * 
         * @param display Pointer to the new primary display. Make sure the object will stay initialized (initialize with "new"!)
         */
        static void setPrimaryDisplay(LcdDisplay *display);


        /**
         * @return A Pointer to The secondary display
         */
        static LcdDisplay *getSecondaryDisplay();

        /**
         * Sets the secondary display. If it is not in the list of displays, add it.
         * 
         * @param display Pointer to the new secondary display. Make sure the object will stay initialized (initialize with "new"!)
         */
        static void setSecondaryDisplay(LcdDisplay *display);


        //Todo: A way to get displays
        
        /**
         * Adds a display, if it isn't already added.
         * 
         * @param display Pointer to the display to add. Make sure the object will stay initialized (initialize with "new"!)
         */
        static void addDisplay(LcdDisplay *display);

    private:
        static LcdDisplay *primaryDisplay;

        static LcdDisplay *secondaryDisplay;

        static List<LcdDisplay> displays;
        

        Displays();

};









#endif