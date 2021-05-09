#include "OwOProgram.h"




OwOProgram::OwOProgram(LiquidCrystal* x, LiquidCrystal_I2C* y) : Program(F("OwO The Game"), F("UwU")) {
    liquidCrystal = x;
    liquidCrystal2 = y;

    i = 0;
}

void OwOProgram::listOnSelect() {
    liquidCrystal2->print(description);
}

void OwOProgram::loop() {
    while(!Input::isActionJustPressed("back")) {
        liquidCrystal2->print(F("OwO"));
        liquidCrystal->print(F("OwO"));
        delay(1);
        liquidCrystal2->print(F("UwU"));
        liquidCrystal->print(F("UwU"));
        delay(50);
    }  
    running = false;
}

