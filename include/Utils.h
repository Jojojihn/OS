#ifndef Utils_h
#define Utils_h

#include <Arduino.h>

class Color {
    public:
        byte red;
        byte green;
        byte blue;
    
    public:
        Color(byte r, byte g, byte b);
        
};


struct Vector2UI {
    unsigned int x;
    unsigned int y;
    
    Vector2UI(unsigned int x, unsigned int y);
    Vector2UI();
};



#endif