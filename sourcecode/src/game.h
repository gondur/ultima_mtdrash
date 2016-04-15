#ifndef GAME_H
#define GAME_H

#include "math/primitives.h"
#include "utility/List.h"
#include "utility/os.h"


class Game {
 private:
   // maze:
   // 0 == wall
   // 1 == corridor
   // 2 == seen corridor
   // 3 == gem
   // 4 == exit square
   byte* maze;
   bool gameOver;
   dword lastSecondTime;
   
   dword lastGateRotateTime;
   byte gateScreenCode;
   byte gateFGColor;
   byte gateBGColor;
  
   // arrays used in game logic:
   // Converts a direction to displacement in X or Y direction
   int dx[4];
   int dy[4];
   
   // positions of monsters
   int monsterX[20];
   int monsterY[20];
  
   
   byte rank;                     // rank of player
   byte level;                    // current level
   byte gems;                     // gems retrieved from level
   byte heading;                  // current player direction
   byte timeLeft;                 // time left in seconds to finish level 
   byte hitpoints;                // hitpoints left of player
   byte playerX, playerY;         // the player position on map.
   byte numMonsters;              // how many monsters on map? 
   
   byte spellsLeft;               // how many spells does the player have left?
   byte sleepTimeLeft;            // if a sleep spell is cast, how many more turns will it work?
   bool screenShaking;            // for the blast spell
   dword screenShakeEndTime;
   
   bool combatMode;               // are we in combat mode or explore mode:
   dword lastCombatTime;
   byte creatureNr;               // if in combat mode, which creature do we fight?
   byte creatureType;             // which monster do we fight?
   byte playerCombatX;            // players X-pos on the combat screen
   byte playerCombatStance;       // one of three stances
   byte monsterCombatX;           // monsters X-pos on the combat screen  
   byte monsterCombatStance;      // monsters have two stances
         
   string rankings[6];            // name of the different rankings 
   string monsterNames[5];        // name of the 5 different monsters
   string directions[4];          // name of the 4 directions
   
   // These are lists of strings dumped from the original vic20 basic program.
   // They contain the graphics for the intro screen, monsters and the 3d view
   List<string> introStrings;     
   List<string> monsterStrings;
   List<string> gameviewStrings;
    
   void updateVisibility();   
   void spawnMonsters();   
   void generateMaze();   
   void initializeLevel(byte lvl); 
   void loadStrings(const string& fileName, List<string>* result); 
   
   void showEnteringLevelMessage(byte level); 
   void showInfoString(const string& message, dword time); 
   void showWinScreen();
    
   void drawMap(); 
   void draw3dTile(byte a, byte b, byte lr, byte distance);
   void draw3dView();   
   void drawLevel();   
   void drawRanking();   
   void drawHeadingAndTime();
   
   void beginCombat(byte foe);
   void drawCombatScreen();
   void drawMonsterName();
   
   
   void requestMoveTo(byte x, byte y);
      
   void updateCombatAI();   
   void updateMonsterPositions();   
      
   static Game* instance;
   Game();
   ~Game();
 public:
   static Game* getInstance();
 
   void showTitleScreen();
 
   void reset();
   bool running();
  
   void frameMove();
   void frameRender();

   void stepForward();
   void stepBackwards();
   void turnLeft();
   void turnRight();
   
   void setCombatStance(byte stance) {playerCombatStance = stance;}
   void useSpell(byte spellnr);
   
};





#endif
