#ifndef Program_h
#define Program_h

#include <Arduino.h>

#include "UIListItem.h"

class Program : public UIListItem {
    public:
        ///The name of the program
        String name;

        //The description of the program
        String description;

        //Whether the program is running
        bool running;


        //Start this program
        void start();

    
        Program(String name, String description);

    protected:
        ///Loops while running = true. Set running to false to stop the loop.
        virtual void loop() = 0;


};


#endif