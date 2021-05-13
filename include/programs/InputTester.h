#ifndef InputTester_h
#define InputTester_h

#include <Arduino.h>

#include "programs/Program.h"
#include "system/Input.h"
#include "system/Displays.h"
#include "programs/ProgramList.h"


class InputTester : public Program {
    public:
        InputTester();

    protected:
        void onStart() override;

        void loop() override;
};




#endif