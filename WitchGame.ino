
/**********included files/libraries***************/
#include <Arduboy2.h>
#include <ArduboyTones.h>
#include "imageData.h"

/**************** Structs and enums *****************/
enum class State : uint8_t {
  Default,
  Up,
  Down,
  Back,
  Forward,
  Hit,
  Shoot,
};

enum class PathLogic : uint8_t {
  Wave,
  Straight,
  LoopNLeave,
  
};

struct Background{
  unsigned char *imageData1;
  unsigned char *imageData2;
  int scrollRate;  
};

struct Foreground{
   unsigned char *imageData1;
  unsigned char *imageData1Mask;
  unsigned char *imageData2;
  unsigned char *imageData2Mask;
  int scrollRate;
};

struct PlayerCharacter{
  int pcWidth;
  int pcHeight;
  int xPos;
  int yPos;
  int moveSpeed;
  State state;
  unsigned char *defaultFrameA;
  unsigned char *defaultFrameB;
  unsigned char *defaultMask;
  //unsigned char *fwdFrameA; //add these in once sprites are drawn?
  //unsigned char *fwdFrameB;
  //unsigned char *backFrameA;
  //unsigned char *backFrameB;
  //unsigned char *upFrameA;
  //unsigned char *upFrameB;
  //unsigned char *downFrameA;
  //unsigned char *downFrameB;
};

struct EnemyType1{
 int emyWidth;
 int emyHeight;
 int xPos;
 int yPos;
 unsigned char *sprite;
 PathLogic pathing;
 bool hit;
 long levelPosition;
 int startHeight;
 int vertDirection;
 int moveSpeed;
  //unsigned char *mask; //not needed for the bats
 //could also add frame like the witch if animated
};

/**************** Variable Declarations *****************/
const int totalBats = 4;
long batLevelStartPos[totalBats] = {128,135,260,500};
int batLevelStartHeight[totalBats]= {(32-(batOutline[1]/2)),0,0,10}; //currently has to be over "bounce point (16) due to movement code. change to factor in move type and direction varies based on this (below midpoint go up,above go down)
EnemyType1 bats[totalBats];
Rect batRects[totalBats];

PlayerCharacter witch = { witchDefaultFrameA[0],witchDefaultFrameA[1],0,0,2,State::Default,witchDefaultFrameA,witchDefaultFrameB,witchDefaultABMask };

long scrollDistance = 0;

int screenCount = 0;
int scrollSpeed = 2;
int cityCounter = -1;
int hillsCounter = -1;

bool aButtonPressed = false;
bool bButtonPressed = false;
bool upButtonPressed = false;
bool downButtonPressed = false;
bool leftButtonPressed = false;
bool rightButtonPressed = false;
bool secondFrame = false;
bool hitThisCycle = false;

bool areaAttack = false;
int areaAttackFrame = 0;
int areaAttackLength = 4; //number of frames of witch animation for the attack

String test = "";

Arduboy2 arduboy;

ArduboyTones sound(arduboy.audio.enabled);

Background hillsBackdrop = {hillsBackdrop1, hillsBackdrop2, 10};

Foreground citySkyline = {citySkyline1, citySkyline1Mask,citySkyline2, citySkyline2Mask, scrollSpeed};

/**************** Functions Descriptions/Code *****************/
int BackgroundLayer(Background _layerData, int _scrollCounterIn)
{
  if(_scrollCounterIn>254) {
       _scrollCounterIn = -1; 
    }
  Sprites::drawOverwrite(0-_scrollCounterIn, 32, _layerData.imageData1, 0);
  Sprites::drawOverwrite(128-_scrollCounterIn, 32, _layerData.imageData2, 0);
  Sprites::drawOverwrite(256-_scrollCounterIn, 32, _layerData.imageData1, 0); 
  if (arduboy.everyXFrames(_layerData.scrollRate)) { 
      _scrollCounterIn++; 
    }   
  return _scrollCounterIn;
}

int ForegroundLayer(Foreground _layerData, int _scrollCounterIn)
{
  if(_scrollCounterIn>254) {
      _scrollCounterIn = -1; 
    }
  Sprites::drawExternalMask(0-_scrollCounterIn, 32, _layerData.imageData1, _layerData.imageData1Mask, 0, 0);
  Sprites::drawExternalMask(128-_scrollCounterIn, 32, _layerData.imageData2, _layerData.imageData2Mask, 0, 0); 
  Sprites::drawExternalMask(256-_scrollCounterIn, 32, _layerData.imageData1, _layerData.imageData1Mask, 0, 0);    
  if (arduboy.everyXFrames(_layerData.scrollRate)) { 
    _scrollCounterIn++; 
  }   
  return _scrollCounterIn;
}

void CreateBats(){
 for(int i = 0; i<totalBats; i++){
  bats[i].emyWidth = batOutline[0];
  bats[i].emyHeight = batOutline[1];
  bats[i].xPos = 128-batOutline[0];
  bats[i].yPos = 0;
  bats[i].sprite = batOutline;
  bats[i].pathing = PathLogic::Wave;
  bats[i].hit = false;
  bats[i].levelPosition = batLevelStartPos[i];
  bats[i].startHeight = batLevelStartHeight[i];
  bats[i].vertDirection = 1;
  bats[i].moveSpeed = 1;

  batRects[i] = { bats[i].emyWidth,bats[i].emyHeight,bats[i].xPos,bats[i].yPos };
 }
}

/**************** Setup.Startup Code  *****************/
void setup() {
  arduboy.begin();
  arduboy.setFrameRate(60);     //60 fps
  arduboy.clear();
  arduboy.audio.on();

  CreateBats(); //see above, makes all the bat data in game at start up (could put at level start if multiple levels?)
}

