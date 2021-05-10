//#include "pitches.h"

#include <Wire.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <IRremote.h>
#include <BigNumbers.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>


#include "system/Input.h"
#include "system/Displays.h"

#include "RGBLed.h"
#include "Utils.h"

#include "programs/OwOProgram.h"







//----------- Pin assignments ---------------
const byte COLS = 4;
const byte ROWS = 4;
byte colPins[COLS] = {28, 29, 30, 31};
byte rowPins[ROWS] = {32, 33, 34, 35};
const byte speakerPin = 8;
const byte IRPin = 46;
const byte RGBleds[] = {3, 4, 5};
//-------------------------------------------

//---------------- Values -------------------
boolean restoreData = true;

char keyUp = '2';
char keyDown = '8';
char keyLeft = '4';
char keyRight = '6';
char keyConfirm = '5';
char keyShutdown = '#';
char keyESC = '*';
char tempKeyAssign[7] = {0, 0, 0, 0, 0, 0, 0};

char keys[ROWS][COLS] = {
  {'D', '#', '0', '*'},
  {'C', '9', '8', '7'},
  {'B', '6', '5', '4'},
  {'A', '3', '2', '1'}
};


const int eepromGroupsSz = 2;
const int eepromGroups[eepromGroupsSz] {8, 3}; //How many bytes each group needs (group 1: settings, group2: rgbValues)
unsigned int eepromConfig[eepromGroupsSz][3];



const int displayPrimarySize[2] = {4, 20};
const int displaySecondarySize[2] = {2, 16};
//-------------------------------------------

//--------------- Programs ------------------
const int programCount = 12;
char buffer[120];
//To add a program, simply add another variable for BOTH name and description, and add the variable names to the two arrays
//Because these names don't change, they're saved in program memory so that less RAM will be used when the strings aren't needed
//<Classes and Object Orientation>: "Am I a joke to you?"
const char prgrm1[] PROGMEM = "Test Program";
const char prgrm2[] PROGMEM = "OwO The Game";
const char prgrm3[] PROGMEM = "Calculator";
const char prgrm4[] PROGMEM = "Google Search";
const char prgrm5[] PROGMEM = "Anime Player";
const char prgrm6[] PROGMEM = "2D Minecraft";
const char prgrm7[] PROGMEM = "Music";
const char prgrm8[] PROGMEM = "Draw";
const char prgrm9[] PROGMEM = "This is just an extremely long program name to test the sideways scrolling feature OwO.";
const char prgrm10[] PROGMEM = "RGB Led Configuration";
const char prgrm11[] PROGMEM = "Text Editor";
const char prgrm12[] PROGMEM = "Bomb Mode";

const char desc1[] PROGMEM = "A test, seems like it failed?";
const char desc2[] PROGMEM = "UwU";
const char desc3[] PROGMEM = "Quick mafs";
const char desc4[] PROGMEM = "*uses Bing";
const char desc5[] PROGMEM = "Yea no";
const char desc6[] PROGMEM = "You mean Terraria?";
const char desc7[] PROGMEM = "Play or record some epic tunes.";
const char desc8[] PROGMEM = "Paint but worse";
const char desc9[] PROGMEM = "Don't bother reading uwu";
const char desc10[] PROGMEM = "Turn on the RGB for more FPS!";
const char desc11[] PROGMEM = "Nokia phone style typing?";
const char desc12[] PROGMEM = "I swear it will explode.";
const char *const programs[] PROGMEM = {
  prgrm1, prgrm2, prgrm3, prgrm4, prgrm5, prgrm6, prgrm7, prgrm8, prgrm9, prgrm10, prgrm11, prgrm12
};
const char *const programDescs[] PROGMEM = {
  desc1, desc2, desc3, desc4, desc5, desc6, desc7, desc8, desc9, desc10, desc11, desc12
};

/*
  const char programs[programCount] = { //The names of the programs
      "Test Program", "OwO The Game", "Calculator", "Google Search", "Anime Player", "2D Minecraft", "Music", "Draw",
      "This is just an extremely long program name to test the sideways scrolling feature OwO.",
      "RGB Led Configuration", "Text Editor", "Bomb Mode"
  };
  String programDescs[programCount] = { //Descriptions of programs
      "A test, seems like it failed?", "UwU", "Quick mafs", "*uses Bing", "Yea no", "You mean Terraria?", "Play or record some epic tunes.",
      "Paint but worse", "Don't bother reading uwu", "Turn on the RGB for more FPS!", "Nokia phone style typing?", "I swear it will explode."
  };
*/
//-------------------------------------------

//------------ Runtime variables ------------
bool debug = false;
byte mainMenuSelected = 0;
boolean isRunning = false;
String opened = "Main_Menu";
boolean muted = true;
//String muted_S = "true";
byte rgbValues[] = {0, 0, 0};
byte settingSelectedOnPage = 0;
int settingPage = 0;
//byte seetingSelectedGeneral = 0; //commented because it's never used and ram ain't cheap
int programMenuListPos[] = {0, 0};
//-------------------------------------------

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
LiquidCrystalDisplay lcd(22, 23, 24, 25, 26, 27);
LiquidCrystalDisplay_I2C lcd2(0x27, 16, 2);
IRrecv irrecv(IRPin);
BigNumbers bigNum(lcd.getLcd());
RGBLed rgbLed = RGBLed(RGBleds[0], RGBleds[1], RGBleds[2]);



//--------------- *New* Input ---------------
IRrecvInputDevice *irInputDevice = new IRrecvInputDevice(IRPin);

KeypadInputDevice *keypadInputDevice = new KeypadInputDevice(makeKeymap(keys), rowPins, colPins, ROWS, COLS);



//-------------------------------------------
char inputKey() {
  decode_results results;
  char key = keypad.getKey();
  char returnChar = 0;
  if (key) {
    returnChar = key;
  } else if (irrecv.decode(&results)) {
    const int sz = 21;
    long unsigned int codes[] = {
      16738455, 16724175, 16718055, 16743045,
      16716015, 16726215, 16734885, 16728765,
      16730805, 16732845, 16769055, 16754775,
      16753245, 16736925, 16769565, 16720605,
      16712445, 16761405, 16748655, 16750695,
      16756815
    };
    char keyValues[] = {
      '0', '1', '2', '3',
      '4', '5', '6', '7',
      '8', '9', '*', '#', //8,9,-,+
      'A', 'B', 'C', 'D', //CH-, CH, CH+, PREV
      'e', 'f', 'g', 'h', //NEXT, PL/PAU, EQ, 100+
      'i'                 //200+
    };
    //Serial.println(results.value);
    for (int i = 0; i < sz; i++) {
      if (codes[i] == results.value) {
        returnChar = keyValues[i];
        break;
      }
    }
    irrecv.resume();
  }
  if (returnChar) {
    Serial.print(F("Pressed "));
    Serial.println(String(returnChar));
  }
  return returnChar;
}

void loadMainMenu() {
  byte g[8] = {B00000, B00000, B01111, B10001, B10001, B01111, B00001, B01110};
  lcd.createChar(0, g);
  lcd.setCursor(0, 1);
  lcd.print(F("Pro"));
  lcd.write(byte(0));
  lcd.print(F("rams Options Off"));
  lcd.setCursor(0, 2);
  lcd.print(F("  <0>      <0>   <0>"));
}

void mainMenu() {
  lcd.setCursor(0, 2);
  if (mainMenuSelected == 0) {
    lcd.print(F("  <0>       0     0 "));
  } else {
    if (mainMenuSelected == 1) {
      lcd.print(F("   0       <0>    0 "));
    } else {
      if (mainMenuSelected == 2) {
        lcd.print(F("   0        0    <0>"));
      }
      else {
        if (mainMenuSelected > 2) {
          mainMenuSelected = 2;
          lcd.print(F("   0        0    <0>"));
        } else {
          if (mainMenuSelected < 0) {
            mainMenuSelected = 0;
            lcd.print(F("  <0>       0     0 "));
          }
        }
      }
    }
  }
  //lcd2.setCursor(0,0);
  //lcd2.print("Temperature: ");
  //lcd2.print(DHT.temperature, 1);
  //lcd2.print("C");
  //lcd2.setCursor(0,1);
  //lcd2.print("Humidity: ");
  //lcd2.print(DHT.humidity, 1);
  //lcd2.print("%");
}

