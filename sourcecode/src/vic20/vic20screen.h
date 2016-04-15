#ifndef VIC20SCREEN_H
#define VIC20SCREEN_H

#include "math/primitives.h"
#include "vic20font.h"


class Vic20Screen {
 private:
  const byte screenWidth;
  const byte screenHeight;
 
  byte* vid_memory;
  byte* color_memory;
  byte* bgcolor_memory; // The vic20 does not have this, but we need it for double buffering
  Vic20Font* vic20Font;
 
  byte cursorX, cursorY;
  byte fgColor; // Current foreground color
  byte bgColor; // Current background color
 
  void advanceCursorX();
  void advanceCursorY();
  
  static Vic20Screen* instance;
  Vic20Screen();
  ~Vic20Screen();
  
 public: 
  static Vic20Screen* getInstance();

  void clear();
  void clearBorder(byte color);
  void draw(int pixelVertDisplacement = 0);
 
  void setCursorPos(byte x, byte y);
  void setFGColor(byte color);
  void setBGColor(byte color);
   
  void drawAtPos(int x, int y, byte c); 
  void drawAtPos(int x, int y, byte c, byte color); 
  void drawAtCursor(byte c);              // draw character, use current fg color
  void drawAtCursor(byte c, byte color);  // draw character, color given explicit
  
  void drawPETASCIIString(const string& str);
  void drawPCASCIIText(char const *format, ...);
};




#endif