/**************** Main Code Loop *****************/
void loop() {
 if(!arduboy.nextFrame()) {
    return; 
  }
  arduboy.clear();

  /*Level Position Logic*/
  if (arduboy.everyXFrames(scrollSpeed)){
  scrollDistance++;
  screenCount = (scrollDistance/128) + 1; //this always rounds down (so is correct for whole screens starting at 1)
  if(screenCount > 5){
    scrollDistance = 0;
    for(int i=0;i<totalBats; i++){
      bats[i].xPos = 128 - bats[i].emyWidth; //test code, reset "level after 5 screen and reset bats to respawn
    }
  }
  }
  
  /*Input Control*/
  if(arduboy.pressed(UP_BUTTON)) {
    upButtonPressed = true;
    witch.yPos = witch.yPos - witch.moveSpeed;
    if(witch.yPos<0){
      witch.yPos=0; //screen limit (top)
    }
  }
  if(arduboy.pressed(DOWN_BUTTON)) {
    downButtonPressed = true;
    witch.yPos = witch.yPos + witch.moveSpeed;
    if(witch.yPos>(64-witch.pcHeight)){
      witch.yPos=(64-witch.pcHeight); //screen limit (bottom, factors character size)
    }
  }
  if(arduboy.pressed(LEFT_BUTTON)) {
    leftButtonPressed = true;
    witch.xPos = witch.xPos - witch.moveSpeed;
    if(witch.xPos<0){
      witch.xPos=0; //screen limit (left)
    }
  }
  if(arduboy.pressed(RIGHT_BUTTON)) {
    rightButtonPressed = true;
    witch.xPos = witch.xPos + witch.moveSpeed;
    if(witch.xPos>(128-witch.pcWidth)){
      witch.xPos=(128-witch.pcWidth); //screen limit (right, factors in character size)
    }
  }  
  if(arduboy.pressed(A_BUTTON)and aButtonPressed == false) {
    aButtonPressed = true;
    if(areaAttack == false) 
    { 
      areaAttack = true;
      areaAttackFrame = 0; 
    }
  }
  if(arduboy.pressed(B_BUTTON)and bButtonPressed == false) {
    bButtonPressed = true;
  }
  
  /*Collision Detection*/
  Rect witchRect = {witch.xPos, witch.yPos+4, witch.pcWidth-8, witch.pcHeight}; //smaller so only body, avoid broom hits counting
  for(int i = 0; i<totalBats; i++){ 
    batRects[i] = {bats[i].xPos,bats[i].yPos,bats[i].emyWidth,bats[i].emyHeight }; 
    if(arduboy.collide(witchRect,batRects[i])){  
      hitThisCycle = true;  
    }
  }
  if(hitThisCycle==true){ 
    test = "hit!"; 
  } 
  else { 
    test = ""; 
  }
  hitThisCycle= false;
  
  /*Witch Animation Code*/
  if (arduboy.everyXFrames(20/scrollSpeed)) { 
    if(!secondFrame) { secondFrame = true; } else { secondFrame = false;}
    if(areaAttack) { areaAttackFrame++; }
    if(areaAttackFrame > areaAttackLength) { areaAttack = false; areaAttackFrame = 0;}
  }
  
  /*Screen Drawing Code*/
  arduboy.setCursor(0, 0);
  arduboy.println(screenCount);
  arduboy.println(test);

  
  /*Drawing background code*/
  hillsCounter = BackgroundLayer(hillsBackdrop, hillsCounter);
  cityCounter = ForegroundLayer(citySkyline, cityCounter);
  
  /*Drawing Witch Code*/
  if(secondFrame){ 
    if(areaAttack) {
        Sprites::drawExternalMask(witch.xPos, witch.yPos-10, areaAttackFrameA, areaAttackMaskA, 0, 0); 
    }
    else{
        Sprites::drawExternalMask(witch.xPos, witch.yPos, witch.defaultFrameA, witch.defaultMask, 0, 0); 
    }
  } 
  else{
    if(areaAttack) {
      Sprites::drawExternalMask(witch.xPos, witch.yPos-10, areaAttackFrameB, areaAttackMaskB,0, 0); 
    }
    else{
      Sprites::drawExternalMask(witch.xPos, witch.yPos, witch.defaultFrameB, witch.defaultMask, 0, 0); 
    }
    } 
  
  /*Drwing Enemies (bats) Code*/
  for(int i = 0; i<totalBats; i++){ 
    if((bats[i].levelPosition - 128 <= scrollDistance)&&(bats[i].xPos>0-bats[i].emyWidth) ){
      bats[i].xPos = bats[i].xPos - bats[i].moveSpeed;
      bats[i].yPos = bats[i].yPos + (bats[i].moveSpeed*bats[i].vertDirection);
      if(bats[i].yPos > 40) { 
        bats[i].vertDirection = -1; 
      }
      if(bats[i].yPos < 16) { 
        bats[i].vertDirection = 1; 
      }
      Sprites::drawOverwrite(bats[i].xPos,bats[i].yPos,bats[i].sprite,0);
    }
  }
  arduboy.display();  //line that actually draws all the setup stuff above
  
  /*Button Reset (avoid multi-press) Logic*/
  if(arduboy.notPressed(A_BUTTON)) {
    aButtonPressed = false;
  }
  if(arduboy.notPressed(B_BUTTON)) {
    bButtonPressed = false;
  }  
  if(arduboy.notPressed(UP_BUTTON)) {
    upButtonPressed = false;
  }  
  if(arduboy.notPressed(DOWN_BUTTON)) {
    downButtonPressed = false;
  }  
  if(arduboy.notPressed(LEFT_BUTTON)) {
    leftButtonPressed = false;
  }
  if(arduboy.notPressed(RIGHT_BUTTON)) {
    rightButtonPressed = false;
  } 
}
