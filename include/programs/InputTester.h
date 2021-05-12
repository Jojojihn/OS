#ifndef InputTester_h
#define InputTester_h

#include <Arduino.h>

#include "programs/Program.h"
#include "system/Input.h"
#include "system/Displays.h"


class InputTester : public Program {
    public:
        InputTester();

    protected:

        void loop() override;
};




#endif