void bootup() {
  byte OLinksOben[8] = {B00000, B00011, B00111, B01110, B11100, B11000, B11000, B11000};
  byte ORechtsOben[8] = {B00000, B11000, B11100, B01110, B00111, B00011, B00011, B00011};
  byte ORechtsUnten[8] = {B00011, B00011, B00011, B00111, B01110, B11100, B11000, B00000};
  byte OUntenLinks[8] = {B11000, B11000, B11000, B11100, B01110, B00111, B00011, B00000};
  byte SObenLinks[8] = {B00000, B00000, B00111, B01110, B01100, B01100, B01100, B01110};
  byte SObenRechts[8] = {B00000, B00000, B11100, B01110, B00010, B00000, B00000, B00000};
  byte SUntenLinks[8] = {B00111, B00011, B00000, B00000, B01000, B01110, B00111, B00000};
  byte SUntenRechts[8] = {B11000, B11100, B00110, B00010, B00010, B00110, B11100, B00000};

  lcd.clear();
  if (!muted) {
    //tone(speakerPin, 1244.51, 222);
    delay(222);
    //tone(speakerPin, 622.254, 222);
    delay(222);
    //tone(speakerPin, 932.328, 333);
    delay(333);
    //tone(speakerPin, 830.609, 333);
    delay(444);
    //tone(speakerPin, 1244.51, 333);
    delay(333);
    //tone(speakerPin, 932.328, 888);
  }
  isRunning = true;
  opened = "Main_Menu";
  mainMenuSelected = 0;
  lcd.createChar(0, OLinksOben);
  lcd.createChar(1, ORechtsOben);
  lcd.createChar(2, OUntenLinks);
  lcd.createChar(3, ORechtsUnten);
  lcd.createChar(4, SObenLinks);
  lcd.createChar(5, SObenRechts);
  lcd.createChar(6, SUntenLinks);
  lcd.createChar(7, SUntenRechts);
  lcd.setCursor(8, 1);
  lcd.write(byte(0));
  lcd.write(byte(1));
  lcd.write(byte(4));
  lcd.write(byte(5));
  lcd.setCursor(8, 2);
  lcd.write(byte(2));
  lcd.write(byte(3));
  lcd.write(byte(6));
  lcd.write(byte(7));
  delay(10);
  lcd2.setBacklightEnabled(true);
  for (int i = 0; i < 20; i++) {
    lcd.setCursor(i, 0);
    lcd.print("-");
    lcd.setCursor(i, 3);
    lcd.print("-");
    lcd2.setCursor(i, 0);
    lcd2.print("-");
    lcd2.setCursor(i, 3);
    lcd2.print("-");
    delay(100);
  }
  lcd.clear();
  lcd2.clear();
  delay(1);
  loadMainMenu();
}

void shutDownSound() {

}

void programMenuDisplayDesc(String elements[], String values[], int index) {
  String text;
  strcpy_P(buffer, (PGM_P)pgm_read_word(&(programDescs[index])));
  text = buffer;
  String lastLine = text;
  text = "";
  do {
    if (lastLine.length() > (unsigned int) displaySecondarySize[1]) {
      if (lastLine.substring(0, displaySecondarySize[1]).charAt(displaySecondarySize[1] - 1) != ' ' && lastLine.substring(0, displaySecondarySize[1]).charAt(displaySecondarySize[1] - 2) != ' ') {
        text += lastLine.substring(0, displaySecondarySize[1] - 1) + "-"; // + lastLine.substring(displaySecondarySize[1]-1, lastLine.length());
        lastLine = lastLine.substring(displaySecondarySize[1] - 1, lastLine.length());
      } else {
        text += lastLine.substring(0, displaySecondarySize[1] - 1) + " "; // + lastLine.substring(displaySecondarySize[1]-1, lastLine.length());
        lastLine = lastLine.substring(displaySecondarySize[1] - 1, lastLine.length());
      }
    }
    lastLine.trim();
  } while (lastLine.length() > (unsigned int) displaySecondarySize[1]);
  text += lastLine;
  if (text.length() > (unsigned int) (displaySecondarySize[0]*displaySecondarySize[1])) {
    text = text.substring(0, displaySecondarySize[0] * displaySecondarySize[1] - 1) + "-";
  }
  lcd2.clear();
  for (int i = 0; i < displaySecondarySize[0]; i++) {
    lcd2.setCursor(0, i);
    lcd2.print(text.substring(i * (displaySecondarySize[1]), i * (displaySecondarySize[1]) + 16));
  }
  ///lcd2.print(text);
}


void rgbIndicator(byte red, byte green, byte blue, boolean skipVars) {
  rgbLed.setColor(red, green, blue);

  if (!skipVars) {
    rgbValues[0] = red;
    rgbValues[1] = green;
    rgbValues[2] = blue;
  }
  Serial.print(F("Set RGB LED to R:"));
  Serial.print(red);
  Serial.print(F(", G:"));
  Serial.print(green);
  Serial.print(F(", B:"));
  Serial.println(blue);
}

void rgbIndicator(byte red, byte green, byte blue) {
  rgbIndicator(red, green, blue, false);
}

void setupEepromConfig(boolean print) {
  int previous = -1;
  for (int i = 0; i < eepromGroupsSz; i++) {
    eepromConfig[i][0] = previous + 1;
    eepromConfig[i][1] = previous + 2;
    eepromConfig[i][2] = previous + 1 + eepromGroups[i];
    if (print) {
      Serial.print(F("EEPROM Group "));
      Serial.print(String(i));
      Serial.print(F(":\n Check Byte: "));
      Serial.print(previous + 1);
      Serial.print(F(", Byte Range from "));
      Serial.print(previous + 2);
      Serial.print(F(" to "));
      Serial.print(previous + 1 + eepromGroups[i]);
      Serial.println(F("."));
    }
    previous += 1 + eepromGroups[i];
  }

}

void restoreEepromData() {
  for (int i = 0; i < eepromGroupsSz; i++) {
    boolean checkByte = (boolean) EEPROM.read(eepromConfig[i][0]);
    //Serial.print("CheckByte on Group " + String(i) + " is ");
    //Serial.println(EEPROM.read(eepromConfig[i][0]));
    if (checkByte) {
      if (i == 0) {
        muted = (boolean) EEPROM.read(eepromConfig[0][1]);
        char * keyRefs[] = {&keyLeft, &keyRight, &keyUp, &keyDown, &keyConfirm, &keyESC, &keyShutdown}; //Ordered after the settings order
        for (int ii = 0; ii <= 6; ii++) { //This used to say <= 7, which should be wrong, so I fixed it but it is not tested
          *keyRefs[ii] = (char) EEPROM.read(eepromConfig[0][1] + 1 + ii);
        }
        Serial.println(F("Settings restored."));
      } else if (i == 1) {
        for (int ii = 0; ii < 3; ii++) {
          rgbValues[ii] = EEPROM.read(eepromConfig[1][1] + ii);
        }
        rgbIndicator(rgbValues[0], rgbValues[1], rgbValues[2]);
        Serial.println(F("RGB values restored."));
      }
    }
  }
}

//----------------------------------------------------------------------------------- \SHUTDOWN/ ------------------------------------------------------------------------------------
unsigned long lastMillis;
boolean rev = false;
boolean gotoBlock = false;

byte flipByte(byte input) { //something something bitwise
  char r = 0;
  for (byte i = 0; i < 5; i++) {
    r <<= 1;
    r |= input & 1;
    input >>= 1;
  }
  return r;
}

