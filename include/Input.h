#ifndef Input_h
#define Input_h

#include <Arduino.h>

#include "Utils.h"
#include "IRremote.h"
#include "Keypad.h"


class InputDevice {
    public:
        virtual char getKey() = 0;

        virtual ~InputDevice();
};

class KeypadInputDevice : public InputDevice {
    private:
        ///A Pointer to the keypad used.
        Keypad keypad;

        KeypadInputDevice();
    public:
        KeypadInputDevice(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols);

        /**
         * @return The last key that was pressed, if it is still pressed
         */
        char getKey() override;

        /**
         * @returns A Pointer to the underlying Keypad object that this Input Device is using
         */
        Keypad *getKeypad();



     
};

class IRrecvInputDevice : public InputDevice {
    private:
        ///Maps a code received from the IR receiver to a character.
        struct CodeMap {
            unsigned long int code;
            char character;
        };

        ///A Pointer to the IR receiver used.
        IRrecv irReceiver;

        ///List of IR code to char mappings
        List<CodeMap> mappings;

        IRrecvInputDevice();

    public:
        /**
         * Instantiates this IR receiver Input Device.
         * 
         * @param recvPin Arduino pin to use. No sanity check is made
         */
        IRrecvInputDevice(unsigned int recvPin);

        /**
         * @return The last key that was pressed, if it has not yet been read
         */
        char getKey() override;

        /**
         * Maps an IR code to a key (character) that this device can return. If a mapping to the same code already exists, it will be overwritten.
         * 
         * @param code The code that will correspond to the key
         * 
         * @param key The key that the code will correspond to
         */
        void addMap(unsigned long int code, char key);

        //TODO: Add a way to remove mappings.
        
        /**
         * @returns A Pointer to the underlying IRrecv object that this Input Device is using
         */
        IRrecv *getIRrecv();

        
};


/**
 * An Input action. Can store a maximum of 3 different associated characters. 
 */
struct Action {
    public:
        Action(String tag);

        struct DeviceKeyMap {
            InputDevice* device;
            char key;
        };

        String tag;

        /**
         * Adds an input mapping to this Action. 
         * 
         * @param device A Pointer to the device that can trigger this mapping. If the device is not on the list of input devices, it will be added. Make sure this object will not get destroyed later (initialize with "new")
         *
         * @param key The key on the device that can trigger this mapping
         *
         * @return The Action this mapping belongs to. Can be used to inline string together mappings.
         */
        Action *addMapping(InputDevice *device, char key);

        /**
         * Remove an input mapping from this Action
         */
        void removeMapping(InputDevice *device, char key);

        /**
         * Removes all mappings associated with the specified device.
         * 
         * @param device A Pointer to the device of which the mappings will be deleted
         */
        void removeMappingsForDevice(InputDevice *device);

        /**
         * Clear all mappings in this Action
         */
        void clearMappings();

        friend class Input;

    private:
        Action();
        List<DeviceKeyMap> mappings;


};




class Input {
    public:
        /**
         * Whether any of the mappings of this action have been pressed.
         * 
         * @param tag The tag of the action to check
         * 
         * @return True when the action is pressed. If the action is not pressed or not found, false.
         */
        static bool isActionJustPressed(String tag);
        
        /**
         * Adds an input device
         * 
         * @param device A Pointer to the device to add. Make sure the object will stay initialized (initialize with "new")
         */
        static void addInputDevice(InputDevice* device);

        /**
         * Removes an input device. Also removes all associated mappings.
         * 
         * @param device A Pointer to the device to remove. 
         * 
         * @param deleteObject Whether to delete this object from memory. Removing without deleting will result in a memory leak
         *                     if you do not have another pointer to it. Deleting while still having another pointer will make that
         *                     pointer point at garbage.
         */
        static void removeInputDevice(InputDevice* device, bool deleteObject);


        /**
         * Gets the pointer to an Action.
         * 
         * @param tag The tag of the Action
         * 
         * @return The Action, or nullptr if the action wasn't found
         */
        static Action *getAction(String tag);

        /**
         * Adds an Action.
         * 
         * @param tag The tag this Action will be accessible under
         * 
         * @return The created Action, or the existing Action if it already existed. This can be used to add mappings inline.
         */
        static Action *addAction(String tag);

        /**
         * Removes the action with the specified tag.
         * 
         * @param tag The tag of the action to remove
         */
        static void removeAction(String tag);


    private:
        Input();

        
        static List<Action> actions;
        static List<InputDevice> inputDevices;


};


#endif