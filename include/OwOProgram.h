#ifndef OwOProgram_h
#define OwOProgram_h

#include <Arduino.h>

#include "Program.h"
#include "LiquidCrystal.h"
#include "LiquidCrystal_I2C.h"
#include "Input.h"


class OwOProgram : public Program {
    public:
        OwOProgram(LiquidCrystal*, LiquidCrystal_I2C*);

    protected:
        void loop() override;

        void listOnSelect() override;

    private:
        LiquidCrystal* liquidCrystal;
        LiquidCrystal_I2C* liquidCrystal2;

        int i;

        
};


#endif