void drawFrame(int subFrameDelay, const int dimensions[], const int spacesCount, const int byteCount, byte byteData[][8], int arrangement[][2]) { //Process the frame data and display it on the LCD
  //lcd.clear();
  String uniqueCharacters = "";
  int uniqueCount = 0;
  for (int i = 0; i < spacesCount; i++) { //Determine a list of unique characters
    String check = String(arrangement[i][0]) + "," + String(arrangement[i][1]); //!!: Converting integers to strings and adding them together
    if (uniqueCharacters.indexOf(check) == -1) {
      uniqueCharacters += check + " "; //format: "0,1 0,2 0,3 0,1 0,2 0,3 0,1 0,2 0,3" //!!: Adding Strings together
      uniqueCount++;
    }
  }
  //Serial.println("Unique character list: " + uniqueCharacters + "    " + uniqueCount);

  for (int pos = 0; float(pos) < (float(uniqueCount) / 8); pos++) { //run as many times as needed to render the entire frame with 8 unique characters per step
    lcd.clear();
    //lcd.setCursor(0,0);
    //lcd.print("                                                                                          ");
    //Serial.println("Start of subframe");
    String lcdChars = ""; //Keeps track of the location each specific character has been saved at //format: "0,0b0 0,1b1"
    int lcdCharsPos = 0;  //Keeps track which location was last saved to on the lcd
    String allowedCharacters;  //A maximum of 8 characters that are allowed in the current subframe
    int lastIndex[2] = {0, 0};
    for (int i = 0; i < pos * 8; i++) {
      lastIndex[0] = uniqueCharacters.indexOf(" ", lastIndex[0] + 1);
    }
    lastIndex[1] = lastIndex[0];
    for (int i = 0; i < 8 && uniqueCharacters.indexOf(" ", lastIndex[1]) != -1; i++) {
      lastIndex[1] = uniqueCharacters.indexOf(" ", lastIndex[1] + 1);
    }
    allowedCharacters = uniqueCharacters.substring(lastIndex[0], lastIndex[1]);
    //Serial.println(" Allowed characters are '" + allowedCharacters + "'");
    //lcd.setCursor((displayPrimarySize[1]/2)-dimensions[1]/2, (displayPrimarySize[0]/2)-dimensions[0]/2); //Set the cursor to the upper left of the frame

    for (int i = 0; i < dimensions[0]; i++) { //for every row in frame
      for (int ii = 0; ii < dimensions[1]; ii++) { //for every character in frame

        //arrangement[i*dimensions[1]+ii] //current position in arrangement
        String curChar = String(arrangement[i * dimensions[1] + ii][0]) + "," + String(arrangement[i * dimensions[1] + ii][1]); //Format: "0,1"
        //Serial.println(" Current Character: " + curChar);
        if (allowedCharacters.indexOf(curChar) != -1) { //If the character at this position is part of the currently allowed characters
          //Serial.println("  -Allowed");
          //delay(300);
          if (lcdChars.indexOf(curChar) != -1) { //If it is part of the created chars //.indexOf(curChar, pos * 8)
            //Serial.println("   -Character already created, loading from " + lcdChars.substring(lcdChars.indexOf("b", lcdChars.indexOf(curChar)) + 1, lcdChars.indexOf("b", lcdChars.indexOf(curChar)) + 2));
            lcd.setCursor((displayPrimarySize[1] / 2) - dimensions[1] / 2 + ii, (displayPrimarySize[0] / 2) - dimensions[0] / 2 + i);
            lcd.write(byte(lcdChars.substring(lcdChars.indexOf("b", lcdChars.indexOf(curChar)) + 1, lcdChars.indexOf("b", lcdChars.indexOf(curChar)) + 2).toInt())); //!!: cursed
          } else {
            lcdChars += curChar + "b" + String(lcdCharsPos) + " ";
            int IRP = arrangement[i * dimensions[1] + ii][1];
            byte result[8];
            if (IRP == 0) { //Do nothing
              for (int b = 0; b < 8; b++) {
                result[b] = byteData[arrangement[i * dimensions[1] + ii][0]][b];
              }
            }
            if (IRP == 2) { //Flip on the horizontal axis
              for (int b = 0; b < 8; b++) {
                result[b] = byteData[arrangement[i * dimensions[1] + ii][0]][7 - b];
              }
            }
            if (IRP == 1) { //Flip on the vertical axis
              for (int b = 0; b < 8; b++) {
                result[b] = flipByte(byteData[arrangement[i * dimensions[1] + ii][0]][b]);
              }
            }
            if (IRP == 3) { //Flip both
              for (int b = 0; b < 8; b++) {
                result[b] = flipByte(byteData[arrangement[i * dimensions[1] + ii][0]][7 - b]);
              }
              //for(int b = 0; b < 8; b++) {
              //  result[b] = flipByte(result[b]);
              //}
            }
            lcd.createChar(lcdCharsPos, result); //save the character
            lcd.setCursor((displayPrimarySize[1] / 2) - dimensions[1] / 2 + ii, (displayPrimarySize[0] / 2) - dimensions[0] / 2 + i);
            lcd.write(byte(lcdCharsPos));
            if (lcdCharsPos > 7) {
              lcd.setCursor(0, 0);
              lcd.print("Sumting went wong! uwu");
            }
            lcdCharsPos++;
          }
        }
      }
      //lcd.setCursor((displayPrimarySize[1]/2)-dimensions[1]/2+ (i+1), (displayPrimarySize[0]/2)-dimensions[0]/2); //move cursor to the next row at the frame beginning
    }

    delay(subFrameDelay);
  }

}

int animDelay = 50; //The delay between frames in milliseconds
//int shutdownDelay = 3; //How long you have to hold the shutdown button to confirm shutdown in seconds //Redundant



