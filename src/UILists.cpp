#include "UILists.h"


UIListItem::UIListItem(String text) : listTitle(text), scrollIndex(0) {}
UIListItem::~UIListItem() {}
UIListItem::UIListItem() : scrollIndex(0) {}



void UIListItem::listOnSelect() {}

void UIListItem::scroll(unsigned int amount) {
    scrollIndex += amount;
    scrollIndex %= listTitle.length();
}



UIList::ListPos::ListPos() : index(0), selection(0) {}


UIList::~UIList() {
    if(destroyList) {
        items->clear(true);
        delete items;
    }
}

UIList::UIList(List<UIListItem> *items, bool displayScrollbar) :
    destroyList(false),
    items(items),
    displayScrollbar(displayScrollbar),
    shown(nullptr),
    state(ListPos()) {}


UIList::UIList(String items[], unsigned int size,  bool displayScrollbar) :
    destroyList(true), 
    displayScrollbar(displayScrollbar),
    shown(nullptr),
    state(ListPos())
    {
        this->items = new List<UIListItem>();

        for(unsigned int i = 0; i < size; i++) {
            this->items->add(new UIListItem(items[i]));
        }
}


void UIList::scrollbar(LcdDisplay *display, unsigned int totalCount, unsigned int visibleCount, unsigned int position) {
    if(totalCount > 0) {
        //Eight bytes in every char row, one space between every character, first and last byte are scroll bar borders
        const unsigned int byteRows = display->getSize().y * 8 + (display->getSize().y -1) - 2; //33 on 4 height display


        const float handleSizePercent = float(visibleCount)/float(totalCount);
        const int handleSizeBytes = max(1, round(byteRows * handleSizePercent));

        const int maxPosition = totalCount - visibleCount;

        
        const float positionPercent = (maxPosition == 0) ? 0 : (float(position) / maxPosition);

        const unsigned int handlePositionByte = round((byteRows - handleSizeBytes) * positionPercent);


        //+2 because byteRows doesn't include the borders at the top and bottom
        byte scrollBarBytes[byteRows + 2];
        scrollBarBytes[0] = B11100;
        scrollBarBytes[byteRows + 2 - 1] = B11100;

        for(unsigned int curByte = 1; curByte < byteRows + 2 - 1; curByte++) {
            unsigned int barPos = (curByte - 1);
            //If the current byte is covered by the scroll bar
            if(barPos >= handlePositionByte && barPos < handlePositionByte + handleSizeBytes) {
                scrollBarBytes[curByte] = B11100;
            } else {
                scrollBarBytes[curByte] = B10100;
            }


        }

        //All different custom characters that are needed for the scrollbar, theoretical maximum of 6:
        //2 for the borders top and bottom
        //1 For an empty section
        //1 For a fully filled section
        //2 For the bottom or the top part of the scroll handle
        byte createdCharacters[display->getSize().y][8];
        int lastCreatedCharacterIndex = -1;


        unsigned int posInBarBytes = 0;
        for(int barChar = 0; barChar < display->getSize().y; barChar++) {
            byte curBarChar[8];

            for(unsigned int i = 0; i < 8; i++) {
                curBarChar[i] = scrollBarBytes[posInBarBytes];
                posInBarBytes++;
            }
            posInBarBytes++;

            //Check if character already created;
            int exists = -1;
            for(int chars = 0; chars < lastCreatedCharacterIndex + 1; chars++) {

                for(int i = 0; i < 8; i++) {
                    if(createdCharacters[chars][i] != curBarChar[i]) {
                        break;
                    }
                    //If we get to the end, the character matched
                    if(i == 7) {
                        exists = chars;
                    }
                }
                if(exists != -1) {
                    break;
                }
            }

            //If it existed, add the index in created chars to the row map

            if(exists != -1) {
                display->setCursor(0, barChar);
                display->write(byte(exists));
                
            } else { //Otherwise create the character, then add the index
                display->createChar(lastCreatedCharacterIndex + 1, curBarChar);
                lastCreatedCharacterIndex++;
                for(unsigned int createChar = 0; createChar < 8; createChar++) {
                    createdCharacters[lastCreatedCharacterIndex][createChar] = curBarChar[createChar];
                }

                display->setCursor(0, barChar);
                display->write(byte(lastCreatedCharacterIndex));
            }
   
        }
    }
}

