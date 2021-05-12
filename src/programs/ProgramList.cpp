#include "programs/ProgramList.h"



ProgramList::ProgramList() :
    Program("Program List", "Displays a list of programs"),
    programs(List<UIListItem>()) {

        programs.add(new OwOProgram());


    }


ProgramList::~ProgramList() {
    programs.clear(true);
}



void ProgramList::loop() {
    int launch = UIList(&programs, false).show(Displays::getPrimaryDisplay());


    if(launch != -1) {
        Program *program = static_cast<Program*>(programs.get(launch));

        program->start();
    } else {
        running = false;
    }
}