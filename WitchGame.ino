
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
  int health;
  int invunFrames;
  bool invunSet;
  int superCharges;
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
 short emyWidth;
 short emyHeight;
 short xPos;
 short yPos;
 unsigned char *sprite;
 PathLogic pathing;
 bool hit;
 int levelPosition;
 short startHeight;
 short vertDirection;
 short moveSpeed;
  //unsigned char *mask; //not needed for the bats
 //could also add frame like the witch if animated
};

/**************** Variable Declarations *****************/
const short totalBats = 12;
short batLevelStartPos[totalBats] = {128,180,180,500,600,700,850,850,900,1050,1050,1050};
short batLevelStartHeight[totalBats]= {20,10,30,15,30,20,10,30,25,5,15,35}; 
PathLogic batLogic[totalBats] = { PathLogic::Straight, PathLogic::Wave, PathLogic::Wave, PathLogic::Straight,PathLogic::Wave,PathLogic::Wave,PathLogic::Wave,PathLogic::Straight,PathLogic::Straight,PathLogic::Wave,PathLogic::Wave,PathLogic::Straight };
EnemyType1 bats[totalBats];
Rect batRects[totalBats];

PlayerCharacter witch = { witchDefaultFrameA[0],witchDefaultFrameA[1],0,8,1,State::Default, 5, 120, false, 3, witchDefaultFrameA,witchDefaultFrameB,witchDefaultABMask };

  Rect witchRect;
  Rect areaAttackRect;
  Rect longAttackRect;

int scrollDistance = 0;
short screenCount = 0;
short scrollSpeed = 2;
short cityCounter = -1;
short hillsCounter = -1;

bool aButtonPressed = false;
bool bButtonPressed = false;
bool upButtonPressed = false;
bool downButtonPressed = false;
bool leftButtonPressed = false;
bool rightButtonPressed = false;
bool secondFrame = false;
bool hitThisCycle = false;

bool areaAttack = false;
short areaAttackFrame = 0;
short areaAttackLength = 4; //number of frames of witch animation for the attack

bool longAttack = false;
short longAttackSpeed = 12; //number of pixels it moves per frame
short longAttackX = -100;
short longAttackY = -100;

String test = "";
short gameScreen = 0;
short witchInvunCount = 0;
short levelLength = 10; //number of screen the level is (array for multiple levls)
short batKillCount = 0; //for a score generator
int score = 0;

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
  bats[i].yPos = batLevelStartHeight[i];
  bats[i].sprite = batOutline;
  bats[i].pathing = batLogic[i];
  bats[i].hit = false;
  bats[i].levelPosition = batLevelStartPos[i];
  bats[i].startHeight = batLevelStartHeight[i];
  bats[i].vertDirection = 1;
  bats[i].moveSpeed = 1; //too coarse

  batRects[i] = { bats[i].emyWidth,bats[i].emyHeight,bats[i].xPos,bats[i].yPos };
 }
}

/**************** Setup.Startup Code  *****************/
void setup() {
  arduboy.begin();
  arduboy.setFrameRate(60);     //60 fps
  arduboy.clear();
  arduboy.audio.on();

}

