#ifndef ListItem_h
#define ListItem_h

#include <Arduino.h>

#include "List.h"
#include "system/Displays.h"
#include "system/Input.h"

/**
 * An Item to be displayed in a list
 */
class UIListItem {
    public: 
        UIListItem(String text);
        virtual ~UIListItem();


        ///The String this item will show up as in the list.
        String listTitle;

    protected:
        UIListItem();

        ///Called when this item becomes highlighted in the list.
        virtual void listOnSelect();

    private:
        unsigned int scrollIndex;

        friend class UIList;
};


class UIList {
    public:
        ~UIList();


        /**
         * Creates a new UIList. Use "show" to show the list.
         * 
         * @param items A Pointer to a List of UIListItems. Make sure this list doesn't go out of scope within the lifetime of this object.
         * 
         * @param size The size of the item array @warning Size must never be bigger than the actual size of the items array, otherwise it will lead to undefined behaviour
         * 
         * @param infinite Whether this list is infinite (Wrapping around)
         * 
         */
        UIList(List<UIListItem> *items, unsigned int size, bool infinite = false);

        /**
         * Creates a new UIList. Use "show" to show the list.
         * 
         * @param items An array of Strings to be used in this list
         * 
         * @param size The size of the item array @warning Size must never be bigger than the actual size of the items array, otherwise it will lead to undefined behaviour
         * 
         * @param infinite Whether this list is infinite (Wrapping around)
         */
        UIList(String items[], unsigned int size, bool infinite = false);


        /**
         * Shows the list on the specified display
         * 
         * @param display The LcdDisplay on which to show the list
         * 
         * @return The index of the object that was selected in the list
         */
        int show(LcdDisplay *display);


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
        /**
         * Draws a scrollbar in the leftmost column of the specified LcdDisplay. The scroll handle will 
         * adjust it's size and position to represent the scroll state. This will use the first four custom
         * character slots on the LCD.
         * 
         * @param display The display to show the scrollbar on. 
         * 
         * @param totalCount The total amount of things this scrollbar is representing
         * 
         * @param visibleCount The amount of visible things from the total things (used to determine scroll handle size)
         * 
         * @param position The position of the first visible thing relative to the total amount of things
         * 
         */
        void scrollbar(LcdDisplay *display, unsigned int totalCount, unsigned int visibleCount, unsigned int position);

        void drawLine(LcdDisplay *display, UIListItem *item, unsigned int row, unsigned int startIndex, unsigned int endIndex);

        void redraw(LcdDisplay *display);

        void select(LcdDisplay *display, unsigned int selIndex, bool force = false);

        struct ListPos {
            ///The index of the first item in the list
            unsigned int index;

            ///The index of the selected item in the list
            unsigned int selection;

            ListPos();
        };

        ///Whether to destroy the list when the object gets destructed
        bool destroyList;

        ///The amount of elements in this list
        unsigned int length;

        ///All the items in this list (Pointer to array of pointers)
        List<UIListItem> *items;

        ///Whether this list is infinite (wraps around)
        bool infinite;
    
        ///The state of the list (Position & Selection)
        ListPos state;

        UIList();


};


#endif