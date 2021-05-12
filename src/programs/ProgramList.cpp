#include "programs/ProgramList.h"



ProgramList::ProgramList() :
    Program("Program List", "Displays a list of programs"),
    programs(List<UIListItem>()) {
        Serial.println("ProgramList instantiated");
    
        programs.add(new OwOProgram());
        programs.add(new OwOProgram());
        programs.add(new OwOProgram());
        programs.add(new OwOProgram());
        programs.add(new OwOProgram());
        programs.add(new OwOProgram());
        
        programlist = new UIList(&programs, false);
    }


ProgramList::~ProgramList() {
    programs.clear(true);
    delete programlist;
}



void ProgramList::loop() {
    Serial.println(F("Starting program list..."));
    int launch = programlist->show(Displays::getPrimaryDisplay());


    if(launch != -1) {
        Program *program = static_cast<Program*>(programs.get(launch));

        program->start();
    } else {
        running = false;
    }
}