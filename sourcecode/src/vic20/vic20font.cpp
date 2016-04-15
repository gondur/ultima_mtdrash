#include "vic20font.h"
#include "utility/Logger.h"
#include "utility/FileIOObject.h"
#include "sdlUtil.h"


// default vice palette
byte vic20_palette[16*3] =
{
   0x00, 0x00, 0x00, // Black       0
   0xFD, 0xFE, 0xFC, // White       1
   0xBE, 0x1A, 0x24, // Red         2
   0x30, 0xE6, 0xC6, // Cyan        3
   0xB4, 0x1A, 0xE2, // Purple      4
   0x1F, 0xD2, 0x1E, // Green       5
   0x21, 0x1B, 0xAE, // Blue        6
   0xDF, 0xF6, 0x0A, // Yellow      7 
   0xB8, 0x41, 0x04, // Orange      8
   0x6A, 0x33, 0x04, // Brown       9
   0xFE, 0x4A, 0x57, // Light Red   10
   0x42, 0x45, 0x40, // Dark Gray   11
   0x70, 0x74, 0x6F, // Medium Gray 12 
   0x59, 0xFE, 0x59, // Light Green 13
   0x5F, 0x53, 0xFE, // Light Blue  14
   0xA4, 0xA7, 0xA2, // Light Gray  15
};


Vic20Font::Vic20Font(const string& filename) : charset(NULL) {
  Log("loading the vic20 font from '%s'", filename.c_str());
  
  // workspace - can hold bitmap image for one character in format RGBA
  character = new byte[8*8*4];
  
  vertDisplacement = 0;
  
  FileIOObject* in = new FileIOObject(filename);
  if (in->isValid()) {
    Log("found font file, now parsing..");
    charset = new byte[8*8*256];
    memset(charset,0,8*8*256);
    
    // Load each character:
    for(unsigned int ch=0; ch<256; ch++) {
      for(unsigned int y=0; y<8; y++) {
        // gets the current byte which holds a pixel line with 8 pixels
        unsigned char linebyte = in->getByte();

        for(unsigned int x=0; x<8; x++) {
          // is the topmost bit set?
          bool bit = (linebyte & 0x80) == 0x80;

          unsigned int destpos = ch*8*8 + (y*8+x);
          charset[destpos] = bit ? 1 : 0;

          // shifts to the next bit
          linebyte <<= 1;
        }
      }
    }
    Log("charset loaded...");
  }
  else {
    Log("ERROR - couldn't open vic20 font file '%s'", filename.c_str()); 
  }
  
  delete in;
}

Vic20Font::~Vic20Font() {
  delete charset;
  delete character;
}


// Draw the image for one character

// xpos,ypos is in the VIC20 screen space (22x23)
// X is right, Y down
void Vic20Font::drawCharacter(int xpos, int ypos, byte nr, byte color, byte bgColor) {
  // Build the image:
  for (int i=0; i<8; i++) {
    for (int j=0; j<8; j++) {
       byte r = vic20_palette[bgColor*3 + 0];
       byte g = vic20_palette[bgColor*3 + 1];
       byte b = vic20_palette[bgColor*3 + 2];
       byte a = 255; 
            
       // if the bit set in the font?
       if (charset[nr*64 + j*8 + i] == 1) {
         r = vic20_palette[color*3 + 0];
         g = vic20_palette[color*3 + 1];
         b = vic20_palette[color*3 + 2];
       }
       
       // set the color: (flip upside-down - 2D graphics have y up)
       character[(7-j)*(8*4) + i*4 + 0] = r;
       character[(7-j)*(8*4) + i*4 + 1] = g;
       character[(7-j)*(8*4) + i*4 + 2] = b;
       character[(7-j)*(8*4) + i*4 + 3] = a;
    } 
  }
  
  // Draw it:
  int beginX = (widthSDL - (22*8)) / 2;
  int beginY = (heightSDL - (23*8)) / 2;
  
  int drawX = beginX + xpos*8;
  int drawY = heightSDL - (ypos*8 + beginY) - 8;
  
  
  drawImage2D(drawX, drawY + vertDisplacement, 8, 8, character); 
}

