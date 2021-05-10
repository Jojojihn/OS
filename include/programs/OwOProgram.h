#ifndef OwOProgram_h
#define OwOProgram_h

#include <Arduino.h>

#include "Program.h"
#include "system/Input.h"


class OwOProgram : public Program {
    public:
        OwOProgram();

    protected:
        void loop() override;


};


#endif