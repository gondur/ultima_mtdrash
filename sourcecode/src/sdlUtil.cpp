#include "sdlUtil.h"
#include "utility/Logger.h"

bool runningSDL = false;
int widthSDL = 0;
int heightSDL = 0;

// Initialize SDL to a given screen mode 
void initSDL2D(int width, int height, bool fullscreen) {
 if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
    Log("Unable to init SDL: %s",SDL_GetError());
    exit(1);
 }

 SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

 int flags = SDL_OPENGL | SDL_HWSURFACE;
 if (fullscreen)
   flags |= SDL_FULLSCREEN;
 if (SDL_SetVideoMode(width, height, 32, flags) == 0 ) {
   Log("Set video mode failed: %s",SDL_GetError());
   exit(1);
 } 
 
 // Reset viewport
 glViewport(0, 0, width, height);

 // Setup 2D projection
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 gluOrtho2D(0, width, 0, height);

 // Reset modelview
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity(); 
 
 // SDL is now running
 runningSDL = true;
 widthSDL = width;
 heightSDL = height;
}  

void closeSDL() {
  SDL_Quit();
  runningSDL = false;
}



void handleInputSDL() {
  SDL_Event event;
  // Here we handle all events
  while (SDL_PollEvent(&event) != 0) {
    if (event.type ==  SDL_KEYDOWN) {
      switch(event.key.keysym.sym) {
	      case SDLK_q: {
	        closeSDL();
	      } break;
	      default: break;
	    }
    }
  }
}



void displaySDL() {
 if (runningSDL)
  SDL_GL_SwapBuffers(); 
}


void clearSDL(COLOR color) {
 if (runningSDL) {
   float r = (float)color.r / 255.0f;
   float g = (float)color.g / 255.0f;
   float b = (float)color.b / 255.0f;
   float a = (float)color.a / 255.0f;
  
   glClearColor(r, g, b, a);
   glClearDepth(1.0);

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
 } 
}


void drawImage2D(int xpos, int ypos, int width, int height, BYTE *image) {
 if (runningSDL) {
  glRasterPos2i(xpos, ypos);
  glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, image);
 }
}

