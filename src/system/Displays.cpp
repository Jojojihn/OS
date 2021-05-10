#include "system/Displays.h"





Vector2 LcdDisplay::getSize() {
    return size;
}

LcdDisplay::LcdDisplay(int sizeX, int sizeY) : size(Vector2(sizeX, sizeY)) {}

LcdDisplay::~LcdDisplay() {}




LiquidCrystalDisplay::LiquidCrystalDisplay(uint8_t rs, uint8_t rw, uint8_t enable,
			     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
			     uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) : 

                 LcdDisplay(1,16),
                 lcd(LiquidCrystal(rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7)) {}

LiquidCrystalDisplay::LiquidCrystalDisplay(uint8_t rs, uint8_t enable,
			     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
			     uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) :
                 
                 LcdDisplay(1,16),
                 lcd(LiquidCrystal(rs, enable, d0, d1, d2, d3, d4, d5, d6, d7)) {}

LiquidCrystalDisplay::LiquidCrystalDisplay(uint8_t rs, uint8_t rw, uint8_t enable,
			     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3) :
                 
                 LcdDisplay(1,16),
                 lcd(LiquidCrystal(rs, rw, enable, d0, d1, d2, d3)) {}

LiquidCrystalDisplay::LiquidCrystalDisplay(uint8_t rs,  uint8_t enable,
			     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3) :

                 LcdDisplay(1,16),
                 lcd(LiquidCrystal(rs, enable, d0, d1, d2, d3)) {}


LiquidCrystal *LiquidCrystalDisplay::getLcd() {
    return &lcd;
}

void LiquidCrystalDisplay::begin(uint8_t cols, uint8_t rows, uint8_t charsize) {
    size = Vector2(cols, rows);
    lcd.begin(cols, rows, charsize);
    lcd.clear();
}

void LiquidCrystalDisplay::createChar(uint8_t location, uint8_t charmap[]) {
    lcd.createChar(location, charmap);
}

inline size_t LiquidCrystalDisplay::write(uint8_t value) {
  return lcd.write(value); 
}

void LiquidCrystalDisplay::setCursor(uint8_t x, uint8_t y) {
    lcd.setCursor(x, y);
}

void LiquidCrystalDisplay::clear() {
    lcd.clear();
}




LiquidCrystalDisplay_I2C::LiquidCrystalDisplay_I2C(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows) :
    LcdDisplay(lcd_cols, lcd_rows),
    lcd(LiquidCrystal_I2C(lcd_Addr, lcd_cols, lcd_rows)) {}


LiquidCrystal_I2C *LiquidCrystalDisplay_I2C::getLcd() {
    return &lcd;
}

void LiquidCrystalDisplay_I2C::init() {
    lcd.init();
    lcd.clear();
}

void LiquidCrystalDisplay_I2C::setBacklightEnabled(bool enabled) {
    enabled ? lcd.backlight() : lcd.noBacklight();
}

void LiquidCrystalDisplay_I2C::createChar(uint8_t location, uint8_t charmap[]) {
    lcd.createChar(location, charmap);
}

inline size_t LiquidCrystalDisplay_I2C::write(uint8_t value) {
  return lcd.write(value); 
}

void LiquidCrystalDisplay_I2C::setCursor(uint8_t x, uint8_t y) {
    lcd.setCursor(x, y);
}

void LiquidCrystalDisplay_I2C::clear() {
    lcd.clear();
}






LcdDisplay *Displays::getPrimaryDisplay() {
    return primaryDisplay;
}

void Displays::setPrimaryDisplay(LcdDisplay *display) {
    addDisplay(display);
    primaryDisplay = display;
}

LcdDisplay *Displays::getSecondaryDisplay() {
    return secondaryDisplay;
}

void Displays::setSecondaryDisplay(LcdDisplay *display) {
    addDisplay(display);
    primaryDisplay = display;
}

void Displays::clearAllDisplays() {
    for(unsigned int i = 0; i < displays.size(); i++) {
        displays.get(i)->clear();
    }
}

void Displays::addDisplay(LcdDisplay *display) {
    if(!displays.contains(display)) {
        displays.add(display);
    }
}

LcdDisplay *Displays::primaryDisplay = nullptr;
LcdDisplay *Displays::secondaryDisplay = nullptr;
List<LcdDisplay> Displays::displays = List<LcdDisplay>();