void shutDown() {
  rev = false;
  gotoBlock = false;
  //AAP Advanced Animation Player,
  //ASR Advanced storage reduction uses IRP Image Reconstruction Program,
  //Free for commercial use, 2019
  //I'm just making crap up I wrote all this

  //IRP Reference: 0; No modification || 1; Flip V || 2; Flip H || 3; Flip Both

  //byte fullChar[8] = {B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111};
  //byte blankChar[8] = {B00000,B00000,B00000,B00000,B00000,B00000,B00000,B00000};
  //Format Dimensions: {Vsquares, Hsquares}
  //Format data: User defined
  //Format Arrangement: {{frameDataIndex, IRP Method}, ...} following the format of {v0h0,v0h1,v0h2,... v1h0,v1h1,v1h2...}


  // Frame 1 //
  const int FRAME_1_DIM[2] = {2, 2}; //The dimensions of this frame in spaces (Vertical, Horizontal)
  const int FRAME_1_SIZ = 4; //The amount of spaces used by this frame
  const int FRAME_1_DAT = 1; //The amount of bytes used by this frame //redundant
  byte frame1Byt[FRAME_1_DAT][8] = {{B00000, B00000, B00000, B00000, B00011, B00111, B01111, B01111}}; //All the required bytes to construct this frame
  int frame1Arr[FRAME_1_SIZ][2] = {{0, 0}, {0, 1}, {0, 2}, {0, 3}}; //The arrangement and IRP method to use for each byte in this frame
  // ------- //

  // Frame 2 //
  const int FRAME_2_DIM[2] = {2, 2};
  const int FRAME_2_SIZ =  4;
  const int FRAME_2_DAT =  1;
  byte frame2Byt[FRAME_2_DAT][8] = {{B00000, B00000, B00011, B01111, B11111, B11111, B11111, B11111}};
  int frame2Arr[FRAME_2_SIZ][2] = {{0, 0}, {0, 1}, {0, 2}, {0, 3}};
  // ------- //

  // Frame 3 //
  const int FRAME_3_DIM[2] = {2, 4};
  const int FRAME_3_SIZ =  8;
  const int FRAME_3_DAT =  2;
  byte frame3Byt[FRAME_3_DAT][8] = {{B00000, B00000, B00000, B00000, B00001, B00001, B00011, B00011}, {B00011, B01111, B11111, B11111, B11111, B11111, B11111, B11111}};
  int frame3Arr[FRAME_3_SIZ][2] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}, {0, 2}, {1, 2}, {1, 3}, {0, 3}};
  // ------- //

  // Frame 4 //
  const int FRAME_4_DIM[2] = {4, 4};
  const int FRAME_4_SIZ =  16;
  const int FRAME_4_DAT =  4;
  byte frame4Byt[FRAME_4_DAT][8] = {{B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000}, {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111}, {B00000, B00000, B00000, B00000, B00000, B00000, B00111, B11111},
    {B00011, B00111, B00111, B01111, B01111, B11111, B11111, B11111}
  };
  int frame4Arr[FRAME_4_SIZ][2] = {{0, 0}, {2, 0}, {2, 1}, {0, 0}, {3, 0}, {1, 0}, {1, 0}, {3, 1}, {3, 2}, {1, 0}, {1, 0}, {3, 3}, {0, 0}, {2, 2}, {2, 3}, {0, 0}};
  // ------- //

  // Frame 5 //
  const int FRAME_5_DIM[2] = {4, 6};
  const int FRAME_5_SIZ =  24;
  const int FRAME_5_DAT =  5;
  byte frame5Byt[FRAME_5_DAT][8] = {{B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000}, {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111}, {B00000, B00000, B00000, B00000, B00000, B00011, B00111, B01111},
    {B00000, B00000, B00000, B00111, B11111, B11111, B11111, B11111}, {B00000, B00000, B00001, B00001, B00001, B00011, B00011, B00011}
  };
  int frame5Arr[FRAME_5_SIZ][2] = {{0, 0}, {2, 0}, {3, 0}, {3, 1}, {2, 1}, {0, 0}, {4, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {4, 1}, {4, 2}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {4, 3}, {0, 0}, {2, 2}, {3, 2}, {3, 3}, {2, 3}, {0 , 0}};
  // ------- //

  // Frame 6 //
  const int FRAME_6_DIM[2] = {4, 6};
  const int FRAME_6_SIZ =  24;
  const int FRAME_6_DAT =  5;
  byte frame6Byt[FRAME_6_DAT][8] = {{B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000}, {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111}, {B00000, B00000, B00000, B00000, B00001, B00011, B00011, B00111},
    {B00001, B00111, B01111, B11111, B11111, B11111, B11111, B11111}, {B01111, B11111, B11111, B11111, B11111, B11111, B11111, B11111}
  };
  int frame6Arr[FRAME_6_SIZ][2] = {{2, 0}, {3, 0}, {1, 0}, {1, 0}, {3, 1}, {2, 1}, {4, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {4, 1}, {4, 2}, {1, 0}, {1, 0}, {1, 0}, {1, 0}, {4, 3}, {2, 2}, {3, 2}, {1, 0}, {1, 0}, {3, 3}, {2 , 3}};
  // ------- //

  // Frame 7 //
  const int FRAME_7_DIM[2] = {4, 6};
  const int FRAME_7_SIZ =  24;
  const int FRAME_7_DAT =  9;
  byte frame7Byt[FRAME_7_DAT][8] = {{B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000}, {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111}, {B00000, B00000, B00000, B00000, B00001, B00011, B00011, B00111},
    {B00001, B00111, B01111, B11111, B11111, B11111, B11110, B11001}, {B11111, B11110, B11110, B11110, B11110, B11110, B01110, B11110}, {B01111, B11111, B11111, B11111, B11111, B11110, B11110, B11110}, {B10111, B01111, B01111, B01111, B01111, B01111, B11111, B11111},
    {B11110, B11110, B11110, B11110, B11111, B11111, B11111, B11111}, {B11111, B01111, B00000, B11000, B11111, B11111, B11111, B11111}
  };
  int frame7Arr[FRAME_6_SIZ][2] = {{2, 0}, {3, 0}, {4, 0}, {4, 1}, {3, 1}, {2, 1}, {5, 0}, {6, 0}, {7, 0}, {7, 1}, {6, 1}, {5, 1}, {5, 2}, {6, 2}, {1, 0}, {1, 0}, {6, 3}, {5, 3}, {2, 2}, {3, 2}, {8, 0}, {8, 1}, {3, 3}, {2 , 3}};
  // ------- //

  for (int i = 0; i < displayPrimarySize[1]; i++) {
    for (int ii = 0; ii < displayPrimarySize[0]; ii++) {
      lcd.setCursor(i, ii);
      lcd.print(" ");
    }
    delay(50);
  }
  boolean selOption;
  for (int p = 0; p < 2; p++) {
    for (int i = 1; i <= 7; i++) {
      lastMillis = millis();
      int ii = i;
      if (rev) {
        ii = 8 - i;
      }
      while (millis() - lastMillis < (unsigned int) animDelay) {
        if (ii == 1) {
          drawFrame(20, FRAME_1_DIM, FRAME_1_SIZ, FRAME_1_DAT, frame1Byt, frame1Arr);
        } else if (ii == 2) {
          drawFrame(20 , FRAME_2_DIM, FRAME_2_SIZ, FRAME_2_DAT, frame2Byt, frame2Arr);
        } else if (ii == 3) {
          drawFrame(20 , FRAME_3_DIM, FRAME_3_SIZ, FRAME_3_DAT, frame3Byt, frame3Arr);
        } else if (ii == 4) {
          drawFrame(20 , FRAME_4_DIM, FRAME_4_SIZ, FRAME_4_DAT, frame4Byt, frame4Arr);
        } else if (ii == 5) {
          drawFrame(20 , FRAME_5_DIM, FRAME_5_SIZ, FRAME_5_DAT, frame5Byt, frame5Arr);
        } else if (ii == 6) {
          drawFrame(20 , FRAME_6_DIM, FRAME_6_SIZ, FRAME_6_DAT, frame6Byt, frame6Arr);
        } else if (ii == 7) { //redundant
          drawFrame(20 , FRAME_7_DIM, FRAME_7_SIZ, FRAME_7_DAT, frame7Byt, frame7Arr);
        }
      }
    }

    if (!gotoBlock) {
      gotoBlock = true;
      rev = true;
      lcd2.clear();
      lcd2.setCursor(0, 0);
      lcd2.print("Shut down OS?");
      lcd2.setCursor(0, 1);
      lcd2.print("Yes<      No");
      selOption = true;
      char selKey = inputKey();
      while (selKey != keyConfirm && selKey != keyESC && selKey != keyShutdown) {
        drawFrame(20 , FRAME_7_DIM, FRAME_7_SIZ, FRAME_7_DAT, frame7Byt, frame7Arr);
        if (selKey == keyLeft && !selOption) {
          lcd2.setCursor(3, 1);
          lcd2.print("<");
          lcd2.setCursor(12, 1);
          lcd2.print(" ");
          selOption = true;
        } else if (selKey == keyRight && selOption) {
          lcd2.setCursor(3, 1);
          lcd2.print(" ");
          lcd2.setCursor(12, 1);
          lcd2.print("<");
          selOption = false;
        }
        selKey = inputKey();
      }
      if (selKey == keyESC) {
        selOption = false;
      } else if (selKey == keyShutdown) {
        selOption = true;
      }
    }
  }

  lcd.clear();
  lcd2.clear();
  if (selOption) {
    isRunning = false;
    lcd2.setBacklightEnabled(false);
  } else {
    opened = "Main_Menu";
    loadMainMenu();
    mainMenu();
  }
}






//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------ \LISTS/ -------------------------------------------------------------------------------------
void refresh(int pos, int selected, String elements[], int elementCount, String values[], boolean paged, boolean infinite, LcdDisplay *lcd, const int displaySize[], void (*onSelect)(String elements[], String values[], int index), int ret[]) { //pos is a value between 0 and elementCount-1;
  String displayed[displaySize[0]];
  //int ret[];

  for (int i = 0; i < displaySize[0]; i++) {
    int ii = pos + i;

    if (pos + i > elementCount - 1) {
      if (infinite == true) {
        ii = pos + i - elementCount;
      } else {
        ii = -1; //? //I don't even know why I put a question mark here anymore
      }
    }
    if (ii != -1) {
      displayed[i] = elements[ii];
      //ret[i] = ii;
    } else {
      displayed[i] = "";
      //ret[i] = ii;
    }
    ret[i] = ii;
  }
  for (int i = 0; i < displaySize[0]; i++) { //For every row in display
    lcd->setCursor(0, i);
    for (int ii = 0; ii < displaySize[1] - 3; ii++) { //margin
      lcd->print(" "); //Clear the entire row
    }
    lcd->setCursor(0, i);
    if (displayed[i].length() > displaySize[1] - 3) { //margin  //If the String meant for this line is longer than the display allows
      String temp = displayed[i];
      temp.remove(displaySize[1] - 3); //Cut it down to the allowed length (3 characters shorter than the display)
      lcd->print(temp);  //margin
    } else {
      lcd->print(displayed[i]); //Just print it if it fits
    }
    if (i == selected) { //If the current row is selected
      lcd->setCursor(displaySize[1] - 1, i);
      lcd->print("<"); //Put the cursor at the end of the line and print it
    } else {
      lcd->setCursor(displaySize[1] - 1, i);
      lcd->print(" "); //Print a space if not, to remove a possible previous cursor
    }
  }
  (*onSelect)(elements, values, ret[selected]);
  //return ret;
}


int list(String elements[], int elementCount, String values[], boolean paged, boolean infinite, LcdDisplay *lcd, const int displaySize[], void (*onSelect)(String elements[], String values[], int index), int listPos[2]) { //Creates a list, returns the index of the item selected
  char curKey = inputKey();
  //int pos = 0;
  int scrollPos = 1;
  //int selected = 0;
  unsigned long lastMillis = millis();
  unsigned long lastSubMillis = millis();
  //String curScreen = "";
  int curScreen[displaySize[0]] = {0, 0, 0, 0}; //Format {0,1,2,3}
  boolean bailed = false;

  const int scrollDelay = 1700;
  const int scrollSpeedDelay = 300;

  lcd->clear();
  refresh(listPos[0], listPos[1], elements, elementCount, values, paged, infinite, lcd, displaySize, (*onSelect), curScreen);
  while (curKey != keyConfirm) {
    if ((unsigned long)(millis() - lastMillis) >= scrollDelay) {

      String currentOption = elements[curScreen[listPos[1]]]; //The current selected String
      if (currentOption.length() > (unsigned int) displaySize[1] - 3) { //margin
        if ((unsigned long)(millis() - lastSubMillis) >= scrollSpeedDelay) {

          if (currentOption.substring(scrollPos, scrollPos + displaySize[1] - 3).length() >= displaySize[1] - 3 ) { //margin
            lcd->setCursor(0, listPos[1]);
            for (int i = 0; i < displaySize[1] - 3; i++) { //margin
              lcd->print(" ");
            }
            lcd->setCursor(0, listPos[1]);
            lcd->print(currentOption.substring(scrollPos, scrollPos + displaySize[1] - 3)); //margin
            scrollPos++;
            lastSubMillis = millis();

          } else {
            lastSubMillis = millis();
            lastMillis = millis();
            scrollPos = 0;
          }
        }
      }

    }

    if (curKey == keyUp) {
      if (listPos[1] > 0) {
        if (elements[curScreen[listPos[1]]].length() > displaySize[1] - 3) {
          scrollPos = 1;
          lcd->setCursor(0, listPos[1]);
          lcd->print(elements[curScreen[listPos[1]]].substring(0, displaySize[1] - 3));
        }
        listPos[1]--;
        lcd->setCursor(displaySize[1] - 1, listPos[1] + 1);
        lcd->print(" ");
        lcd->setCursor(displaySize[1] - 1, listPos[1]);
        lcd->print("<");
        lastMillis = millis();
        (*onSelect)(elements, values, curScreen[listPos[1]]);
      } else {
        if (paged == true) {
          if (infinite == true) {
            if (listPos[0] - displaySize[0] < 0) {
              listPos[0] = elementCount + listPos[0] - displaySize[0] ;
            } else {
              listPos[0] -= displaySize[0];
            }
            listPos[1] = 3;
            lastMillis = millis();
            refresh(listPos[0], listPos[1], elements, elementCount, values, paged, infinite, lcd, displaySize, (*onSelect), curScreen);
          } else if (infinite == false) {
            if (listPos[0] - displaySize[0] < 0) {
              listPos[0] -= displaySize[0];
              listPos[1] = 3;
              lastMillis = millis();
              refresh(listPos[0], listPos[1], elements, elementCount, values, paged, infinite, lcd, displaySize, (*onSelect), curScreen);
            }
          }
        } else if (paged == false) {
          if (infinite == true) {
            if (listPos[0] > 0) {
              listPos[0]--;
            } else {
              listPos[0] = elementCount - 1;
            }
            lastMillis = millis();
            refresh(listPos[0], listPos[1], elements, elementCount, values, paged, infinite, lcd, displaySize, (*onSelect), curScreen);
            (*onSelect)(elements, values, curScreen[listPos[1]]);
          } else if (infinite == false) {
            if (listPos[0] > 0) {
              listPos[0]--;
              lastMillis = millis();
              refresh(listPos[0], listPos[1], elements, elementCount, values, paged, infinite, lcd, displaySize, (*onSelect), curScreen);
            }
          }
        }
      }
    } else if (curKey == keyDown) {
      if (listPos[1] < displaySize[0] - 1 && listPos[1] < elementCount - 1) { //remove && part to fix?
        if (elements[curScreen[listPos[1]]].length() > displaySize[1] - 3) {
          scrollPos = 1;
          lcd->setCursor(0, listPos[1]);
          lcd->print(elements[curScreen[listPos[1]]].substring(0, displaySize[1] - 3));
        }
        listPos[1]++;
        lcd->setCursor(displaySize[1] - 1, listPos[1] - 1);
        lcd->print(" ");
        lcd->setCursor(displaySize[1] - 1, listPos[1]);
        lcd->print("<");
        lastMillis = millis();
        (*onSelect)(elements, values, curScreen[listPos[1]]);
      } else {
        if (paged == true) {
          if (infinite == true) { //This used to say "infinite = true", I fixed it but I don't know what it will fix or not fix, we should test whether this dumb mistake caused paged glitching
            if (listPos[0] + displaySize[0] > elementCount - 1) {
              listPos[0] = listPos[0] + displaySize[0] - elementCount;
            } else {
              listPos[0] += displaySize[0];
            }
            listPos[1] = 0;
            lastMillis = millis();
            refresh(listPos[0], listPos[1], elements, elementCount, values, paged, infinite, lcd, displaySize, (*onSelect), curScreen);
          } else if (infinite == false) {
            if (listPos[0] < elementCount - displaySize[0]) {
              listPos[0] += displaySize[0];
              listPos[1] = 0;
              lastMillis = millis();
              refresh(listPos[0], listPos[1], elements, elementCount, values, paged, infinite, lcd, displaySize, (*onSelect), curScreen);
            }
          }
        } else if (paged == false) {
          if (infinite == true) {
            if (listPos[0] < elementCount - 1) {
              listPos[0]++;
            } else {
              listPos[0] = 0;
            }

            //String programss[] = {"TestProgram", "Calculator", "GoogleSearch", "AnimePlayer", "2dMinecraft", "Music", "Draw"};
            lastMillis = millis();
            refresh(listPos[0], listPos[1], elements, elementCount, values, paged, infinite, lcd, displaySize, (*onSelect), curScreen);
          } else if (infinite == false) {
            if (listPos[0] < elementCount - displaySize[0]) {
              listPos[0]++;
              lastMillis = millis();
              refresh(listPos[0], listPos[1], elements, elementCount, values, paged, infinite, lcd, displaySize, (*onSelect), curScreen);
            }
          }
        }
      }
    } else if (curKey == keyESC) {
      bailed = true;
      goto bail;
    } else if (curKey == keyShutdown) {
      bailed = true;
      Serial.println(F("Shutdown via bail"));
      shutDown();
      goto bail;
    }
    curKey = inputKey();
  }
bail:
  if (bailed) {
    return -1;
  } else {
    Serial.println(F("List returned "));
    Serial.print(String(curScreen[listPos[1]]));
    return curScreen[listPos[1]];
  }
}




void listDoNothing(String elements[], String values[], int i) {}

int list(String elements[], int elementCount, String values[], boolean paged, boolean infinite, LcdDisplay *lcd, const int displaySize[], void (*onSelect)(String elements[], String values[], int index)) {
  int listPos[2] = {0, 0};
  return list(elements, elementCount, values, paged, infinite, lcd, displaySize, (*onSelect), listPos);
}
int list(String elements[], int elementCount, String values[], boolean paged, boolean infinite, LcdDisplay *lcd, const int displaySize[], int listPos[2]) {
  return list(elements, elementCount, values, paged, infinite, lcd, displaySize, listDoNothing, listPos);
}
int list(String elements[], int elementCount, String values[], boolean paged, boolean infinite, LcdDisplay *lcd, const int displaySize[]) {
  return list(elements, elementCount, values, paged, infinite, lcd, displaySize, listDoNothing);
}
int list(String elements[], int elementCount, String values[]) {
  return list(elements, elementCount, values, false, true, Displays::getPrimaryDisplay(), displayPrimarySize, listDoNothing);
}
//--------- /\ With values /\ -------

//------- \/ Without values \/ ------
int list(String elements[], int elementCount, boolean paged, boolean infinite, LcdDisplay *lcd, const int displaySize[], void (*onSelect)(String elements[], String values[], int index), int listPos[2]) {
  String values[] = {""};
  
  return list(elements, elementCount, values, paged, infinite, lcd, displaySize, (*onSelect), listPos);
}
int list(String elements[], int elementCount, boolean paged, boolean infinite, LcdDisplay *lcd, const int displaySize[], void (*onSelect)(String elements[], String values[], int index)) {
  int listPos[2] = {0, 0};
  return list(elements, elementCount, paged, infinite, lcd, displaySize, (*onSelect), listPos);
}
int list(String elements[], int elementCount, boolean paged, boolean infinite, LcdDisplay *lcd, const int displaySize[], int listPos[2]) {
  return list(elements, elementCount, paged, infinite, lcd, displaySize, listDoNothing, listPos);
}
int list(String elements[], int elementCount, boolean paged, boolean infinite, LcdDisplay *lcd, const int displaySize[]) {
  return list(elements, elementCount, paged, infinite, lcd, displaySize, listDoNothing);
}
int list(String elements[], int elementCount) {
  return list(elements, elementCount, false, true, Displays::getPrimaryDisplay(), displayPrimarySize, listDoNothing);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------- \Bomb Mode/ ------------------------------------------------------------------------------------
//Features:
//Settings a timer DD:HH:MM:SS
//A confirmation popup if sound is disabled
//Set Pincode to unlock
//Beeping Sounds
void bombModeOnSelect(String elements[], String values[], int index) {

}

void bombMode() { //EEPROM group 2, Byte Usage: Timer - 3, Pincode - 4, Mistakes - 1, Beeping - 1, Message - 20 || 29
  boolean exitProgram = false;
  lcd2.clear();
  lcd2.setCursor(0, 0);
  lcd2.print("Bomb Mode");



  //if (restoreData && EEPROM.read(eepromConfig[2][0]) == 1) { //Don't know what this was for, but it accessed an array out of bounds so I commented it out, the continue was already commented out
    //continue
  //}

  int listPos[] = {0, 0};
  while (!exitProgram) {
    const int optionCount = 5;
    String optionList[] = {F("Set Timer"), F("Set Pincode"), F("Set Mistakes"), F("Set Beeping"), F("- Start -")};
    String optionListValues[] = {};
    lcd2.setCursor(0, 1);
    lcd2.print(F("                "));
    lcd2.setCursor(0, 1);
    lcd2.print(F("-Config"));
    int option = list(optionList, optionCount, false, false, &lcd, displayPrimarySize, bombModeOnSelect, listPos);
    if (option == -1) {
      exitProgram = true;
    } else if (option == 0) {
      lcd.clear();
      bigNum.begin();
      while (inputKey() != keyESC) {
        int currentTime = millis() / 100;
        byte lastDigit = currentTime % 10;
        currentTime = currentTime / 10;
        bigNum.displayLargeInt(currentTime, 0, 4, false);

        lcd.setCursor(12, 1);
        lcd.print(".");
        lcd.print(lastDigit);
      }
    } else if (option == 1) {

    } else if (option == 2) {

    } else if (option == 3) {

    } else if (option == 4) {

    } else if (option == 5) {

    }

  }
}




//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------- \RGB LED CONFIGURATOR/ ------------------------------------------------------------------------------
void rgbLedConfiguratorUpdateCursor(int selection) {
  for (int i = 0; i < 4; i++) {
    //Serial.println("For loop run: " + String(i) ", selection is" + String(selection));
    if (i != selection) {
      //Serial.println("Wasn't equal to selection, set the cursor at " + String(displayPrimarySize[1] - 1) + ", " + String(i));
      lcd.setCursor(displayPrimarySize[1] - 1, i);
      lcd.print(" ");
    } else {
      //Serial.println("Was equal to the selection");
      if (i == 3) {
        i = displayPrimarySize[0] - 1;
      }
      lcd.setCursor(displayPrimarySize[1] - 1, i);
      lcd.print("<");
    }
  }
}

void rgbLedConfiguratorUpdateValue(int rgb) {
  rgbIndicator(rgbValues[0], rgbValues[1], rgbValues[2]);
  lcd.setCursor(displayPrimarySize[1] - 5, rgb);
  lcd.print("   ");
  lcd.setCursor(displayPrimarySize[1] - String(rgbValues[rgb]).length() - 2, rgb);
  lcd.print(String(rgbValues[rgb]));
}

void rgbLedConfiguratorColorsOnSelect(String elements[], String values[], int index) {
  //String values[] = {"130,105,0", "160,0,180", "0,100,120", "120,105,255", "0,0,0"};
  rgbIndicator(values[index].substring(0, values[index].indexOf(",")).toInt(), values[index].substring(values[index].indexOf(",") + 1, values[index].lastIndexOf(",")).toInt(), values[index].substring(values[index].lastIndexOf(",") + 1, values[index].length()).toInt(), true);
}

void rgbLedConfiguratorSaveToEeprom() {
  for (int i = 0; i < 3; i++) {
    EEPROM.update(eepromConfig[1][1] + i, rgbValues[i]);
    //Serial.println("RGB Values Saved at address " + String(eepromConfig[1][1] + i) + ", R: " + String(rgbValues[0]) + ", G: " + String(rgbValues[1]) + ", B: " + String(rgbValues[2]));
  }
  Serial.println(F("RGB Values Saved."));
}

void rgbLedConfigurator() {
  lcd2.clear();
  lcd2.setCursor(0, 0);
  lcd2.print("RGB LED Config");
  const int optionCount = 4;
  String optionList[optionCount] = {F("Set RGB values"), F("Colors"), F("Cycle"), F("Turn off")};
  const int addStepsSz = 5;
  int addSteps[addStepsSz] = {1, 5, 10, 30, 255};
  int add = addSteps[3];
  boolean exitProgram = false;
  int listPos[] = {0, 0};
  while (!exitProgram) {
    lcd2.setCursor(0, 1);
    lcd2.print(F("                "));
    lcd2.setCursor(0, 1);
    lcd2.print(F("-Menu"));
    int option = list(optionList, optionCount, true, true, &lcd, displayPrimarySize, listPos);
    if (option == -1) {
      exitProgram = true;
    } else if (option == 0) {
      lcd2.setCursor(0, 1);
      lcd2.print("                ");
      lcd2.setCursor(0, 1);
      lcd2.print(F(">Set RGB Value"));
      String rgb[] = {"R:", "G:", "B:"};
      lcd.clear();
      for (int i = 0; i < 3; i++) {
        lcd.setCursor(0, i);
        lcd.print(rgb[i]);
        lcd.setCursor(displayPrimarySize[1] - String(rgbValues[i]).length() - 2, i);
        lcd.print(String(rgbValues[i]));
      }
      lcd.setCursor(0, displayPrimarySize[0] - 1);
      lcd.print("+" + String(add));

      char curKey = inputKey();
      int selection = 0;
      rgbIndicator(rgbValues[0], rgbValues[1], rgbValues[2]);
      rgbLedConfiguratorUpdateCursor(selection);
      while (curKey != keyESC && curKey != keyShutdown) {
        if (curKey == keyUp && selection > 0) {
          selection--;
          rgbLedConfiguratorUpdateCursor(selection);
        } else if (curKey == keyDown && selection < 3) {
          selection++;
          rgbLedConfiguratorUpdateCursor(selection);
        } else if (curKey == keyRight && selection < 3) {
          if (rgbValues[selection] == 255) {
            rgbValues[selection] = 0;
          } else if (rgbValues[selection] + add > 255) {
            rgbValues[selection] = 255;
          } else {
            rgbValues[selection] += add;
          }
          rgbLedConfiguratorUpdateValue(selection);
        } else if (curKey == keyLeft && selection < 3) {
          if (rgbValues[selection] == 0) {
            rgbValues[selection] = 255;
          } else if (rgbValues[selection] - add < 0) {
            rgbValues[selection] = 0;
          } else {
            rgbValues[selection] -= add;;
          }
          rgbLedConfiguratorUpdateValue(selection);
        } else if (curKey == keyConfirm) {
          if (selection != 3) {
            unsigned long lastMillis = 0;
            boolean flash = true;
            String input = "";
            char curKey2 = inputKey();
            byte blankChar[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B111111, B11111};
            lcd.createChar(0, blankChar);
            char charKeys[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
            while ((curKey2 == 0 || curKey2 == '0' || curKey2 == '1' || curKey2 == '2' || curKey2 == '3' || curKey2 == '4' || curKey2 == '5' || curKey2 == '6' || curKey2 == '7' || curKey2 == '8' || curKey2 == '9') && input.length() < 3 && input != "0") {
              if (millis() - lastMillis >= 400) {
                if (!flash) {
                  lcd.setCursor(displayPrimarySize[1] - 5, selection);
                  //                  for (int i = 1; i + input.length() <= 3; i++) {
                  //                    lcd.print(" ");
                  //                  }
                  lcd.print("   ");
                  lcd.setCursor(displayPrimarySize[1] - input.length() - 2, selection);
                  lcd.print(input);
                } else {
                  lcd.setCursor(displayPrimarySize[1] - 5, selection);
                  for (int i = 1; i + input.length() <= 3; i++) {
                    lcd.write(byte(0));
                  }
                }
                flash = !flash;
                lastMillis = millis();
              }
              if (curKey2) {
                for (int i = 0; i < 10; i++) {
                  if (curKey2 == charKeys[i]) {
                    input += String(i);
                    lcd.setCursor(displayPrimarySize[1] - 5, selection);
                    if (!flash) {
                      lcd.write(byte(0));
                      lcd.write(byte(0));
                      lcd.write(byte(0));
                    } else {
                      lcd.print("   ");
                    }
                    lcd.setCursor(displayPrimarySize[1] - input.length() - 2, selection);
                    lcd.print(input);
                    //flash = false;
                    //lastMillis -= 500;
                    break;
                  }
                }
              }
              curKey2 = inputKey();
            }
            lcd.setCursor(displayPrimarySize[1] - 5, selection);
            for (int i = 1; i + input.length() <= 3; i++) {
              lcd.print(" ");
            }
            if (input.toInt() > 255) {
              input = "255";
            }
            rgbValues[selection] = input.toInt();
            rgbLedConfiguratorUpdateValue(selection);
          } else {
            for (int i = 0; i < addStepsSz; i++) {
              if (addSteps[i] == add) {
                if (i == addStepsSz - 1) {
                  i = -1;
                }
                add = addSteps[i + 1];
                lcd.setCursor(1, displayPrimarySize[0] - 1);
                lcd.print("    ");
                lcd.setCursor(1, displayPrimarySize[0] - 1);
                lcd.print(String(add));
                break;
              }
            }
          }
        }
        curKey = inputKey();
      }
      if (curKey == keyShutdown) {
        exitProgram = true;
        shutDown();
      }
      rgbLedConfiguratorSaveToEeprom();
    } else if (option == 1) {
      lcd2.setCursor(0, 1);
      lcd2.print("                ");
      lcd2.setCursor(0, 1);
      lcd2.print(">Colors");
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
    } else if (option == 2) {
      //add cycle
    } else if (option == 3) {
      analogWrite(RGBleds[0], 0);
      analogWrite(RGBleds[1], 0);
      analogWrite(RGBleds[2], 0);
      lcd2.setCursor(0, 1);
      lcd2.print("                ");
      lcd2.setCursor(0, 1);
      lcd2.print(F(">Turned Off."));
      delay(850);
      inputKey();
      //String rgb[] = {"R:", "G:", "B:"};
    }
  }
  lcd2.clear();
  Serial.println(F("Quitting Program"));
}



//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------





//------------------------------------------------------------------------------------- \DRAW/ -------------------------------------------------------------------------------------
void refreshCursor(int cursorPos[], boolean curBlink, LiquidCrystal *lcd) {
  lcd->setCursor(cursorPos[0], cursorPos[1] / 2);
  if (curBlink) { //shouldn't just overwrite anything
    lcd->write(byte(1));
  } else {
    lcd->write(byte(0));
  }
}

void draw(LiquidCrystal *lcd, const int displaySize[]) { //A quick and simple drawing program without any extra features
  byte characters[][8] = {{B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000}, {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111}, {B00000, B00000, B00000, B00000, B11111, B11111, B11111, B11111},
    {B11111, B11111, B11111, B11111, B00000, B00000, B00000, B00000}
  };
  String drawing = "";  //format "h,vb0 h,vb0"...
  int cursorPos[2] = {0, 0}; //format h,v
  char curKey = inputKey();
  unsigned long lastMillis = millis();
  boolean curBlink = true;

  int blinkDelay = 500;

  lcd->clear();
  lcd->createChar(0 , characters[0]);
  lcd->createChar(1, characters[1]);
  lcd->createChar(2, characters[2]);
  lcd->createChar(3, characters[3]);
  for (int i = 0; i < displaySize[0] * 2; i++) {
    for (int ii = 0; ii < displaySize[1]; ii++) {
      drawing += String(ii) + "," + String(i) + "b" + "0" ;
    }
  }
  while (curKey != keyESC && curKey != keyShutdown) {
    curKey = inputKey();
    if ((unsigned long)(millis() - lastMillis) >= (unsigned int) blinkDelay) {
      curBlink = !curBlink;
      refreshCursor(cursorPos, curBlink, lcd);
      lastMillis = millis();
    }
    if (curKey == keyUp || curKey == keyDown || curKey == keyLeft || curKey == keyRight) {
      lcd->setCursor(cursorPos[0], cursorPos[1] / 2); //Shouldn't just clear the character
      lcd->print(" ");
      lcd2.setCursor(0, 0);
      if (curKey == keyUp) {
        if (cursorPos[1] > 0) {
          cursorPos[1] = cursorPos[1] - 1;
          lcd2.print(String(cursorPos[0]) + "," + String(cursorPos[1]));
          refreshCursor(cursorPos, curBlink, lcd);
        }
      } else if (curKey == keyDown) {
        if (cursorPos[1] < displaySize[0] * 2 - 1) {
          cursorPos[1] = cursorPos[1] + 1;
          lcd2.print(String(cursorPos[0]) + "," + String(cursorPos[1]));
          refreshCursor(cursorPos, curBlink, lcd);
        }
      } else if (curKey == keyRight) {
        if (cursorPos[0] < displaySize[1] - 1) {
          cursorPos[0] = cursorPos[0] + 1;
          lcd2.print(String(cursorPos[0]) + "," + String(cursorPos[1]));
          refreshCursor(cursorPos, curBlink, lcd);
        }
      } else if (curKey == keyLeft) {
        if (cursorPos[0] > 0) {
          cursorPos[0] = cursorPos[0] - 1;
          lcd2.print(String(cursorPos[0]) + "," + String(cursorPos[1]));
          refreshCursor(cursorPos, curBlink, lcd);
        }
      }
    }
    if (curKey == keyConfirm) {
      //int curChar = drawing.substring(drawing.indexOf("b", drawing.indexOf(String(cursorPos[0]) + "," + String(cursorPos[1]))) + 1, drawing.indexOf("b", drawing.indexOf(String(cursorPos[0]) + "," + String(cursorPos[1]))) + 2).toInt();

    }

  } //format "h,vb0 h,vb0"

}



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------




void setupInput() {

  long unsigned int codes[] = {
      16738455, 16724175, 16718055, 16743045,
      16716015, 16726215, 16734885, 16728765,
      16730805, 16732845, 16769055, 16754775,
      16753245, 16736925, 16769565, 16720605,
      16712445, 16761405, 16748655, 16750695,
      16756815
    };
    char keyValues[] = {
      '0', '1', '2', '3',
      '4', '5', '6', '7',
      '8', '9', '*', '#', //8,9,-,+
      'A', 'B', 'C', 'D', //CH-, CH, CH+, PREV
      'e', 'f', 'g', 'h', //NEXT, PL/PAU, EQ, 100+
      'i'                 //200+
    };
  for(unsigned int i = 0; i < (sizeof(codes) / sizeof(codes[0])); i++) {
    irInputDevice->addMap(codes[i], keyValues[i]);
  }

  
  Input::addInputDevice(irInputDevice);
  Input::addInputDevice(keypadInputDevice);


  Input::addAction("up")
    ->addMapping(irInputDevice, '2')
    ->addMapping(keypadInputDevice, '2');

  Input::addAction("down")
    ->addMapping(irInputDevice, '8')
    ->addMapping(keypadInputDevice, '8');

  Input::addAction("left")
    ->addMapping(irInputDevice, '4')
    ->addMapping(keypadInputDevice, '4');

  Input::addAction("right")
    ->addMapping(irInputDevice, '6')
    ->addMapping(keypadInputDevice, '6');

  Input::addAction("confirm")
    ->addMapping(irInputDevice, '5')
    ->addMapping(keypadInputDevice, '5');

  Input::addAction("back")
    ->addMapping(irInputDevice, '*')
    ->addMapping(keypadInputDevice, '*');

  
  Input::addAction("shutdown")
    ->addMapping(irInputDevice, '#')
    ->addMapping(keypadInputDevice, '#');

  
  Serial.println("Input setup complete.");
}

void setupDisplays() {
  lcd.begin(20, 4);
  Displays::setPrimaryDisplay(&lcd);

  lcd2.init();
  lcd2.setBacklightEnabled(false);
  Displays::setSecondaryDisplay(&lcd2);
  


}

void setup() {
  
  //What is this for???
  pinMode(2, OUTPUT);

  setupDisplays();
  
  delay(40);
  Serial.begin(9600);
  //Serial.print("EEPROM address 0 holds: ");
  //Serial.println(EEPROM.read(0));

  //const int eepromGroupsSz = 2;
  //const int eepromGroups[eepromGroupsSz] {8, 3};
  //const int eepromConfig[][3];// = {{0,1,8}, {9,10,12}}; {CheckByte, DataStartByte, DataEndByte}

  setupInput();

  setupEepromConfig(true); //Formats the address assignments into eepromConfig  //true for debug printing
  if (restoreData) {
    restoreEepromData();
  }
  bootup();
}

void loop() {
  char key = inputKey();//keypad.getKey();
  inputKey();
  if (isRunning) {
    if (key == keyESC) {
      boolean dublicate = false;
      for (int i = 0; i < 7; i++) {
        for (int ii = i + 1; ii < 7; ii++) {
          if (tempKeyAssign[i] && tempKeyAssign[i] == tempKeyAssign[ii]) {
            dublicate = true;
            break;
          }
        }
        if (dublicate) {
          lcd2.clear();
          lcd2.print(F("Dublicate"));
          lcd2.setCursor(0, 1);
          lcd2.print("assignments!");
          rgbIndicator(255, 0, 0);
          delay(50);
          rgbIndicator(0, 0, 0);
          delay(50);
          rgbIndicator(255, 0, 0);
          delay(50);
          rgbIndicator(0, 0, 0);
          delay(500);
          lcd2.clear();
          break;
        }
      }
      if (!dublicate) {
        if (opened == "Settings") { //save settings
          char * keyRefs[] = {&keyLeft, &keyRight, &keyUp, &keyDown, &keyConfirm, &keyESC, &keyShutdown};
          for (int i = 0; i < 7 && tempKeyAssign[i]; i++) {
            *keyRefs[i] = tempKeyAssign[i];
          }
          char saveVars[] = {(byte) true, (byte) muted , keyLeft, keyRight, keyUp, keyDown, keyConfirm, keyESC, keyShutdown};
          for (int i = 0; i < 9; i++) {
            EEPROM.update(eepromConfig[0][0] + i, saveVars[i]);
            //Serial.print("Saved '");
            //Serial.print(saveVars[i]);
            //Serial.println("' at address " + String(eepromConfig[0][0]+i));
          }
          Serial.println(F("Settings saved."));
        }
        opened = "Main_Menu";
        lcd.clear();
        lcd2.clear();
        loadMainMenu();
        settingSelectedOnPage = 0;
        settingPage = 0;
        //seetingSelectedGeneral = 0;
      }
    }
    if (key == keyShutdown) {
      Serial.println(F("Shutdown via keypress"));
      opened = "Main_Menu";
      shutDown();
    }
    if (opened == "Main_Menu" && isRunning) {
      mainMenu();
      if (key) {
        if (key == keyRight) {
          mainMenuSelected += 1;
        }
        if (key == keyLeft) {
          mainMenuSelected -= 1;
        }
        if (key == keyConfirm) {
          if (mainMenuSelected == 0) {
            opened = "programs";
          }
          if (mainMenuSelected == 1) {
            char keyRefs[] = {keyLeft, keyRight, keyUp, keyDown, keyConfirm, keyESC, keyShutdown};
            for (int i = 0; i < 7; i++) {
              //            Serial.print(F("temp before: "));
              //            Serial.println(tempKeyAssign[i]);
              tempKeyAssign[i] = keyRefs[i];
              //            Serial.print(F("temp: "));
              //            Serial.println(tempKeyAssign[i]);
              //            Serial.print(F("ref: "));
              //            Serial.println(keyRefs[i]);
            }
            opened = "Settings";
            lcd.clear();
          }
          if (mainMenuSelected == 2) {
            Serial.println(F("Shutdown via menu option"));
            shutDown();
          }
        }
      }
    } else {
      if (opened == "Settings") {
        String settings[] = {F("Mute"), F("Left"), F("Right"), F("Up"), F("Down"), F("Confirm"), F("Esc"), F("Shutdown")}; //The EEPROM uses the same order STARTING AT 1 (not 0)
        if (key == keyDown) {
          if (settingSelectedOnPage == 3) {
            settingSelectedOnPage = 0;
            settingPage += 1;
            lcd.clear();
          } else {
            settingSelectedOnPage += 1;
          }
        } else {
          if (key == keyUp) {
            if (settingSelectedOnPage == 0) {
              settingPage -= 1;
              settingSelectedOnPage = 3;
              lcd.clear();
            } else {
              settingSelectedOnPage -= 1;
            }
          } else {
            if (key == keyConfirm) {
              delay(10);
            }
          }
        }
        if (settingPage == -1) {
          settingPage = 1;
        }
        if (settingPage == 2) {
          settingPage = 0;
        }
        if (settingPage == 0) {
          lcd.setCursor(1, 0);
          lcd.print(settings[0]);
          if (muted) {
            lcd.setCursor(13, 0);
            lcd.print(F(" true"));
          } else {
            lcd.setCursor(13, 0);
            lcd.print(F("false"));
          }
          lcd.setCursor(1, 1);
          lcd.print(settings[1]);
          lcd.setCursor(17, 1);
          lcd.print(tempKeyAssign[0]);
          lcd.setCursor(1, 2);
          lcd.print(settings[2]);
          lcd.setCursor(17, 2);
          lcd.print(tempKeyAssign[1]);
          lcd.setCursor(1, 3);
          lcd.print(settings[3]);
          lcd.setCursor(17, 3);
          lcd.print(tempKeyAssign[2]);
        } else {
          if (settingPage == 1) {
            lcd.setCursor(1, 0);
            lcd.print(settings[4]);
            lcd.setCursor(17, 0);
            lcd.print(tempKeyAssign[3]);
            lcd.setCursor(1, 1);
            lcd.print(settings[5]);
            lcd.setCursor(17, 1);
            lcd.print(tempKeyAssign[4]);
            lcd.setCursor(1, 2);
            lcd.print(settings[6]);
            lcd.setCursor(17, 2);
            lcd.print(tempKeyAssign[5]);
            lcd.setCursor(1, 3);
            lcd.print(settings[7]);
            lcd.setCursor(17, 3);
            lcd.print(tempKeyAssign[6]);
          }
        }
        for (int i = 0; i < displayPrimarySize[0]; i++) {
          lcd.setCursor(18, i);
          if (i == settingSelectedOnPage) {
            lcd.print("<");
          } else {
            lcd.print(" ");
          }
        }
        int settingRam = 0;
        settingRam = (settingSelectedOnPage + 1) + (settingPage * 4);
        //lcd2.print(settingRam);

        if (key == keyConfirm) {
          //char * keyRefs[] = {&keyLeft, &keyRight, &keyUp, &keyDown, &keyConfirm, &keyESC, &keyShutdown}; //idk why this is not used
          if (settingRam == 1) {
            muted = !muted;
          } else {
            byte g[8] = {B00000, B00000, B01111, B10001, B10001, B01111, B00001, B01110};
            lcd2.createChar(0, g);
            lcd2.setCursor(0, 0);
            lcd2.print(F("Press to assi"));
            lcd2.write(byte(0));
            lcd2.print(F("n"));
            delay(100);
            boolean assigned = false;
            while (!assigned) {
              key = inputKey();
              if (key == '1' or key == '2' or key == '3' or key == 'A' or key == '4' or key == '5' or key == '6' or key == 'B' or key == '7' or key == '8' or key == '9' or key == 'C' or key == '*' or key == '0' or key == '#' or key == 'D' or key) { // lol
                if ((key == keyConfirm || key == keyLeft || key == keyRight || key == keyUp || key == keyDown || key == keyESC || key == keyShutdown) && false) {
                  lcd2.setCursor(0, 1);
                  lcd2.print(key);
                  lcd2.createChar(0, g);
                  lcd2.setCursor(1, 1);
                  lcd2.print(F(" already assi"));
                  lcd2.write(byte(0));
                  lcd2.print(F("ned"));
                  rgbIndicator(255, 0, 0);
                  delay(50);
                  rgbIndicator(0, 0, 0);
                  delay(50);
                  rgbIndicator(255, 0, 0);
                  delay(50);
                  rgbIndicator(0, 0, 0);
                  delay(500);
                  lcd2.clear();
                  lcd2.createChar(0, g);
                  lcd2.setCursor(0, 0);
                  lcd2.print(F("Press to assi"));
                  lcd2.write(byte(0));
                  lcd2.print(F("n"));
                } else {
                  //*keyRefs[settingRam - 2] = key;
                  tempKeyAssign[settingRam - 2] = key;
                  lcd.setCursor(17, 1);
                  lcd.print(keyLeft); //Why are we printing keyLeft???
                  lcd2.clear();
                  assigned = true;
                }
              }
            }
          }
        }
      } else if (opened == "programs") {  //Program list
        //lcd2.setCursor(0,0); //Might be redundant
        //lcd2.print("Select a program")
        String programNames[programCount];
        for (int i = 0; i < programCount; i++) {
          strcpy_P(buffer, (PGM_P)pgm_read_word(&(programs[i])));
          programNames[i] = buffer;
        }
        int launch = list(programNames, programCount, false, true, &lcd, displayPrimarySize, programMenuDisplayDesc, programMenuListPos);

        switch (launch) {
          case -1:  //Return to menu
            lcd.clear();
            lcd2.clear();
            opened = "Main_Menu";
            loadMainMenu();
            mainMenu();
            break;
          case 0: //Launch program 0
            //function program 0
            break;
          case 1://Launch program 1
            OwOProgram().start();
            
            break;
          case 7:
            lcd2.clear();
            lcd2.setCursor(0, 0);
            lcd2.print(F("Draw"));
            draw(lcd.getLcd(), displayPrimarySize);
            break;
          case 8:
            //function program 8
            break;
          case 9:
            rgbLedConfigurator();
            break;
          case 10:
            //function program 10
            break;
          case 11:
            bombMode();
            break;
        }
      }
    }
  } else {
    if (key) {
      bootup();
    }
  }
}








//TODO: Improve the frame renderer of shutdown anim to calculate frequency and importance of characters
//      add a "..." when scrolling a name in a list
//      list fixes:
//        Fix paging not working
//        Single infinite: no problems;
//        Paged infinite: no problems;
//        Paged: Pressing up totally broken, list scrolls infinitely down;
//