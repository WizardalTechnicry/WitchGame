
#include <Arduboy2.h>
#include <ArduboyTones.h>
#include "imageData.h"

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

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


int witchWidth = witchDefaultFrameA[0];
int witchHeight = witchDefaultFrameA[1];
int batWidth = batOutline[0];
int batHeight = batOutline[1];
int witchX = 0;
int witchY = 0;
int batX = (128-batWidth);
int batY = (32-(batHeight/2));
int witchSpeed =2;
int scrollSpeed = 2;
int batDirection = 1;
long scrollDistance = 0;
int cityCounter = -1;
int hillsCounter = -1;


bool aButtonPressed = false;
bool bButtonPressed = false;
bool upButtonPressed = false;
bool downButtonPressed = false;
bool leftButtonPressed = false;
bool rightButtonPressed = false;

bool secondFrame = false;

String test = "";

Background hillsBackdrop = {hillsBackdrop1, hillsBackdrop2, 10};

Foreground citySkyline = {citySkyline1, citySkyline1Mask,citySkyline2, citySkyline2Mask, scrollSpeed};

int BackgroundLayer(Background _layerData, int _scrollCounterIn)
{
if(_scrollCounterIn>254) {_scrollCounterIn = -1; }

Sprites::drawOverwrite(0-_scrollCounterIn, 32, _layerData.imageData1, 0);
Sprites::drawOverwrite(128-_scrollCounterIn, 32, _layerData.imageData2, 0);
Sprites::drawOverwrite(256-_scrollCounterIn, 32, _layerData.imageData1, 0); 

if (arduboy.everyXFrames(_layerData.scrollRate)) { _scrollCounterIn++; }   

return _scrollCounterIn;
}

int ForegroundLayer(Foreground _layerData, int _scrollCounterIn)
{
if(_scrollCounterIn>254) {_scrollCounterIn = -1; }

Sprites::drawExternalMask(0-_scrollCounterIn, 32, _layerData.imageData1, _layerData.imageData1Mask, 0, 0);
Sprites::drawExternalMask(128-_scrollCounterIn, 32, _layerData.imageData2, _layerData.imageData2Mask, 0, 0); 
Sprites::drawExternalMask(256-_scrollCounterIn, 32, _layerData.imageData1, _layerData.imageData1Mask, 0, 0);    

if (arduboy.everyXFrames(_layerData.scrollRate)) { _scrollCounterIn++; }   

return _scrollCounterIn;
}

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(60);     //60 fps
  arduboy.clear();
  arduboy.audio.on();
}

void loop() {
 if(!arduboy.nextFrame()) {
    return; 
}
arduboy.clear();

//Witch Animation Update Code
if (arduboy.everyXFrames(20/scrollSpeed)) { 
  if(!secondFrame) { secondFrame = true; } else { secondFrame = false;}
  }

//Input Control Code
        if(arduboy.pressed(UP_BUTTON)) {
           upButtonPressed = true;
           witchY = witchY - witchSpeed;
           if(witchY<0){
               witchY=0; //screen limit (top)
           }
        }
        if(arduboy.pressed(DOWN_BUTTON)) {
          downButtonPressed = true;
          witchY = witchY + witchSpeed;
          if(witchY>(64-witchHeight)){
            witchY=(64-witchHeight); //screen limit (bottom, factors cursor size)
          }
        }
        if(arduboy.pressed(LEFT_BUTTON)) {
          leftButtonPressed = true;
          witchX = witchX - witchSpeed;
          if(witchX<0){
            witchX=0; //screen limit (left)
          }
        }
        if(arduboy.pressed(RIGHT_BUTTON)) {
          rightButtonPressed = true;
          witchX = witchX + witchSpeed;
          if(witchX>(128-witchWidth)){
            witchX=(128-witchWidth); //screen limit (right, factors in cursor size)
          }
        }
        
        if(arduboy.pressed(A_BUTTON)and aButtonPressed == false) {
          aButtonPressed = true;
        }
        if(arduboy.pressed(B_BUTTON)and bButtonPressed == false) {
          bButtonPressed = true;
        }


batX = batX - scrollSpeed;
if(batX<(0-batWidth)) { batX = 128; }
batY = batY + (1*batDirection);  //limited contorl of speed via this: consider some code using everyXframe to make more use of Framerate (current only 60,30,15 etc)
if(batY >40 ) { batDirection = -1; } //this gives a sinwave ish movement
if(batY <16 ) { batDirection = 1; }


Rect witchRect = {witchX, witchY+4, witchWidth-8, witchHeight};
Rect batRect = {batX, batY, batWidth, batHeight};

if(arduboy.collide(witchRect,batRect)){

  test = "hit!";
}
else{ test = ""; }

//Start of Screen drawing code
arduboy.setCursor(0, 0);
arduboy.println(test);

//Scrolling Background Code

hillsCounter = BackgroundLayer(hillsBackdrop, hillsCounter);
cityCounter = ForegroundLayer(citySkyline, cityCounter);



if(secondFrame) { Sprites::drawExternalMask(witchX, witchY, witchDefaultFrameB,witchDefaultABMask,0, 0); } else { Sprites::drawExternalMask(witchX, witchY, witchDefaultFrameA,witchDefaultABMask,0, 0); } 


Sprites::drawOverwrite(batX,batY, batOutline, 0);

 arduboy.display();

 //resets the bools that say a button is pressed (makes it wait for release to stop multiple hit on every press)
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
