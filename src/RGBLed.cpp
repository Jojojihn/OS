#include "RGBLed.h"

RGBLed::RGBLed(byte r, byte g, byte b) {
    pinMode(r, OUTPUT);
    pinMode(g, OUTPUT);
    pinMode(b, OUTPUT);

    pinsRGB[0] = r;
    pinsRGB[1] = g;
    pinsRGB[2] = b;

    lastColor[0] = 0;
    lastColor[1] = 0;
    lastColor[2] = 0;
}

void RGBLed::apply(byte r, byte g, byte b) {
    analogWrite(pinsRGB[0], r);
    analogWrite(pinsRGB[1], g);
    analogWrite(pinsRGB[2], b);
}


void RGBLed::setColor(byte r, byte g, byte b) {
    apply(r,g,b);

    lastColor[0] = r;
    lastColor[1] = g;
    lastColor[2] = b;
}

void RGBLed::off() {
    apply(0,0,0);
}

void RGBLed::on() {
    apply(lastColor[0], lastColor[1], lastColor[2]);
}

