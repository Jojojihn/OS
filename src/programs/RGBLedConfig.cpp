#include "programs/RGBLedConfig.h"



RGBLedConfig::RGBLedConfig() :
    Program("RGB Led Configuration", "Turn on the RGB for more FPS!"),
    lcd(Displays::getPrimaryDisplay()),
    lcd2(Displays::getSecondaryDisplay()) {

    String optionList[] = {"Set RGB values", "Colors", "Cycle", "Turn off"};
    mainList = new UIList(optionList, 4, false);

 };

 RGBLedConfig::~RGBLedConfig() {
     delete mainList;
 }
    


void RGBLedConfig::onStart() {
    Displays::getSecondaryDisplay()->print("RGB LED Config");
}

void RGBLedConfig::updateSecondary(const String &text) {
    lcd2->setCursor(0, 1);
    lcd2->print(text);
    for(int i = text.length() - 1; i < lcd2->getSize().x; i++) {
        //Not sure if this works
        lcd2->write(' ');
    };
}

void RGBLedConfig::updateValue(int rgb) {
  //rgbIndicator(rgbValues[0], rgbValues[1], rgbValues[2]);
  RGBLed *led = RGBLed::getRGBLed();
  lcd->setCursor(lcd->getSize().x - 5, rgb);
  lcd->print("   ");
  lcd->setCursor(lcd->getSize().x - String(led->getColor()[rgb]).length() - 2, rgb);
  lcd->print(String(led->getColor()[rgb]));

}

void RGBLedConfig::updateCursor(int selection) {
    for (int i = 0; i < 4; i++) {
        //Serial.println("For loop run: " + String(i) ", selection is" + String(selection));
        if (i != selection) {
            //Serial.println("Wasn't equal to selection, set the cursor at " + String(displayPrimarySize[1] - 1) + ", " + String(i));
            lcd->setCursor(lcd->getSize().x - 1, i);
            lcd->print(" ");
        } else {
            //Serial.println("Was equal to the selection");
            if (i == 3) {
                i = lcd->getSize().x - 1;
            }
            lcd->setCursor(lcd->getSize().x - 1, i);
            lcd->print("<");
        }
    }
}

