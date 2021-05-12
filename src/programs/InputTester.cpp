#include "programs/InputTester.h"



InputTester::InputTester() :
    Program("Testing", "YEEEET") {};
    




void InputTester::loop() {
    while(!Input::isActionJustPressed("back"));
    
}