/**************** Main Code Loop *****************/
void loop() {
 if(!arduboy.nextFrame()) {
    return; 
  }
  arduboy.clear();

switch(gameScreen){
  
  case 0:
    arduboy.setCursor(0, 0);
  arduboy.println("Wizardal Technicry...");
  arduboy.println("proundly presents...");
  if(arduboy.pressed(B_BUTTON) and bButtonPressed == false) { bButtonPressed = true; gameScreen = 1; } 
  break;
  
  case 1:
  Sprites::drawOverwrite(0,0,StartScreen,0);
  arduboy.setCursor(68, 32);
  arduboy.println("Press B!");
  
  //reset all changeable things for new run (I think that is all!)
  CreateBats();  //see above, makes all the bat data in game at start up (could put at level start if multiple levels?)
  witch.xPos = 0;
  witch.yPos = 8;
  witch.health = 5;
  witch.invunSet = false;
  scrollDistance = 0;
  screenCount = 0;
  areaAttack = false;
  longAttack = false;
  witch.superCharges = 3;
  batKillCount = 0;
  score =0;
  
  if(arduboy.pressed(B_BUTTON) and bButtonPressed == false) { bButtonPressed = true; gameScreen = 2; } 
  break;
  
  case 2:
  
  /*Level Position Logic*/
  if (arduboy.everyXFrames(scrollSpeed)){
  scrollDistance++;
  screenCount = (scrollDistance/128) + 1; //this always rounds down (so is correct for whole screens starting at 1)
  if(screenCount > levelLength){
    scrollDistance = 0;
    for(int i=0;i<totalBats; i++){
      bats[i].xPos = 128 - bats[i].emyWidth; //test code, reset "level after 5 screen and reset bats to respawn
      bats[i].hit = false;
    }
    gameScreen = 4;
  }
  }
  
  /*Input Control*/
  if(arduboy.pressed(UP_BUTTON)) {
    upButtonPressed = true;
    witch.yPos = witch.yPos - witch.moveSpeed;
    if(witch.yPos<8){
      witch.yPos=8; //screen limit (top - text)
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
    if(areaAttack == false && (witch.superCharges > 0)) 
    { 
      areaAttack = true;
      areaAttackFrame = 0; 
    }
  }
  if(arduboy.pressed(B_BUTTON)and bButtonPressed == false) {
    bButtonPressed = true;
    if(longAttack == false)
    {
      longAttack = true;
      longAttackX = witch.xPos + witch.pcWidth - 5; //5 is just to look nicer!
      longAttackY = witch.yPos + (witch.pcHeight/2) - 2; //(2 is ~1/2 of the attack sprite height, could also reference this from sprite data)
    }
  }
  
  /*Collision Detection*/
   witchRect = {witch.xPos, witch.yPos+4, witch.pcWidth-8, witch.pcHeight}; //smaller so only body, avoid broom hits counting
   areaAttackRect = {witch.xPos, witch.yPos-10, 30, 40};  
   longAttackRect = {longAttackX, longAttackY, 10, 5};  
  for(int i = 0; i<totalBats; i++){ 
    batRects[i] = {bats[i].xPos,bats[i].yPos,bats[i].emyWidth,bats[i].emyHeight }; 
    if(arduboy.collide(witchRect,batRects[i]) && bats[i].hit==false){    //forgot to ensure bats haven't already been hit (i.e. removed). For more complex enemies, could use a health > 0 check...
      hitThisCycle = true;
      batKillCount++;  
    }
    if((areaAttack && arduboy.collide(areaAttackRect,batRects[i]))||(longAttack && arduboy.collide(longAttackRect,batRects[i]))) 
    {
      bats[i].hit = true;  
    }
    //else
    //{
      //bats[i].hit = false;  
    //}  //else not needed outside of hit testing, once hit they are hit and dead unitl respawn
  }
  
  if(hitThisCycle==true && witch.invunSet==false){ 
    witch.invunSet = true;
    witch.health--; //drops too fast! need to make lone hit only.
    if(witch.health == 0) { gameScreen = 3; }
    witchInvunCount = arduboy.frameCount + witch.invunFrames;
  } 

  if (arduboy.frameCount == witchInvunCount) {
    witch.invunSet = false;
  }

    hitThisCycle= false;


  
  /*Witch Animation Code*/
  if (arduboy.everyXFrames(18/scrollSpeed)) { 
    if(!secondFrame) { secondFrame = true; } else { secondFrame = false;}
    if(areaAttack) { areaAttackFrame++; }
    if(areaAttackFrame > areaAttackLength) { areaAttack = false; witch.superCharges--; areaAttackFrame = 0;}
    if(longAttack) { longAttackX = longAttackX + longAttackSpeed; }
    if(longAttackX > 128) { longAttack = false; }
  }
  
  /*Screen Drawing Code*/
  arduboy.setCursor(0, 0);
  arduboy.print("hp:");
  for(int i=0; i<6; i++) {
  if(witch.health>i){
    arduboy.print("+");
  }else{
      arduboy.print(" ");
    }
  
  }
  arduboy.print("      mp:");
  for(int i=0; i<witch.superCharges; i++) {
  arduboy.print("*");
  }


  /*Drawing background code*/
  hillsCounter = BackgroundLayer(hillsBackdrop, hillsCounter);
  cityCounter = ForegroundLayer(citySkyline, cityCounter);
  
  /*Drawing Witch Code*/
  if(secondFrame){
    Sprites::drawExternalMask(longAttackX, longAttackY, longAttackFrameA, longAttackMaskA, 0, 0); 
    if(areaAttack) {
        Sprites::drawExternalMask(witch.xPos, witch.yPos-10, areaAttackFrameA, areaAttackMaskA, 0, 0); 
    }
    else{
        Sprites::drawExternalMask(witch.xPos, witch.yPos, witch.defaultFrameA, witch.defaultMask, 0, 0); 
    }
  } 
  else{
    Sprites::drawExternalMask(longAttackX, longAttackY, longAttackFrameB, longAttackMaskB, 0, 0); 
    if(areaAttack) {
      Sprites::drawExternalMask(witch.xPos, witch.yPos-10, areaAttackFrameB, areaAttackMaskB,0, 0); 
    }
    else{
      Sprites::drawExternalMask(witch.xPos, witch.yPos, witch.defaultFrameB, witch.defaultMask, 0, 0); 
    }
    } 
  
  /*Drwing Enemies (bats) Code*/
  //issue here. Bat speed is too coarse using pixels per frame (slowest speed is barely over 2 seconds, next is 1 second, then 0.66)
  //consider moving to a everyXframes type movement? where the move speed  determins if that bats postion is updated? link to existing scroll speed update rate?
  for(int i = 0; i<totalBats; i++){ 
    if((bats[i].levelPosition - 128 <= scrollDistance) && (bats[i].xPos>0-bats[i].emyWidth) && bats[i].hit == false ){

      switch(bats[i].pathing){
        
     case PathLogic::Wave: 
        bats[i].xPos = bats[i].xPos - bats[i].moveSpeed;
        bats[i].yPos = bats[i].yPos + (bats[i].moveSpeed*bats[i].vertDirection);
        if(bats[i].yPos > bats[i].startHeight + 16) { 
          bats[i].vertDirection = -1; 
        }
        if(bats[i].yPos < bats[i].startHeight - 16) { 
          bats[i].vertDirection = 1; 
        }
        break;

       case PathLogic::Straight:
       bats[i].xPos = bats[i].xPos - bats[i].moveSpeed;
       bats[i].yPos = bats[i].yPos;
       break;

       default:
       break;
        
      }
      Sprites::drawOverwrite(bats[i].xPos,bats[i].yPos,bats[i].sprite,0);
    }
  }


   break;

   case 3:
   arduboy.setCursor(0, 0);
   arduboy.println("   Game Over  :(");
   arduboy.println("See if you can make");
   arduboy.println("the end next time!");
   Sprites::drawOverwrite(48,32,TreeCrash,0);
   if(arduboy.pressed(B_BUTTON)and bButtonPressed == false) { bButtonPressed = true; gameScreen = 0; } 
   break;

   case 4:
   arduboy.setCursor(0, 0);
   Sprites::drawOverwrite(0,0,WinScreen,0);
   if(arduboy.pressed(B_BUTTON)and bButtonPressed == false) { bButtonPressed = true; gameScreen = 5; } 
   break;
   
   case 5:
   arduboy.setCursor(0, 0);
   arduboy.println("You did it!");
   score = 0;
   score = score + (witch.health * 10); //10 pts per bit of health
   score = score + (witch.superCharges *20); //20 pts per super charge
   score = score + (batKillCount * 5);
   arduboy.print("Score: ");
   arduboy.print(score); 
   arduboy.println("");
   arduboy.println("");
   arduboy.println("");
   arduboy.println("Press A to restart");
   if(arduboy.pressed(A_BUTTON)and aButtonPressed == false) { aButtonPressed = true; gameScreen = 0; } 
   break;
   
   default:
   break;
  
}

  
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

  arduboy.display();  //line that actually draws all the setup stuff above in any of the states
}
