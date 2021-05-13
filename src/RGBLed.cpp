#include "RGBLed.h"

RGBLed::RGBLed(int r, int g, int b) {
    if(r >= 0 && g >= 0 && b >= 0) {
        pinMode(r, OUTPUT);
        pinMode(g, OUTPUT);
        pinMode(b, OUTPUT);

        digitalWrite(r, LOW);
        digitalWrite(g, LOW);
        digitalWrite(b, LOW);

        pinsRGB[0] = r;
        pinsRGB[1] = g;
        pinsRGB[2] = b;

        lastColor[0] = 0;
        lastColor[1] = 0;
        lastColor[2] = 0;

        instance = *this;

        state = true;
    }
}

///Create dummy instance
RGBLed RGBLed::instance = RGBLed(-1,-1,-1);

void RGBLed::apply(byte r, byte g, byte b) {
    analogWrite(pinsRGB[0], r);
    analogWrite(pinsRGB[1], g);
    analogWrite(pinsRGB[2], b);
}



void RGBLed::setColor(byte r, byte g, byte b) {
    state = true;
    apply(r,g,b);

    lastColor[0] = r;
    lastColor[1] = g;
    lastColor[2] = b;
}
void RGBLed::setColor(int i, byte color) {
    i = max(0, min(2, i));
    state = true;
    lastColor[i] = color;
    apply(lastColor[0],lastColor[1],lastColor[2]);
}

Color RGBLed::getColor() {
    return Color(lastColor[0], lastColor[1], lastColor[2]);
}

RGBLed *RGBLed::getRGBLed() {
    return &instance;
}

void RGBLed::off() {
    state = false;
    apply(0,0,0);
}

void RGBLed::on() {
    state = true;
    apply(lastColor[0], lastColor[1], lastColor[2]);
}

