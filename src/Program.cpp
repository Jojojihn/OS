#include "Program.h"


Program::Program(String name, String description) : name(name), description(description), running(false) {
    listTitle = name;
}

void Program::start() {
    running = true;
    while(running) {
        loop();
    }
    
}

