#include "game.h"
#include "vic20/vic20Screen.h"
#include "utility/Logger.h"
#include "utility/FileIOObject.h"
#include "sdlUtil.h"

Game* Game::instance = NULL;
const int mapSize = 13;


Game* Game::getInstance() {
  if (instance == NULL)
    instance = new Game();
    
  return instance; 
}

Game::Game() {
  maze = new byte[mapSize*mapSize];
  
  // Fill the maze to be solid:
  memset(maze, 1, mapSize*mapSize); 
 
  // Load game data:
  loadStrings("introstrings.pet", &introStrings); 
  loadStrings("monsterstrings.pet", &monsterStrings);
  loadStrings("gameviewstrings.pet", &gameviewStrings);
  
  // Set strings:
  rankings[0] = "qwimby";
  rankings[1] = "corporal";
  rankings[2] = "cadet";
  rankings[3] = "captain";
  rankings[4] = "colonel";
  rankings[5] = "questor";
  
  directions[0] = "north";
  directions[1] = "east";
  directions[2] = "south";
  directions[3] = "west";
    
  monsterNames[0] = "gremlin!!";
  monsterNames[1] = "floating orb!!";
  monsterNames[2] = "dancing demon!!";
  monsterNames[3] = "phantom!!";
  monsterNames[4] = "purple slime!!";
  
  dx[0] = 0;
  dx[1] = 1;
  dx[2] = 0;
  dx[3] = -1;
  
  dy[0] = -1;
  dy[1] = 0;
  dy[2] = 1;
  dy[3] = 0;
  
  gateScreenCode = 0x66;
  lastGateRotateTime = 0;
  
}


Game::~Game() {
  delete maze;
  
}


// Load in a set of strings from a .pet file
void Game::loadStrings(const string& fileName, List<string>* result) {
  FileIOObject* in = new FileIOObject(fileName);
  if (in->isValid()) {
    // How many strings?
    in->setAbsoluteFilePos(in->getFileLength() - 4);
    int nrStrings = in->getInt();
    in->setAbsoluteFilePos(0);
      
    for (int i=0; i<nrStrings; i++) {
      int nrChars = in->getInt();
      string str;
      for (int c=0; c<nrChars; c++) {
        byte ch = in->getByte();
        str.append(1,(char)ch); 
      }  
      result->Add(str); 
    }
  }
  else {
   Log("ERROR - couldn't find string file '%s'", fileName.c_str()); 
  }
  delete in;
}


void Game::reset() {
  gameOver = false;
  hitpoints = 3;
  initializeLevel(1);
  
  spellsLeft = 3;      // Give player 3 spells.
  sleepTimeLeft = 0;   // reset sleep counter
  screenShaking = false;
  screenShakeEndTime = 0;  
    
  // Show the 'prepare' screen:
  Vic20Screen::getInstance()->setFGColor(WHITE);
  Vic20Screen::getInstance()->setBGColor(BLACK);
  Vic20Screen::getInstance()->clear();
 
  // Draw info:
  Vic20Screen::getInstance()->setCursorPos(3, 5);
  Vic20Screen::getInstance()->drawPCASCIIText("Make ready for");
  Vic20Screen::getInstance()->setCursorPos(1, 7);
  Vic20Screen::getInstance()->drawPCASCIIText("Ultima: Escape from");
  Vic20Screen::getInstance()->setCursorPos(6, 9);
  Vic20Screen::getInstance()->drawPCASCIIText("Mount Drash");
  
  Vic20Screen::getInstance()->setCursorPos(4, 17);
  Vic20Screen::getInstance()->drawPCASCIIText("PC Version by:");
  Vic20Screen::getInstance()->setCursorPos(4, 19);
  Vic20Screen::getInstance()->drawPCASCIIText("Kasper Fauerby");
  Vic20Screen::getInstance()->setCursorPos(9, 21);
  Vic20Screen::getInstance()->drawPCASCIIText("2003");
 
  
  // Flip the vic screen to SDL
  Vic20Screen::getInstance()->draw(); 
  displaySDL(); 
  
  // Delay:
  SDL_Delay(4000);  
}
 
bool Game::running() {
  return !gameOver; 
}

