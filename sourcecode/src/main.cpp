// *********************************************************
// **** Default Mingw32 app with SDL, STLport, Logger   ****
// **** and other useful utility functions              ****
// ********************************************************* 
#include <time.h>
#include "sdlUtil.h" 
#include "vic20/vic20screen.h"
#include "utility/Logger.h"
#include "utility/os.h"
#include "game.h"


// Simple input handler
void handleInput() {
  SDL_Event event;
  // Here we handle all events
  while (SDL_PollEvent(&event) != 0) {
    if (event.type ==  SDL_KEYDOWN) {
      switch(event.key.keysym.sym) {
        case SDLK_q: 
        case SDLK_ESCAPE:
	        closeSDL();
	       break;
	      case SDLK_UP:
        case SDLK_RETURN:
	       Game::getInstance()->stepForward();
	       break;
        case SDLK_DOWN:
	       Game::getInstance()->stepBackwards();
	       break;
        case SDLK_LEFT:
         Game::getInstance()->turnLeft();
	       break;
	      case SDLK_RIGHT:
	       Game::getInstance()->turnRight();
	       break;
        case SDLK_z: // ready position
	       Game::getInstance()->setCombatStance(0);
	       break;
	      case SDLK_x: // Counterthrust movement:
	       Game::getInstance()->setCombatStance(1);
	       break;
	      case SDLK_c: // Thrust movement:
	       Game::getInstance()->setCombatStance(2);
	       break;
	      case SDLK_s: // sleep spell
	       Game::getInstance()->useSpell(0);
	       break;
	      case SDLK_b: // Blast spell:
	       Game::getInstance()->useSpell(2);
	       break;
	      case SDLK_t: // Teleport spell:
	       Game::getInstance()->useSpell(1);
	       break;
	      default: break;
	    }
    }
  }
}


int main(int argc, char** argv) {
  srand(time(NULL));
  
  // Create the log:
  logger = new Logger("mtdrash.log");
  
  // SDL - show graphics in window
  // change 'true' to 'false' to run in a windo
  initSDL2D(320,200, true);
     
  SDL_ShowCursor(false);   
     
  // Set BG and border color 
  byte bgColor = (93>>4)&15;
  byte borderColor = 93 & 7;
  Vic20Screen::getInstance()->clearBorder(borderColor);
  Vic20Screen::getInstance()->setBGColor(bgColor);
   
            
  // Show the title screen:
  
  Game::getInstance()->showTitleScreen();
  displaySDL(); 
 
  // Wait a while:  
  SDL_Delay(4000); 
  
   
  // Reset the game:  
  Game::getInstance()->reset();
 
  dword lastFrameMove = getTime();
  while(runningSDL && Game::getInstance()->running()) {
    
    handleInput();
      
    dword now = getTime();
    if (now - lastFrameMove > 1000/25) {  
      // Limit this to 25fps to emulate the IRQ   
      Game::getInstance()->frameMove();
      lastFrameMove = now;
    }
   
    // Render the game:  
    Game::getInstance()->frameRender(); 
    displaySDL(); 
  }
      
  // Quit
  return 1;
}

