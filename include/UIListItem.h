#ifndef ListItem_h
#define ListItem_h

#include <Arduino.h>

/**
 * An Item to be displayed in a list
 */
class UIListItem {
    public: 
        ///The String this item will show up as in the list.
        String listTitle;

    protected:
        ///Called when this item becomes highlighted in the list.
        virtual void listOnSelect() = 0;
};


#endif