void Game::spawnMonsters() {
  for (int i=0; i<numMonsters; i++) {
    monsterX[i] = (rand()%5)*2+2; // random between 2 and 10
    monsterY[i] = (rand()%5)*2+2;
    
    // Don't allow a spawn on top of the exit
    if (monsterX[i] == 2 && monsterY[i] == 2) {
      monsterX[i] = 4;
    } 
  }
}


void Game::generateMaze() {
  // Clear the interior of the maze:
  for (int i=1; i<mapSize-1; i++) {
    for (int j=1; j<mapSize-1; j++) {
      maze[j*mapSize + i] = 0;     
    } 
  }
  
  // Check if we need to do special initialization:
  if (level == 15) {
    for (int i=2; i<11; i+=2) {
      for (int j=2; j<11; j+=2) {
        maze[j*mapSize+i] = 1;      
      } 
    }

    maze[5*mapSize+2] = 1;
    maze[9*mapSize+2] = 1;
    maze[3*mapSize+10] = 1;
    maze[7*mapSize+10] = 1;
  }
  else {
     // randomize maze (algorithm from the basic code)
     int px = 4;
     int py = 6;
     bool newPosNeeded = false;
     bool mazeFinished = false;
     maze[py*mapSize + px] = 1;
      
     while (!mazeFinished) {
       
       // need to flag if we take the branch below or not..
       bool lessThan4 = false;
       
       if (maze[py*mapSize + (px-2)] + maze[py*mapSize + (px+2)] + maze[(py-2)*mapSize + px] + maze[(py+2)*mapSize + px] < 4) {
         lessThan4 = true;
         int f;
         do {
           f = rand()%4;
         } while(maze[(py+2*dy[f])*mapSize + (px + 2*dx[f])] == 1);
          
         // Fill neighbor square 
         maze[(py+dy[f])*mapSize + (px+dx[f])] = 1;
         
         // Move generation point
         px += 2*dx[f];
         py += 2*dy[f];    
         
         // Fill this spot out too..
         maze[py*mapSize + px] = 1; 
         
         // flag that a new position is needed
         newPosNeeded = true;
         
       }
       
       if (!lessThan4 || randFloat(0.0, 1.0) > 0.8) {
         // new generation point..
         if (newPosNeeded) {
           px=0;
           py=2;
           newPosNeeded = false;
         }  

         // advance generation position:
         do {
           px+=2;
           
           if (px > 10) {
             px=2;
             py+=2;
             if (py>10) 
               mazeFinished = true;
           }
         } while(!mazeFinished && maze[py*mapSize + px] != 1);
       }
     }
     
     // maybe spawn gems:
     if (level >= 5) {
       maze[10*mapSize+2] = 3;
       maze[2*mapSize+10] = 3;
     }
  } // if not level 15
}



// Sets up the maze, spawns the monsters etc for the given level
void Game::initializeLevel(byte lvl) {
  level = lvl;
  rank = (lvl-1)/3; 
  heading = 0;
  playerX = 10;
  playerY = 10;
  timeLeft = 100;
  gems = 0;
  lastSecondTime = getTime();
  numMonsters = (level+1)/2 + 1;
    
  combatMode = false;
  creatureNr = 0;  
    
  // setup the maze array
  generateMaze();
  maze[playerY*mapSize + playerX] = 2; // Set player position on map
  maze[2*mapSize+2] = 4;               // Set the exit space
  
  // spawn monsters into the map
  spawnMonsters();
}  


void Game::showWinScreen() {
  Vic20Screen::getInstance()->setFGColor(WHITE);
  Vic20Screen::getInstance()->setBGColor(BLACK);
  Vic20Screen::getInstance()->clear();
  
  Vic20Screen::getInstance()->setCursorPos(3, 6);
  Vic20Screen::getInstance()->drawPCASCIIText("you have gained");
  Vic20Screen::getInstance()->setCursorPos(0, 8);
  Vic20Screen::getInstance()->drawPCASCIIText("your freedom and truly");
  Vic20Screen::getInstance()->setCursorPos(3, 10);
  Vic20Screen::getInstance()->drawPCASCIIText("earned the title:"); 
  
  Vic20Screen::getInstance()->setCursorPos(7, 15);
  Vic20Screen::getInstance()->drawPCASCIIText("questor!"); 
  
  
  // Flip the vic screen to SDL
  Vic20Screen::getInstance()->draw(); 
  displaySDL(); 
  
  // Delay:
  SDL_Delay(5000); 
    
  // swipe:
  Vic20Screen::getInstance()->setFGColor(GREEN);
  Vic20Screen::getInstance()->setBGColor(GREEN);
  
  for (int i=21; i>=0; i--) {
    for (int j=0; j<23; j++)
      Vic20Screen::getInstance()->drawAtPos(i, j, 0x20);  
    
    // Flip the vic screen to SDL
    Vic20Screen::getInstance()->draw(); 
    displaySDL(); 
  
    // Delay:
    SDL_Delay(10); 
  }
}

