#include "programs/Program.h"


Program::Program(String name, String description) : name(name), description(description), running(false) {
    listTitle = name;
}

void Program::start() {
    //
    
    Displays::clearAllDisplays();
    running = true;
    while(running) {
        loop();
    }
    Displays::clearAllDisplays();
}


/**
 * Displays the program description on the secondary display.
 */
void Program::listOnSelect() {
    LcdDisplay *lcd = Displays::getSecondaryDisplay();
    
    if(lcd != nullptr) {

        String text = "";

        String lastLine = description;

        do {
            if (lastLine.length() > (unsigned int) lcd->getSize().x) {
                if (lastLine.substring(0, lcd->getSize().x).charAt(lcd->getSize().x - 1) != ' ' && lastLine.substring(0, lcd->getSize().x).charAt(lcd->getSize().x - 2) != ' ') {
                    text += lastLine.substring(0, lcd->getSize().x - 1) + "-"; // + lastLine.substring(displaySecondarySize[1]-1, lastLine.length());
                    lastLine = lastLine.substring(lcd->getSize().x - 1, lastLine.length());
                } else {
                    text += lastLine.substring(0, lcd->getSize().x - 1) + " "; // + lastLine.substring(displaySecondarySize[1]-1, lastLine.length());
                    lastLine = lastLine.substring(lcd->getSize().x - 1, lastLine.length());
                }
            }
        lastLine.trim();
        } while (lastLine.length() > (unsigned int) lcd->getSize().x);

        text += lastLine;
        if (text.length() > (unsigned int) (lcd->getSize().y *lcd->getSize().x)) {
            text = text.substring(0, lcd->getSize().y * lcd->getSize().x - 1) + "-";
        }

        lcd->clear();
        for (unsigned int i = 0; i < lcd->getSize().y; i++) {
            lcd->setCursor(0, i);
            lcd->print(text.substring(i * (lcd->getSize().x), i * (lcd->getSize().x) + 16));
        }
    }
}

