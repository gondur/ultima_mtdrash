#ifndef VIC20FONT_H
#define VIC20FONT_H

#include "math/primitives.h"

// Constants for colors:
const byte BLACK = 0;
const byte WHITE = 1;
const byte RED = 2;
const byte CYAN = 3;
const byte PURPLE = 4;
const byte GREEN = 5;
const byte BLUE = 6;
const byte YELLOW = 7;
const byte ORANGE = 8;
const byte BROWN = 9;
const byte LIGHTRED = 10;
const byte DARKGRAY = 11;
const byte MEDIUMGRAY = 12;
const byte LIGHTGREEN = 13;
const byte LIGHTBLUE = 14;
const byte LIGHTGRAY = 15;


extern byte vic20_palette[16*3];

// Simple class for drawing VIC20 character onto a SDL surface
class Vic20Font {
 private:
  byte* charset;
  byte* character; // workspace 
  int vertDisplacement;
 
 public:
  Vic20Font(const string& filename);
  ~Vic20Font(); 

  // xpos,ypos is in the VIC20 screen space (22x23)
  // X is right, Y down
  void drawCharacter(int xpos, int ypos, byte nr, byte color, byte bgColor=BLACK);
  void setVertDisplacement(int nrPixels) {vertDisplacement = nrPixels; }
    
};

#endif