// This message is shown in the 3d view when changing to a lower level
void Game::showEnteringLevelMessage(byte level) {
  // Clear 3d view: 
  Vic20Screen::getInstance()->setFGColor(WHITE);
  Vic20Screen::getInstance()->setBGColor(BLACK);
  for (int i=0; i<10; i++) {
    for (int j=0; j<11; j++) {
      Vic20Screen::getInstance()->drawAtPos(i, j, 0x20);       
    }
  }
  
  Vic20Screen::getInstance()->setCursorPos(1, 4);
  Vic20Screen::getInstance()->drawPCASCIIText("Entering");
  Vic20Screen::getInstance()->setCursorPos(1, 6);
  Vic20Screen::getInstance()->drawPCASCIIText("Level %i", level);
  
  // Flip the vic screen to SDL
  Vic20Screen::getInstance()->draw(); 
  displaySDL(); 
  
  // Delay:
  SDL_Delay(2000); 
} 


// Show an info string where Level is normally shown
void Game::showInfoString(const string& message, dword time) {
   Vic20Screen::getInstance()->setBGColor(BLACK);
   Vic20Screen::getInstance()->setFGColor(WHITE);
    
   // Clear out info area:
    for (int i=0; i<22; i++)
      Vic20Screen::getInstance()->drawAtPos(i, 22, 0x20, BLACK);     
  
   // Draw info:
   Vic20Screen::getInstance()->setCursorPos(0, 22);
   Vic20Screen::getInstance()->drawPCASCIIText("%s", message.c_str());
  
   
   // Flip the vic screen to SDL
   Vic20Screen::getInstance()->draw(); 
   displaySDL(); 
  
   // Delay:
   SDL_Delay(time); 
}


// Advance AI in combat
void Game::updateCombatAI() {
  dword now = getTime();
  if (now - lastCombatTime >= (1000-level*25)) {
    lastCombatTime = now;
    
    // Time to update monster:
    int distance = monsterCombatX-playerCombatX-6;
    
    if (playerCombatStance == 2)
      distance = distance-1;
    
    // if distance < 0 and monster is in attack position it scores a hit:
    if (distance < 0 && monsterCombatStance == 1) {
      // if player just have 1 point left he'll die
      if (hitpoints == 1) {
         showInfoString("    you lose, cadet", 2000);
         reset();
         return;
      }  
        
      // subtract 1 HP
      hitpoints--;
        
      // Print how many times he has been hit. 2 points left means 1 hit, else two hits..
      string howMany = "twice";
      if (hitpoints == 2)
        howMany = "once";  
            
      monsterX[creatureNr] = 0;
      combatMode = false;
      showInfoString("you've died " + howMany, 2000);
      return;
    }
    
    // If distance is 3 and player is in 'Thrust movement' he kills monster
    if (distance == 3 && playerCombatStance == 2) {
      monsterX[creatureNr] = 0; // flag as dead
      combatMode = false;
      showInfoString("      it's dead!     ", 2000);
      return;
    }
    
    // Set random monster stance. The closer, the more likely it will attack
    monsterCombatStance=0;
    if ((rand()%9) > distance)     
      monsterCombatStance=1;
      
    // advance monster
    monsterCombatX = monsterCombatX-1;
    
    // If player is in ready position and monster is close it is pushed 2 squares back
    if (distance < 2 && playerCombatStance == 0 && monsterCombatX<18)
      monsterCombatX += 2;
    
    // If distance is 2 or 3 and player is counter-thrust it is pushed 2 squares back
    if (distance > 1 && distance < 4 && monsterCombatX<18)
      monsterCombatX += 2;

    // If distance is 4 and player is in thrus movement it is pushed 2 squares back  
    if (distance == 4 && playerCombatStance == 2 && monsterCombatX<18)
      monsterCombatX += 2;

  }
}
  
  
// Advance the time with 1/25 sec  
void Game::frameMove() {
  // Check if full second has passed
  dword now = getTime();
  if (now-lastSecondTime >= 1000) {
    timeLeft--;
    lastSecondTime = now; 
  }
   
  // Rotate exit gate colors: (not correct but hey..)
  if (now-lastGateRotateTime >= 100) {
    lastGateRotateTime = now;
    gateBGColor = CYAN;
    gateFGColor = 4 + (rand()%5);
  }
      
  // Check time: running out?
  if (timeLeft == 0) {
    Vic20Screen::getInstance()->setBGColor(BLACK);
    Vic20Screen::getInstance()->setFGColor(WHITE);
    Vic20Screen::getInstance()->setCursorPos(6, 10);
    Vic20Screen::getInstance()->drawPCASCIIText("time"); 
    showInfoString("    you lose, cadet", 2000);
    
    // restart game:
    reset();
  }
  
  // Update combat AI:
  if (combatMode) {
    updateCombatAI(); 
  }  
}



