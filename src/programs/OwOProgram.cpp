#include "programs/OwOProgram.h"




OwOProgram::OwOProgram() :
    Program(F("OwO The Game"), F("UwU")) {}



void OwOProgram::loop() {
    LcdDisplay *lcd = Displays::getPrimaryDisplay();
    LcdDisplay *lcd2 = Displays::getSecondaryDisplay();

    while(!Input::isActionJustPressed("back")) {
        lcd2->print(F("OwO"));
        lcd->print(F("OwO"));
        delay(1);
        lcd2->print(F("UwU"));
        lcd->print(F("UwU"));
        delay(50);
    }  
    running = false;
}

