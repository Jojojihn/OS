#include "Input.h"

InputDevice::~InputDevice() {}



KeypadInputDevice::KeypadInputDevice(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols) : keypad(Keypad(userKeymap, row, col, numRows, numCols)) {}

char KeypadInputDevice::getKey() {
    return keypad.getKey();
}

Keypad *KeypadInputDevice::getKeypad() {
    return &keypad;
}




IRrecvInputDevice::IRrecvInputDevice(unsigned int recvPin) : irReceiver(IRrecv(recvPin)), mappings(List<CodeMap>()) {}

void IRrecvInputDevice::addMap(unsigned long int code, char key) {
    CodeMap *newMap = new CodeMap {code, key};
    for(unsigned int i = 0; i < mappings.size(); i++) {
        CodeMap *curMap = mappings.get(i);
        if(curMap->code == code) {
            curMap->character = key;
            return;
        }
    }
    mappings.add(newMap);
}

char IRrecvInputDevice::getKey() {
    decode_results results;

    if(irReceiver.decode(&results)) {
        for(unsigned int i = 0; i < mappings.size(); i++) {
            CodeMap *cMap = mappings.get(i);

            if(cMap->code == results.value) {
                return cMap->character;
     
            }
        }

        return '\0';

    }

    return '\0';
}

IRrecv *IRrecvInputDevice::getIRrecv() {
    return &irReceiver;
}

Action::Action(String tag) : tag(tag), mappings(List<DeviceKeyMap>()) {}

Action *Action::addMapping(InputDevice *device, char key) {
    Input::addInputDevice(device);

    for(unsigned int i = 0; i < mappings.size(); i++) {
        DeviceKeyMap *curMap = mappings.get(i);
        if(curMap->device == device && curMap->key == key) {
            return this;
        }
    }
    DeviceKeyMap *newMap = new DeviceKeyMap {device, key};
    mappings.add(newMap);
    return this;
}

void Action::removeMapping(InputDevice *device, char key) {
    for(unsigned int i = 0; i < mappings.size(); i++) {
        DeviceKeyMap *curMap = mappings.get(i);
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


bool Input::isActionJustPressed(String tag) {
    Action *act = getAction(tag);
    if(act != nullptr) {
        for(unsigned int i = 0; i < act->mappings.size(); i++) {
            Action::DeviceKeyMap *curMap = act->mappings.get(i);
            if(curMap->device->getKey() == curMap->key) {
                return true;
            }
        }
    } 

    return false;
}

void Input::addInputDevice(InputDevice* device) {
    if(!inputDevices.contains(device)) {
        inputDevices.add(device);
    }
}

void Input::removeInputDevice(InputDevice* device, bool deleteObject) {
    for(unsigned int i = 0; i < actions.size(); i++) {
        Action *curAct = actions.get(i);
        curAct->removeMappingsForDevice(device);
    }
    inputDevices.remove(device, deleteObject);
}

Action *Input::getAction(String tag) {
    for(unsigned int i = 0; i < actions.size(); i++) {
        Action *curAct = actions.get(i);
        if(curAct->tag == tag) {
            return curAct;
        }
    }
    
    return nullptr;
}

Action *Input::addAction(String tag) {
    for(unsigned int i = 0; i < actions.size(); i++) {
        Action *curAct = actions.get(i);
        if(curAct->tag == tag) {
            return curAct;
        }
    }
    Action *newAct = new Action(tag);
    actions.add(newAct);
    return newAct;
}

void Input::removeAction(String tag) {
    for(unsigned int i = 0; i < actions.size(); i++) {
        Action *curAct = actions.get(i);
        if(curAct->tag == tag) {
            curAct->clearMappings();
            actions.remove(curAct, true);
            return;
        }
    }
}



List<Action> Input::actions = List<Action>(); 
List<InputDevice> Input::inputDevices = List<InputDevice>(); 