// Draw the title screen.. just a bunch of prints:
void Game::showTitleScreen() {
  // Clear the vic20 screen:
  Vic20Screen::getInstance()->clear();
 
  // Draw the intro screen:
  for (int i=0; i<introStrings.size; i++) {
    string str = introStrings[i];
    Vic20Screen::getInstance()->drawPETASCIIString(str);  
  }
 
  // Flip the vic screen to SDL
  Vic20Screen::getInstance()->draw();
}


// Draw overhead map, monsters, gems and player
void Game::drawMap() {
  
  Vic20Screen::getInstance()->setFGColor(WHITE);
  
    
  // At level 7 the overhead map disappears... TODO: should player still be seen?
  if (level >= 7) {
    for (int i=0; i<11; i++) {
      for (int j=0; j<11; j++) {
        Vic20Screen::getInstance()->setBGColor(RED); 
        Vic20Screen::getInstance()->drawAtPos(11+i, j, 0x20); 
      }
    }
  }
  else {
    // Draw map: 
    for (int i=0; i<11; i++) {
      for (int j=0; j<11; j++) {
        // Draw the maze (TODO - use visibility map)
        switch (maze[(j+1)*mapSize + (i+1)]) {
          case 0: // solid wall
          case 1: // hidden corridor
           Vic20Screen::getInstance()->setBGColor(RED); 
           Vic20Screen::getInstance()->drawAtPos(11+i, j, 0x20);  
           break;
          case 2: // seen corridor
           Vic20Screen::getInstance()->setBGColor(BLACK); 
           Vic20Screen::getInstance()->drawAtPos(11+i, j, 0x20); 
           break;
          case 3: // gem
           Vic20Screen::getInstance()->setBGColor(BLACK);
           Vic20Screen::getInstance()->setFGColor(YELLOW);
           Vic20Screen::getInstance()->drawAtPos(11+i, j, 0x5a);  
           break;
          case 4: // exit
           Vic20Screen::getInstance()->setBGColor(gateBGColor);
           Vic20Screen::getInstance()->setFGColor(gateFGColor);
           Vic20Screen::getInstance()->drawAtPos(11+i, j, gateScreenCode);  
           break; 
        }
      } 
    }
  }
  
  // Draw player:
  Vic20Screen::getInstance()->setBGColor(BLACK); 
  Vic20Screen::getInstance()->setFGColor(BLUE);
  Vic20Screen::getInstance()->drawAtPos(10+playerX, playerY-1, 0x57); 
  
  
  // At level 9 the monsters are no longer shown
  if (level < 9) {
    // draw the monsters:
    Vic20Screen::getInstance()->setBGColor(BLACK);
    Vic20Screen::getInstance()->setFGColor(RED);
    for (int i=0; i<numMonsters; i++) {
      if (monsterX[i] != 0)
        Vic20Screen::getInstance()->drawAtPos(10+monsterX[i], monsterY[i]-1, 0x56); 
    }
  }   
} 


