#include "UILists.h"


UIListItem::UIListItem() {}
UIListItem::UIListItem(String text) : listTitle(text) {}


void UIListItem::listOnSelect() {}




UIList::~UIList() {
    if(destroyItems) {
        for(unsigned int i = 0; i < length; i++) {
            delete items[i];
        }
        delete[] items;
    }
}


UIList::UIList(UIListItem *items[], unsigned int size, bool infinite = false, bool destroyItems = true) :
    destroyItems(destroyItems),
    length(size),
    items(items),
    infinite(infinite),
    position(position) {}


UIList::UIList(String *items[], unsigned int size, bool infinite = false) :
    destroyItems(true), 
    length(size),
    infinite(infinite) 
    {
        //TODO: AAAAAAAA
        UIListItem *listItems[size];

        for(unsigned int i = 0; i < size; i++) {
            listItems[i] = new UIListItem(*items[i]);
        }

        this->items = ;

    }

