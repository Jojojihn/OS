#include "Input.h"
#define logD(a) (Serial.println(a))

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

char IRrecvInputDevice::_getKey() {
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

Action::Action(char* tag) : tag(tag), mappings(List<DeviceKeyMap>()) {}

Action::~Action() {
    delete[] tag;

    clearMappings();
}

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


bool Input::isActionJustPressed(const char* tag) {
    Serial.println(F("Check if action is pressed. Getting action:"));
    Action *act = getAction(tag);
    Serial.print(F("Got action. Action is: "));
    if(act != nullptr) {
        Serial.println("Not null.");

        for(unsigned int i = 0; i < inputDevices.size(); i++) {
            inputDevices.get(i)->getKey();
        }   

        for(unsigned int i = 0; i < act->mappings.size(); i++) {
            Action::DeviceKeyMap *curMap = act->mappings.get(i);
            Serial.println(F("Checking mappings. Current map:"));
            Serial.print(F("Key:"));
            Serial.println(curMap->key);
            
            char key = curMap->device->getLastKey();
            Serial.print(F("Key from mapping:"));
            Serial.println(key);

            if(key == curMap->key) {
                Serial.print(F("Device and Key match! Return true."));
                Serial.println(key);
               

                return true;
            }
        }
    } 
    Serial.println(F("No Action pressed."));
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

Action *Input::getAction(const char* tag) {
    Serial.print(F("Attempting to find action with tag: "));
    Serial.println(tag);
    for(unsigned int i = 0; i < actions.size(); i++) {
        Action *curAct = actions.get(i);
        Serial.print(F("Comparing with: "));
        Serial.println(curAct->tag);
        if(strcmp(curAct->tag, tag) == 0) {
            Serial.println(F("Match. Returning the action..."));
            return curAct;
        }
        Serial.println(F("No match. Continue..."));
    }
    
    Serial.println(F("No action with the specified tag found. Returning null..."));
    return nullptr;
}

Action *Input::addAction(const char* tag) {
    Serial.print(F("Adding action with tag: "));
    Serial.println(tag);

    for(unsigned int i = 0; i < actions.size(); i++) {
        Action *curAct = actions.get(i);
        if(strcmp(curAct->tag, tag) == 0) {
            Serial.println(F("Found existing action by the same tag, returning..."));
            return curAct;
        }
    }

    byte strLen = strlen(tag);
    char *actTag = new char[strLen + 1];
    memcpy(actTag, tag, strLen);
    actTag[strLen] = '\0';

    Action *newAct = new Action(actTag);
    actions.add(newAct);

    Serial.println(F("Action successfully created and added to action list."));
    return newAct;
}

void Input::removeAction(const char* tag) {
    for(unsigned int i = 0; i < actions.size(); i++) {
        Action *curAct = actions.get(i);
        if(strcmp(curAct->tag, tag) == 0) {
            //curAct->clearMappings(); //this is done in the destructor of Action
            actions.remove(curAct, true);
            return;
        }
    }
}



List<Action> Input::actions = List<Action>(); 
List<InputDevice> Input::inputDevices = List<InputDevice>(); 