// Draw graphics for a single tile in the 3d view.
void Game::draw3dTile(byte a, byte b, byte lr, byte distance) {
  byte s = maze[b*mapSize+a];
  
  // flag corridor as seen
  if (s == 1)  
    maze[b*mapSize+a]=2;
    
  // clamp to 1 for calculation below:  
  if (s > 1)
    s = 1; 
          
  // Calculate string to draw:
  byte index = 1 + distance + lr*3 + s*6;
  if (index >= 1 && index <= 12) 
    Vic20Screen::getInstance()->drawPETASCIIString(gameviewStrings[index-1]);
}


// Draw the 3d view
void Game::draw3dView() {
  // Clear 3d view area to black:
  Vic20Screen::getInstance()->setFGColor(WHITE);
  Vic20Screen::getInstance()->setBGColor(BLACK);
  for (int i=0; i<10; i++) {
    for (int j=0; j<11; j++) {
      Vic20Screen::getInstance()->drawAtPos(i, j, 0x20);       
    }
  }
 
  
  // Draw the 3d view:
  int curdx = dx[heading];
  int curdy = dy[heading];
 
  
  // draw 3 steps ahead (or until a wall is in front):
  for (int di=0; di<3; di++) {
    // calculate position
    byte a = playerX + di * curdx;
    byte b = playerY + di * curdy; 
  
    // status of the tile:
    byte s = maze[b*mapSize + a];  
    
    // if a corridor player has seen that now 
    if (s == 1)  
      maze[b*mapSize+a]=2;
          
    
    // if wall solid:
    if (s == 0) {  
      // Draw graphics
      if (di == 1) {
        Vic20Screen::getInstance()->drawPETASCIIString(gameviewStrings[12]);
        //Vic20Screen::getInstance()->drawPETASCIIString(gameviewStrings[13]);
        
        // Patch up. For some reason a gap appears at the bottom of this solid wall. Str 13 above should do this
        Vic20Screen::getInstance()->setCursorPos(2,7);
        Vic20Screen::getInstance()->setBGColor(BLUE);
        for (int p=0; p<6; p++) {
          Vic20Screen::getInstance()->drawAtCursor(0x20);
        }
      }
      if (di == 2) {
        Vic20Screen::getInstance()->drawPETASCIIString(gameviewStrings[14]);
      }
      
      return; // Dont draw any more tiles
    }  
        
    
    // more general 3d drawing code:
    if (heading == 0 || heading == 2) {
      // looking north or south: 
      
      // Tile 1:
      a = playerX + curdy;
      b = playerY + di*curdy;
      draw3dTile(a, b, 0, di);
      
      // Tile 2:
      a = playerX-curdy;
      draw3dTile(a, b, 1, di);
    }
    else {
      // looking east or west:
      
      // Tile 1:
      b = playerY-curdx;
      draw3dTile(a, b, 0, di);
                    
      // Tile 2:
      b = playerY+curdx;
      draw3dTile(a, b, 1, di);
    }
  }
} 



// Draw heading and time left. 
void Game::drawHeadingAndTime() {
  Vic20Screen::getInstance()->setFGColor(WHITE);
  Vic20Screen::getInstance()->setBGColor(BLACK);
  
  // Draw heading:
  if (level < 11) {
    Vic20Screen::getInstance()->setCursorPos(0, 10);
    Vic20Screen::getInstance()->drawPCASCIIText("%s", directions[heading].c_str());
  }
  
  // Draw time left:
  int width = 3;
  if (timeLeft < 100)
   width = 2;
  if (timeLeft < 10)
   width = 1;
   
  Vic20Screen::getInstance()->setCursorPos(10-width, 10);
  Vic20Screen::getInstance()->drawPCASCIIText("%i", timeLeft); 
}



// Draw the current level in the info bar
void Game::drawLevel() {
  Vic20Screen::getInstance()->setBGColor(BLACK);
  Vic20Screen::getInstance()->setFGColor(WHITE);
    
  // Clear out level area:
  for (int i=0; i<22; i++)
    Vic20Screen::getInstance()->drawAtPos(i, 22, 0x20, BLACK);     
  
  // Draw level:
  Vic20Screen::getInstance()->setCursorPos(0, 22);
  Vic20Screen::getInstance()->drawPCASCIIText("Level %i", level);
}


