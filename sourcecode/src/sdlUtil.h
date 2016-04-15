#ifndef SDLUTIL_H
#define SDLUTIL_H

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "math/primitives.h"

// status flag for my SDL library
extern bool runningSDL;
extern int heightSDL;
extern int widthSDL;

// SDL utility functions:
void initSDL2D(int width, int height, bool fullscreen=false);
void closeSDL();
void handleInputSDL();
void displaySDL();
void clearSDL(COLOR color);
void drawImage2D(int xpos, int ypos, int width, int height, BYTE *image);


#endif

