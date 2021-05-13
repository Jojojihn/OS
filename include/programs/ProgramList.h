#ifndef ProgramList_h
#define ProgramList_h

#include <Arduino.h>

#include "programs/Program.h"
#include "List.h"
#include "UILists.h"
#include "programs/OwOProgram.h"
#include "programs/InputTester.h"
#include "programs/RGBLedConfig.h"
#include "system/Displays.h"


class ProgramList : public Program {
    public:
        ProgramList();
        virtual ~ProgramList();

    protected:
        List<UIListItem> programs;
        UIList *programlist;
        

        void loop() override;
};




#endif