// Draw the rankings
void Game::drawRanking() {
  Vic20Screen::getInstance()->setBGColor(BLACK);
  Vic20Screen::getInstance()->setFGColor(WHITE);  
  
  // Clear out the ranking area:
  for (int i=0; i<22; i++) {
    for (int j=0; j<9; j++) {
      Vic20Screen::getInstance()->drawAtPos(i, 12+j, 0x20);     
    } 
  } 
  
  // Draw the 6 rankings:
  Vic20Screen::getInstance()->setCursorPos(1, 13);
  Vic20Screen::getInstance()->drawPCASCIIText("Rankings:");
  for (int r=0; r<6; r++) {
    if (r == rank) {
      Vic20Screen::getInstance()->setFGColor(WHITE); 
    } 
    else {
      Vic20Screen::getInstance()->setFGColor(PURPLE); 
    }
    Vic20Screen::getInstance()->setCursorPos(7, 14+r);
    Vic20Screen::getInstance()->drawPCASCIIText("%s", rankings[r].c_str());
  }  
}



// Draw combat screen
void Game::drawCombatScreen() {
  Vic20Screen::getInstance()->setBGColor(BLACK);
  Vic20Screen::getInstance()->setFGColor(WHITE);  
  
  // Clear out combat area:
  for (int i=0; i<22; i++) {
    for (int j=0; j<9; j++) {
      Vic20Screen::getInstance()->drawAtPos(i, 12+j, 0x20);     
    } 
  }  
      
  // Draw player:
  Vic20Screen::getInstance()->setCursorPos(playerCombatX, 15);
  Vic20Screen::getInstance()->drawPETASCIIString(monsterStrings[playerCombatStance]); 

  // Draw monster:
  Vic20Screen::getInstance()->setCursorPos(monsterCombatX, 14);
  Vic20Screen::getInstance()->drawPETASCIIString(monsterStrings[3 + creatureType*2 + monsterCombatStance]);
}


// Draw monster name in info-bar
void Game::drawMonsterName() {
  Vic20Screen::getInstance()->setBGColor(BLACK);
  Vic20Screen::getInstance()->setFGColor(WHITE);
    
  // Clear out level area:
  for (int i=0; i<22; i++)
    Vic20Screen::getInstance()->drawAtPos(i, 22, 0x20, BLACK);     
  
  // Draw monster name (centered):
  int nameLength = strlen(monsterNames[creatureType].c_str());
  int xBegin = (21 - nameLength)/2;
  
  Vic20Screen::getInstance()->setCursorPos(xBegin, 22);
  Vic20Screen::getInstance()->drawPCASCIIText("a %s", monsterNames[creatureType].c_str());
}




// Render the graphics
void Game::frameRender() {
  // Clear the vic screen
  Vic20Screen::getInstance()->setBGColor(GREEN);
  Vic20Screen::getInstance()->setFGColor(WHITE);
  Vic20Screen::getInstance()->clear();
   
    
  if (combatMode) {
    drawCombatScreen();
    drawMonsterName();
  }
  else {
    drawRanking();
    drawLevel();
  }
  
  draw3dView();
  drawHeadingAndTime();
  drawMap();
 
  // Draw hitpoints:
  Vic20Screen::getInstance()->setBGColor(GREEN);
  Vic20Screen::getInstance()->setFGColor(BLACK);
  Vic20Screen::getInstance()->setCursorPos(9, 11);
  for (int i=0; i<hitpoints; i++)
    Vic20Screen::getInstance()->drawAtCursor(87);
  
  // Handle shaking of screen (if nessesary)
  int displacement = 0;
  if (getTime() > screenShakeEndTime) {
    screenShaking = false;
    Vic20Screen::getInstance()->clearBorder(5);
  }  
    
  if (screenShaking) 
    displacement = rand()%12 - 6; 
        
  // Flip the vic screen to SDL
  Vic20Screen::getInstance()->draw(displacement); 
}



// For player movement - request a move to a certain position on the map.
// Result depends on the target tile type
void Game::requestMoveTo(byte x, byte y) {
  byte s = maze[y*mapSize + x];
  
  switch (s) {
   case 0:
    // solid wall, cant move
    break; 
   case 1:
   case 2:
    // corridor. Move player:
    playerX = x;
    playerY = y;
    maze[y*mapSize + x] = 2;
    break; 
   case 3:
    // Pick up gem, move player, set as seen corridor 
    gems++;
    playerX = x;
    playerY = y;
    maze[y*mapSize + x] = 2;
    break; 
   case 4:
    // exit square:
    if (level < 5 || (level < 13 && gems > 0) || gems == 2 || level == 15) {
      // Wining the game:
      if (level == 15) {
        showWinScreen();
        reset(); 
      }
      else {
        // Change level:
        showEnteringLevelMessage(level+1);
        initializeLevel(level+1); 
      }
    }
    break; 
  }
}


