#include "Vic20Screen.h"
#include "utility/Logger.h"
#include "sdlUtil.h"
#include "string.h"
#include "stdarg.h"

Vic20Screen* Vic20Screen::instance = NULL;


Vic20Screen* Vic20Screen::getInstance() {
  if (instance == NULL)
    instance = new Vic20Screen();
    
  return instance; 
}


Vic20Screen::Vic20Screen() : screenWidth(22), screenHeight(23) {
  
  // Create memory buffers:
  vid_memory = new byte[screenWidth*screenHeight];
  color_memory = new byte[screenWidth*screenHeight];
  bgcolor_memory = new byte[screenWidth*screenHeight];
  
  
  // create the font
  vic20Font = new Vic20Font("vic20.fnt");
  
 
  // Colors:
  fgColor = WHITE;
  
  cursorX = 0;
  cursorY = 0;
}


Vic20Screen::~Vic20Screen() {
  delete vic20Font;
  delete vid_memory;
  delete color_memory;
  delete bgcolor_memory;
  
}


// this will also clear the content of the vic20 screen (SDL wise, not the memory buffers)
void Vic20Screen::clearBorder(byte color) {
  COLOR SDLClearColor = COLOR(vic20_palette[color*3],vic20_palette[color*3+1],vic20_palette[color*3+2]);
  clearSDL(SDLClearColor);
}

void Vic20Screen::clear() {
  memset(vid_memory, 0x20, 22*23); // fill video memory with spaces
  memset(color_memory, fgColor, 22*23); // Clear color memory 
  memset(bgcolor_memory, bgColor, 22*23); // Clear bgcolor memory

  cursorX = 0;
  cursorY = 0;
}
 

// Draw the vic20 screen to the SDL surface 
void Vic20Screen::draw(int pixelVertDisplacement) {
  
  clearBorder(5); 
  
  vic20Font->setVertDisplacement(pixelVertDisplacement);
  
  for (int i=0; i<22; i++) {
    for (int j=0; j<23; j++) {
      int index = j*screenWidth + i;
      vic20Font->drawCharacter(i, j, vid_memory[index], color_memory[index], bgcolor_memory[index]);      
    } 
  }
}


void Vic20Screen::setCursorPos(byte x, byte y) {
  cursorX = x;
  cursorY = y;
}


void Vic20Screen::setFGColor(byte color) {
  fgColor = color;
}

void Vic20Screen::setBGColor(byte color) {
  bgColor = color; 
}


void Vic20Screen::advanceCursorY() {
  cursorY++;
    
  if (cursorY > screenHeight) {
    //must scroll:
    memcpy(vid_memory, &vid_memory[screenWidth], screenWidth*screenHeight -  screenWidth);
    memcpy(color_memory, &color_memory[screenWidth], screenWidth*screenHeight -  screenWidth);
    memcpy(bgcolor_memory, &bgcolor_memory[screenWidth], screenWidth*screenHeight -  screenWidth);
    
    memset(&vid_memory[screenHeight*screenWidth - screenWidth], 0x20, screenWidth);
    memset(&color_memory[screenHeight*screenWidth - screenWidth], fgColor, screenWidth);
    memset(&bgcolor_memory[screenHeight*screenWidth - screenWidth], bgColor, screenWidth);
    
      
    cursorY--;
  } 
}


void Vic20Screen::advanceCursorX() {
  cursorX++;
  
  if (cursorX == screenWidth) {
    cursorX = 0;
    advanceCursorY(); 
  } 
}



 
 
void Vic20Screen::drawAtPos(int x, int y, byte c, byte color) {
  vid_memory[y*screenWidth+x] = c;
  color_memory[y*screenWidth+x] = color;
  bgcolor_memory[y*screenWidth+x] = bgColor;
  
}

void Vic20Screen::drawAtPos(int x, int y, byte c) {
  drawAtPos(x,y,c,fgColor);
} 


void Vic20Screen::drawAtCursor(byte c, byte color) {
  vid_memory[cursorY*screenWidth + cursorX] = c;
  color_memory[cursorY*screenWidth + cursorX] = color;
  bgcolor_memory[cursorY*screenWidth + cursorX] = bgColor;
  
  advanceCursorX();
}

