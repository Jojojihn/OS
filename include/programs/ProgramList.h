#ifndef ProgramList_h
#define ProgramList_h

#include <Arduino.h>

#include "programs/Program.h"
#include "List.h"
#include "UILists.h"
#include "programs/OwOProgram.h"
#include "system/Displays.h"


class ProgramList : public Program {
    public:
        ProgramList();
        virtual ~ProgramList();

    protected:
        List<UIListItem> programs;

        void loop() override;
};




#endif