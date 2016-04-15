#include "os.h"
#include <SDL/SDL.h>

dword startTime;
bool startTimed = false;

dword getTime() {
  if (!startTimed) {
    startTime = SDL_GetTicks();
    startTimed = true;
  }
  return SDL_GetTicks() - startTime;
}