// Player input:
void Game::stepBackwards() {
  if (combatMode) return;
  
  byte newX = playerX - dx[heading];
  byte newY = playerY - dy[heading];
  requestMoveTo(newX,newY);
  updateMonsterPositions();
}

void Game::stepForward() {
  if (combatMode) return;

  byte newX = playerX + dx[heading];
  byte newY = playerY + dy[heading];
  requestMoveTo(newX,newY);
  updateMonsterPositions();
}

void Game::turnLeft() {
  // in combat mode this key is used for retreat
  if (combatMode) {
    playerCombatStance = 1;
    if (playerCombatX > 2)
       playerCombatX--;
    return;
  }

  if (heading == 0)
    heading = 3;
  else
    heading--;
  updateMonsterPositions();  
}

void Game::turnRight() {
  // in combat mode this key is used for advance
  if (combatMode) {
    playerCombatStance = 1;
    if (playerCombatX < 13)
       playerCombatX++;
    return;
  }

  heading = (heading+1)%4; 
  updateMonsterPositions(); 
}



// Update monster positions every time player moves (takes a turn)
void Game::updateMonsterPositions() {
  // Are monsters sleeping? (from a spell)
  if (sleepTimeLeft > 0) {
     sleepTimeLeft--;
     return; 
  }
  
  for (int i=0; i<numMonsters; i++) {
     // if monster isn't dead 
     if (monsterX[i] != 0) {
       int mx = monsterX[i];
       int my = monsterY[i];
     
       // The BASIC SGN command on (playerX-mx) and (playerY-my)
       int px = 0;
       if (playerX-mx < 0)
         px = -1;
       if (playerX-mx > 0)
         px = 1;  
       
       int py = 0;
       if (playerY-my < 0)
        py = -1;
       if (playerY-my > 0)
        py = 1;
       
       
       // check if combat mode should be activated:
       if (px == 0 && py == 0) {
         beginCombat(i);
         return;
       }
      
       // go towards player 
       if (px == 0) {
         byte v= maze[(my+py)*mapSize + mx];
         if (v == 1 || v == 2)
           my = my+py;
       }
       else {
         byte v= maze[my*mapSize+(mx+px)];
         if (v==1 || v==2) 
           mx = mx+px;
       }
      
       // Set new position:
       monsterX[i] = mx;
       monsterY[i] = my;
      
       // Check combat again:
       if (mx == playerX && my == playerY) {
         beginCombat(i);
         return;
       }
     }
   }
}  


void Game::beginCombat(byte foe) {
  combatMode = true;
  creatureNr = foe;
         
  // find creature type:
  int mt;
  do {
    mt = int( randFloat(0.0, 1.0f)*numMonsters+2)*2-1;
  } while(mt > 11);
  creatureType = ((mt-3)/2)%5;
  
  // Init combat positions and stances.     
  playerCombatX = 2;        
  playerCombatStance = 1;       
  
  monsterCombatX = 18;           
  monsterCombatStance = 0;   
  
  // init the combat timer
  lastCombatTime = getTime();
 }



// 0 == sleep
// 1 == teleport
// 2 == blast
void Game::useSpell(byte spellnr) {
  if (spellsLeft == 0 && level < 15)
    return;
  
  // Unless we are at level 15 we count the spells
  if (level < 15)  
    spellsLeft--;
  
  switch (spellnr) {
   case 0:
    sleepTimeLeft = 3; 
    break;
   case 1: {
    int newX = (rand()%4)*2+4;
    int newY = (rand()%6)*2+2;
    if (newX > 1 && newX<11 && newY > 1 && newY < 11) 
      requestMoveTo(newX, newY);  
   } break; 
   case 2: {
     byte a = playerX + dx[heading];
     byte b = playerY + dy[heading];
     if ((playerX%2)==0 && (playerY%2)==0 && a > 1 && a<11 && b>1 && b<11) {
       maze[b*mapSize+a] = 1;
       screenShaking = true;
       screenShakeEndTime = getTime() + 500;
     }
   } break; 
  }  
}

