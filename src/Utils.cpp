#include "Utils.h"


Color::Color(byte r, byte g, byte b) : red(r), green(g), blue(b) {}

byte &Color::operator[](int i ) {
    i = max(0, min(2, i));
    switch(i) {
        case 0:
            return red;
        case 1:
            return green;
        case 2:
            return blue;
    }
}

Vector2UI::Vector2UI(unsigned int x, unsigned int y) : x(x), y(y) {}

Vector2UI::Vector2UI() : x(0), y(0) {}



