#ifndef ListItem_h
#define ListItem_h

#include <Arduino.h>

#include "List.h"
#include "system/Displays.h"

/**
 * An Item to be displayed in a list
 */
class UIListItem {
    public: 
        ///The String this item will show up as in the list.
        String listTitle;

        UIListItem(String text);

    protected:
        UIListItem();

        ///Called when this item becomes highlighted in the list.
        virtual void listOnSelect();
};


class UIList {
    public:
        ~UIList();

        /**
         * Creates a new UIList. Use "show" to show the list.
         * 
         * @param items An array of Pointers to the UIListItems to be used in this list
         * 
         * @param size The size of the item array @warning Size must never be bigger than the actual size of the items array, otherwise it will lead to undefined behaviour
         * 
         * @param infinite Whether this list is infinite (Wrapping around)
         * 
         * @param destroyItems Whether to delete the UIListItems and the array when the list is destroyed
         */
        UIList(UIListItem *items[], unsigned int size, bool infinite = false, bool destroyItems = true);

        /**
         * Creates a new UIList. Use "show" to show the list.
         * 
         * @param items An array of Strings to be used in this list
         * 
         * @param size The size of the item array @warning Size must never be bigger than the actual size of the items array, otherwise it will lead to undefined behaviour
         * 
         * @param infinite Whether this list is infinite (Wrapping around)
         */
        UIList(String *items[], unsigned int size, bool infinite = false);


        /**
         * Creates a scrollable list on the UI. The function will return once an item has been selected.
         * 
         * @param items The items that are going to be displayed in the list
         * 
         * @param infiniteScroll Wether this list will scroll infinitely, repeating the elements
         * 
         * @param display The display to display the list on
         * 
         * @param startingPos The index of the element this list should start on
         */
        static int scrolledList(UIListItem *items[], bool infiniteScroll = false, LcdDisplay *display = Displays::getPrimaryDisplay());

    private:
        ///Whether to destroy the listitems when the list is destroyed
        bool destroyItems;

        ///The amount of elements in this list
        unsigned int length;

        ///All the items in this list (Pointer to array of pointers)
        UIListItem **items;

        ///Whether this list is infinite (wraps around)
        bool infinite;
    
        ///The current position in the list (Index of the first displayed item)
        unsigned int position;

        UIList();


};


#endif