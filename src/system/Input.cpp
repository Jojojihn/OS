#include "system/Input.h"
#define logD(a) (//

InputDevice::InputDevice() : lastPolled('\0') {}
InputDevice::~InputDevice() {}

char InputDevice::getKey() {
    lastPolled = _getKey();
    return lastPolled;
}

char InputDevice::getLastKey() {
    return lastPolled;
}



KeypadInputDevice::KeypadInputDevice(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols) : keypad(Keypad(userKeymap, row, col, numRows, numCols)) {}

char KeypadInputDevice::_getKey() {
    return keypad.getKey();
}

Keypad *KeypadInputDevice::getKeypad() {
    return &keypad;
}




IRrecvInputDevice::IRrecvInputDevice(unsigned int recvPin) : irReceiver(IRrecv(recvPin)), mappings(List<CodeMap>()) {
    irReceiver.enableIRIn();
}

void IRrecvInputDevice::addMap(unsigned long int code, char key) {
    CodeMap *newMap = new CodeMap {code, key};
    mappings.startIteration();
    for(unsigned int i = 0; i < mappings.size(); i++) {
        CodeMap *curMap = mappings.iterate();
        if(curMap->code == code) {
            curMap->character = key;
            return;
        }
    }
    mappings.add(newMap);
}

char IRrecvInputDevice::_getKey() {
    decode_results results;

    if(irReceiver.decode(&results)) {
        mappings.startIteration();
        for(unsigned int i = 0; i < mappings.size(); i++) {
            CodeMap *cMap = mappings.iterate();

            if(cMap->code == results.value) {
                irReceiver.resume();
                return cMap->character;
            }
        }
        irReceiver.resume();
        return '\0';

    }
    irReceiver.resume();
    return '\0';
}

IRrecv *IRrecvInputDevice::getIRrecv() {
    return &irReceiver;
}

Action::Action(char* tag) : tag(tag), mappings(List<DeviceKeyMap>()) {}

Action::~Action() {
    delete[] tag;

    clearMappings();
}

Action *Action::addMapping(InputDevice *device, char key) {
    Input::addInputDevice(device);

    mappings.startIteration();
    for(unsigned int i = 0; i < mappings.size(); i++) {
        DeviceKeyMap *curMap = mappings.iterate();
        if(curMap->device == device && curMap->key == key) {
            return this;
        }
    }
    DeviceKeyMap *newMap = new DeviceKeyMap {device, key};
    mappings.add(newMap);
    return this;
}

void Action::removeMapping(InputDevice *device, char key) {
    mappings.startIteration();
    for(unsigned int i = 0; i < mappings.size(); i++) {
        DeviceKeyMap *curMap = mappings.iterate();
        if(curMap->device == device && curMap->key == key) {
            mappings.remove(curMap, true);
            return;
        }
    }
}

void Action::clearMappings() {
    mappings.clear(true);
}

void Action::removeMappingsForDevice(InputDevice *device) {
    for(unsigned int i = mappings.size()-1; i >= 0; i--) {
        DeviceKeyMap *curMap = mappings.get(i);
        if(curMap->device == device) {
            mappings.remove(curMap, true);
        }
    }
}

void Input::poll() {
    inputDevices.startIteration();
    for(unsigned int i = 0; i < inputDevices.size(); i++) {
        InputDevice *device = inputDevices.iterate();

        ////

        device->getKey();
    }   
}

bool Input::isActionJustPressed(const char* tag) {
    ////
    Action *act = getAction(tag);
    
    if(act != nullptr) {
        ////
        ////

        
        ////

        act->mappings.startIteration();
        for(unsigned int i = 0; i < act->mappings.size(); i++) {
            Action::DeviceKeyMap *curMap = act->mappings.iterate();
            ////
            ////
            ////
            ////
            
            char key = curMap->device->getLastKey();
            ////
            ////

            if(key == curMap->key) {
                ////
                //
               
                return true;
            } else {
                 ////
            }
        }
    } 

    ////
    return false;
}

void Input::addInputDevice(InputDevice* device) {
    if(!inputDevices.contains(device)) {
        inputDevices.add(device);
    }
}

void Input::removeInputDevice(InputDevice* device, bool deleteObject) {
    actions.startIteration();
    for(unsigned int i = 0; i < actions.size(); i++) {
        Action *curAct = actions.iterate();
        curAct->removeMappingsForDevice(device);
    }
    inputDevices.remove(device, deleteObject);
}

Action *Input::getAction(const char* tag) {
    ////
    ////

    actions.startIteration();
    for(unsigned int i = 0; i < actions.size(); i++) {
        Action *curAct = actions.iterate();
        ////
        ////
        if(strcmp(curAct->tag, tag) == 0) {
            ////
            return curAct;
        }
        ////
    }
    
    ////
    return nullptr;
}

Action *Input::addAction(const char* tag) {
    //
    //

    actions.startIteration();
    for(unsigned int i = 0; i < actions.size(); i++) {
        Action *curAct = actions.iterate();
        if(strcmp(curAct->tag, tag) == 0) {
            //
            return curAct;
        }
    }

    byte strLen = strlen(tag);
    char *actTag = new char[strLen + 1];
    memcpy(actTag, tag, strLen);
    actTag[strLen] = '\0';

    Action *newAct = new Action(actTag);
    actions.add(newAct);

    //
    return newAct;
}

void Input::removeAction(const char* tag) {

    actions.startIteration();
    for(unsigned int i = 0; i < actions.size(); i++) {
        Action *curAct = actions.iterate();
        if(strcmp(curAct->tag, tag) == 0) {
            //curAct->clearMappings(); //this is done in the destructor of Action
            actions.remove(curAct, true);
            return;
        }
    }
}



List<Action> Input::actions = List<Action>(); 
List<InputDevice> Input::inputDevices = List<InputDevice>(); 



