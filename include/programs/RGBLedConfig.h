#ifndef RGBLedConfig_h
#define RGBLedConfig_h

#include <Arduino.h>

#include "programs/Program.h"
#include "system/Input.h"
#include "system/Displays.h"
#include "List.h"
#include "UILists.h"
#include "RGBLed.h"


class RGBLedConfig : public Program {
    public:
        RGBLedConfig();
        ~RGBLedConfig();

    protected:
        void onStart() override;
        void loop() override;

    private:
        LcdDisplay *lcd;
        LcdDisplay *lcd2;
        UIList *mainList;

        void updateSecondary(const String &text);
        void updateCursor(int selection);
        void updateValue(int selection);

};




#endif