void Vic20Screen::drawAtCursor(byte c) {
  drawAtCursor(c, fgColor);  
}           




// Thanks to Michael Fink (vividos@users.sourceforge.net) for this one!!
void Vic20Screen::drawPETASCIIString(const string& str) {

  const char* text = str.c_str();
  
  // Draw the string..   
  bool reverse = false;
  byte ch = *text;

  while(ch != 0) {
    switch(ch) {
     case 0x05: // color white
       fgColor = WHITE;
       break;
     case 0x11: // cursor down
       advanceCursorY();
       break;
     case 0x12: // rvs on
       reverse = true;
       break;
     case 0x1c: // color red
       fgColor = RED;
       break;
     case 0x1d: // cursor right
       advanceCursorX();
       break;
     case 0x1e: // color green
       fgColor = GREEN;
       break;
     case 0x1f: // color blue
       fgColor = BLUE;
       break;

     // control codes 0x80-0x9f
     case 0x8d: // Return:
       cursorX = 0;
       advanceCursorY();
       break;
     case 0x90: // color black
       fgColor = BLACK;
       break;
     case 0x91: // cursor up
       cursorY--; // good enough?
       break;
     case 0x92: // rvs off
       reverse = false;
       break;

     case 0x13: // both 0x13 and 0x93 is a clear.. but in our case it seems wrong to clear on 0x13 (used per tile in 3d view)
       cursorX = 0;
       cursorY = 0;
       break;
     case 0x93: // clear screen and set cursor to 0,0
       clear();
       break;
     case 0x9c: // color purple
       fgColor = PURPLE;
       break;
     case 0x9d: // cursor left
       cursorX--; // good enough? 
       break;
     case 0x9e: // color yellow
       fgColor = YELLOW; 
       break;
     case 0x9f: // color cyan
       fgColor = CYAN; 
       break;
     default:
       if (ch<0x20 || (ch>=0x80 && ch<0xa0)) {
          Log("unprocessed control code: %i", ch);
          break; // don't process unprocessed control codes
       }

       // remap ascii to screen codes
       if (ch>=0x20 && ch<0x40) ; // do nothing
       else if (ch>=0x40 && ch<0x60) ch -= 0x40;
       else if (ch>=0x60 && ch<0x80 &&ch!=0x7e) ch -= 0x20;
       else if (ch>=0xa0 && ch<=0xbe) ch -= 0x40; // 0xbf is mapped specially
       else if (ch>=0xc0 && ch<0xe0) ch -= 0x80;
       else if (ch>=0xe0 && ch<=0xfe) ch -= 0x80; // 0xff is mapped specially

       if (ch==0x7e || ch==0xff) ch = 0x5e;

       if (reverse && ch<0x80) ch += 0x80;

       // Draw the character
       drawAtCursor(ch);
              
       break;
    } // Switch

    ch = *++text;
   } // while(ch!=0)
  
   // Linebreak:
   cursorX=0;
   advanceCursorY();
}


// This function will draw text with a small selected number of characters:
void Vic20Screen::drawPCASCIIText(char const *format, ...) {
  char buffer[MAX_LOG_ENTRY_SIZE+1];
  va_list errmsg;
                
  va_start (errmsg, format);
  vsprintf (buffer, format, errmsg);
  va_end (errmsg);
  
  const char* text = &buffer[0];
  
  // Draw the string..   
  byte ch = *text;
  while(ch != 0) {
    byte screenCode = 0x20;
    if (ch >= 'A' && ch <= 'Z') {
      screenCode = ch - 'A' + 1;
    }
    if (ch >= 'a' && ch <= 'z') {
      screenCode = ch - 'a' + 1;
    }
    if (ch >= '0' && ch <= '9') {
      screenCode = 48 + (ch-'0');
    }
    if (ch == '.')
      screenCode = 46;
    if (ch == '!')
      screenCode = 33;
    if (ch == ':')
      screenCode = 58;
    if (ch == ';')
      screenCode = 59;
    if (ch == '=')
      screenCode = 61;    
    if (ch == ',')
      screenCode = 44;    
    if (ch == '\'')
      screenCode = 0x27;  
      
        
    drawAtCursor(screenCode);
    ch = *++text;
  }
  
  // Linebreak:
  cursorX=0;
  advanceCursorY();
}


