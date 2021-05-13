#include "programs/OwOProgram.h"




OwOProgram::OwOProgram() :
    Program("OwO The Game", "UwU") {}



void OwOProgram::loop() {
    LcdDisplay *lcd = Displays::getPrimaryDisplay();
    LcdDisplay *lcd2 = Displays::getSecondaryDisplay();

    do {
        lcd2->print("OwO");
        lcd->print("OwO");
        delay(1);
        lcd2->print("UwU");
        lcd->print("UwU");
        delay(1);

        Input::poll();
    } while(!Input::isActionJustPressed("back"));
    running = false;
}