//Draws a single line respecting it's scroll position and margins
//Both start and endIndex are inclusive
void UIList::drawLine(LcdDisplay *display, UIListItem *item, int row, int startIndex, int endIndex) {

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
            byte ellipsis[8] = {0,0,0,0,0,0,0,0};
            ellipsis[6] = B10101;
            display->createChar(6, ellipsis);
        }
        display->setCursor(startIndex, row);

        display->print(displayedText);
        if(oversized) {
            display->write(6);
        } else {
            unsigned int emptyCols = endIndex - (startIndex + displayedText.length() -1);
            
            for(unsigned int col = 0; col < emptyCols; col++) {
                display->write(' ');
            }
            
        }



    }

}

//Draws all the lines again according to the display state and renders the scrollbar
void UIList::redraw(LcdDisplay *display) {

    items->startIteration(state.index);
    for(int row = 0; row < min(items->size(), display->getSize().y); row++) {

        UIListItem *item = items->iterate();      
        drawLine(display, item, row, (displayScrollbar) ? 1 : 0, display->getSize().x -1 - 2);

    }
    if(displayScrollbar) {
        scrollbar(display, items->size(), display->getSize().y, state.index);
    }
}


//Move the selector and redraw the entries if required
void UIList::select(LcdDisplay *display, int selIndex, bool force) {
    selIndex = min(items->size() - 1, selIndex);
    selIndex = max(0, selIndex);

    //If the new index isn't the same as before, we need to update something
    if(selIndex != state.selection || force) {
        int prevSelIndex = state.selection;
        state.selection = selIndex;

        //The row on the display that the new selection will be on
        int selRow = max(0, min(display->getSize().y - 1, selIndex - state.index));

        //The index of the slected item minus the row it is on should result in the index of the
        //  first item visible on the list. If that's not the case, we need to redraw the list
        if((selIndex - selRow) != state.index) {
            state.index = selIndex - selRow;
            redraw(display);

        } else {
            UIListItem *item = items->get(prevSelIndex);

            if(item->scrollIndex != 0) { //If the list hasn't changed, but the previous selection has scrolled text, reset the scroll on that line and redraw it
                item->scrollIndex = 0;
                drawLine(display, item, prevSelIndex - state.index, 1, display->getSize().x -1 - 2);
                
            }

        }
            
        for(int row = 0; row < display->getSize().y; row++) {

            display->setCursor(display->getSize().x - 1, row);
            (row == selRow) ? display->print("<") : display->print(" "); 

        }
            
        items->get(selIndex)->listOnSelect();
    }
}


int UIList::show(LcdDisplay *display) {
    display->clear();


    Serial.println(F("Showing list"));
    shown = display;
    redraw(display);
    select(display, state.selection, true);

    Serial.println("List drawn");
    do { 
        Input::poll();

        if(Input::isActionJustPressed("down")) {
            select(display, state.selection + 1);

        } else if(Input::isActionJustPressed("up")) {
            select(display, state.selection - 1);

        } else if(Input::isActionJustPressed("confirm")) {
            return state.selection;

        } else if(Input::isActionJustPressed("shutdown")) {
            Serial.print(F("Index: "));
            Serial.println(state.index);
            Serial.print(F("Selection: "));
            Serial.println(state.selection);
        }

    } while (!Input::isActionJustPressed("back"));
    
    shown = nullptr;
    return -1;
}

void UIList::setScrollBarEnabled(bool enabled) {
    if(enabled && !displayScrollbar) {
        if(shown != nullptr) {
            displayScrollbar = enabled;
            redraw(shown);
        }
        return;

    } else if(!enabled && displayScrollbar) {
        if(shown != nullptr) {
            for(int i = 0; i < shown->getSize().y; i++) {
                shown->setCursor(0, i);
                shown->write(' ');
            }
        }
    }

    displayScrollbar = enabled;
}