void RGBLedConfig::loop() {
    
    Input::poll();


    const int addStepsSz = 5;
    int addSteps[addStepsSz] = {1, 5, 10, 30, 255};
    int add = addSteps[3];
    RGBLed *led = RGBLed::getRGBLed();

    //--------------
 
    updateSecondary("-Menu");

    int option = mainList->show(lcd);
    switch(option) {
        case -1: {
            running = false;
            break;
        } case 0: {
            updateSecondary(">Set RGB Value");

            String rgb[] = {"R:", "G:", "B:"};
            lcd->clear();
            for (int i = 0; i < 3; i++) {
                lcd->setCursor(0, i);
                lcd->print(rgb[i]);
                lcd->setCursor(lcd->getSize().x - String(led->getColor()[i]).length() - 2, i);
                lcd->print(String(led->getColor()[i]));
            }
            lcd->setCursor(0, lcd->getSize().y - 1);
            lcd->print("+" + String(add));

            //char curKey = inputKey();
            int selection = 0;
            //rgbIndicator(rgbValues[0], rgbValues[1], rgbValues[2]);
            updateCursor(selection);
            do {
                Input::poll();

                if (Input::isActionJustPressed("up") && selection > 0) {
                    selection--;
                    updateCursor(selection);

                } else if (Input::isActionJustPressed("down") && selection < 3) {
                    selection++;
                    updateCursor(selection);
                    
                } else if (Input::isActionJustPressed("right") && selection < 3) {
                    if (led->getColor()[selection] == 255) {
                        led->setColor(selection, 0);
                    } else if (led->getColor()[selection] + add > 255) {
                         led->setColor(selection, 255);
                    } else {
                         led->setColor(selection, byte(led->getColor()[selection] + add));
                    }
                    updateValue(selection);

                } else if (Input::isActionJustPressed("left") && selection < 3) {
                    if (led->getColor()[selection] == 0) {
                        led->setColor(selection, 255);
                    } else if (led->getColor()[selection] - add < 0) {
                        led->setColor(selection, 0);
                    } else {
                        led->setColor(selection, byte(led->getColor()[selection] - add));
                    }
                    updateValue(selection);

                } else if (Input::isActionJustPressed("confirm")) {
                    if (selection != 3) {
                        unsigned long lastMillis = 0;
                        boolean flash = true;
                        String input = "";
                        char curKey2 = Input::getKey();
                        byte blankChar[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B111111, B11111};
                        lcd->createChar(0, blankChar);
                        char charKeys[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
                        while ((curKey2 == 0 || curKey2 == '0' || curKey2 == '1' || curKey2 == '2' || curKey2 == '3' || curKey2 == '4' || curKey2 == '5' || curKey2 == '6' || curKey2 == '7' || curKey2 == '8' || curKey2 == '9') && input.length() < 3 && input != "0") {
                            if (millis() - lastMillis >= 400) {
                                if (!flash) {
                                    lcd->setCursor(lcd->getSize().x - 5, selection);
                                    //                  for (int i = 1; i + input.length() <= 3; i++) {
                                    //                    lcd.print(" ");
                                    //                  }
                                    lcd->print("   ");
                                    lcd->setCursor(lcd->getSize().x - input.length() - 2, selection);
                                    lcd->print(input);
                                } else {
                                    lcd->setCursor(lcd->getSize().x - 5, selection);
                                    for (int i = 1; i + input.length() <= 3; i++) {
                                        lcd->write(byte(0));
                                    }
                                }
                                flash = !flash;
                                lastMillis = millis();
                            }
                            if (curKey2) {
                                for (int i = 0; i < 10; i++) {
                                    if (curKey2 == charKeys[i]) {
                                        input += String(i);
                                        lcd->setCursor(lcd->getSize().x - 5, selection);
                                        if (!flash) {
                                        lcd->write(byte(0));
                                        lcd->write(byte(0));
                                        lcd->write(byte(0));
                                        } else {
                                        lcd->print("   ");
                                        }
                                        lcd->setCursor(lcd->getSize().x - input.length() - 2, selection);
                                        lcd->print(input);
                                        //flash = false;
                                        //lastMillis -= 500;
                                        break;
                                    }
                                }
                            }
                            curKey2 = Input::getKey();
                        }
                        lcd->setCursor(lcd->getSize().x - 5, selection);
                        for (int i = 1; i + input.length() <= 3; i++) {
                            lcd->print(" ");
                        }
                        if (input.toInt() > 255) {
                            input = "255";
                        }
                        led->setColor(selection, input.toInt());
                        updateValue(selection);
                    } else {
                        for (int i = 0; i < addStepsSz; i++) {
                            if (addSteps[i] == add) {
                                if (i == addStepsSz - 1) {
                                    i = -1;
                                }
                                add = addSteps[i + 1];
                                lcd->setCursor(1, lcd->getSize().y - 1);
                                lcd->print("    ");
                                lcd->setCursor(1, lcd->getSize().y - 1);
                                lcd->print(String(add));
                                break;
                            }
                        }
                    }
                }

                //Input::poll();
            } while(!Input::isActionJustPressed("back"));

            
            //rgbLedConfiguratorSaveToEeprom();
            break;
        } case 1: {
            /*
            lcd2->setCursor(0, 1);
            lcd2->print("                ");
            lcd2->setCursor(0, 1);
            lcd2->print(">Colors");
            String colors[] = {F("Yellow"), F("Pink"), F("Cyan"), F("White"), F("Off")};
            String values[] = {"130,105,0", "160,0,180", "0,100,120", "120,105,255", "0,0,0"};
            int colorsSz = 5;
            int pick = list(colors, colorsSz, values, false, true, &lcd, displayPrimarySize, rgbLedConfiguratorColorsOnSelect);
            if (pick != -1) {
                rgbIndicator(values[pick].substring(0, values[pick].indexOf(",")).toInt(), values[pick].substring(values[pick].indexOf(",") + 1, values[pick].lastIndexOf(",")).toInt(), values[pick].substring(values[pick].lastIndexOf(",") + 1, values[pick].length()).toInt());
                rgbLedConfiguratorSaveToEeprom();
            } else {
                rgbIndicator(rgbValues[0], rgbValues[1], rgbValues[2]);
            }
            break;
            */
        } case 2: {
            led->off();
            updateSecondary(">Turned off");
        }
    }
    

    
}




/*
void rgbLedConfiguratorColorsOnSelect(String elements[], String values[], int index) {
  //String values[] = {"130,105,0", "160,0,180", "0,100,120", "120,105,255", "0,0,0"};
  rgbIndicator(values[index].substring(0, values[index].indexOf(",")).toInt(), values[index].substring(values[index].indexOf(",") + 1, values[index].lastIndexOf(",")).toInt(), values[index].substring(values[index].lastIndexOf(",") + 1, values[index].length()).toInt(), true);
}
*/

/*
void rgbLedConfiguratorSaveToEeprom() {
  for (int i = 0; i < 3; i++) {
    EEPROM.update(eepromConfig[1][1] + i, rgbValues[i]);
    //Serial.println("RGB Values Saved at address " + String(eepromConfig[1][1] + i) + ", R: " + String(rgbValues[0]) + ", G: " + String(rgbValues[1]) + ", B: " + String(rgbValues[2]));
  }
  Serial.println(F("RGB Values Saved."));
}
*/
