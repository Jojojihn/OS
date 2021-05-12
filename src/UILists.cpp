#include "UILists.h"


UIListItem::UIListItem(String text) : listTitle(text), scrollIndex(0) {}
UIListItem::~UIListItem() {}
UIListItem::UIListItem() : scrollIndex(0) {}



void UIListItem::listOnSelect() {}



UIList::ListPos::ListPos() : index(0), selection(0) {}


UIList::~UIList() {
    if(destroyList) {
        items->clear(true);
        delete items;
    }
}

UIList::UIList(List<UIListItem> *items, unsigned int size, bool infinite) :
    destroyList(false),
    length(size),
    items(items),
    infinite(infinite),
    state(ListPos()) {}


UIList::UIList(String items[], unsigned int size, bool infinite) :
    destroyList(true), 
    length(size),
    infinite(infinite),
    state(ListPos())
    {
        this->items = new List<UIListItem>();

        for(unsigned int i = 0; i < size; i++) {
            this->items->add(new UIListItem(items[i]));
        }
}


void UIList::scrollbar(LcdDisplay *display, unsigned int totalCount, unsigned int visibleCount, unsigned int position) {

}

//Draws a single line respecting it's scroll position and margins
//Both start and endIndex are inclusive
void UIList::drawLine(LcdDisplay *display, UIListItem *item, unsigned int row, unsigned int startIndex, unsigned int endIndex) {
    
    startIndex = min(startIndex, endIndex);

    //If there are at least two spaces for text available...
    if(endIndex - startIndex >= 2) {
    
        //Minus one because zero indexed, minus one again because one column needs to be free for the cursor
        endIndex = min(display->getSize().x-2, endIndex);

        //The amount of space available for the text
        unsigned int length = endIndex - startIndex;

        String displayedText = item->listTitle.substring(item->scrollIndex);
        
        bool oversized = displayedText.length() > length;
        if(oversized) {
            displayedText.remove(length-1);
        }
        display->setCursor(startIndex, row);

        display->print(displayedText);
        if(oversized) {
            byte ellipsis[8];
            ellipsis [7] = B10101;
            display->createChar(4, ellipsis);
            display->write(4);
        } else {
            unsigned int emptyCols = endIndex - (startIndex + displayedText.length() -1);
            
            for(unsigned int col = 0; col < emptyCols; col++) {
                display->print(' ');
            }
            
        }



    }

}

//Draws all the lines again according to the display state and renders the scrollbar
void UIList::redraw(LcdDisplay *display) {
    items->startIteration(state.index);
    for(unsigned int row = 0; row < display->getSize().y; row++) {

        UIListItem *item = items->iterate();      
        drawLine(display, item, 1, row, display->getSize().x -1 - 3);

    }
    scrollbar(display, items->size(), display->getSize().y, state.index);
}


//Move the selector and redraw the entries if required
void UIList::select(LcdDisplay *display, unsigned int selIndex, bool force) {
    selIndex = min(items->size() - 1, selIndex);

    //If the new index isn't the same as before, we need to update something
    if(selIndex != state.selection || force) {
        unsigned int prevSelIndex = state.selection;
        state.selection = selIndex;

        //The row on the display that the new selection will be on
        unsigned int selRow = min(display->getSize().y - 1, selIndex - state.index);

        //The index of the slected item minus the row it is on should result in the index of the
        //  first item visible on the list. If that's not the case, we need to redraw the list

        
        if((selIndex - selRow) != state.index) {
            state.index = selIndex - selRow;
            redraw(display);

        } else {
            UIListItem *item = items->get(prevSelIndex);

            if(item->scrollIndex != 0) { //If the list hasn't changed, but the previous selection has scrolled text, reset the scroll on that line and redraw it
                
                item->scrollIndex = 0;
                drawLine(display, item, prevSelIndex - state.index, 1, display->getSize().x -1 - 3);
                
            }

        }
            
        
        for(unsigned int row = 0; row < display->getSize().y; row++) {

            display->setCursor(display->getSize().x - 1, row);
            (row == selRow) ? display->print("<") : display->print(" "); 

        }
            
        

    }
}


int UIList::show(LcdDisplay *display) {
    display->clear();



    redraw(display);
    select(display, state.selection, true);

    
    while(!Input::isActionJustPressed("back")) {
        
        if(Input::isActionJustPressed("down")) {
            select(display, state.selection + 1);

        } else if(Input::isActionJustPressed("up")) {
            select(display, state.selection - 1);

        } else if(Input::isActionJustPressed("confírm")) {
            return state.selection;
        }
    }
    

    return -1;
}
