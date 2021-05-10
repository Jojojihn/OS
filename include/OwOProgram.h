#ifndef OwOProgram_h
#define OwOProgram_h

#include <Arduino.h>

#include "Program.h"
#include "Displays.h"
#include "Input.h"


class OwOProgram : public Program {
    public:
        OwOProgram();

    protected:
        void loop() override;


};


#endif