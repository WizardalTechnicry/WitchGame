
#include <Arduboy2.h>
#include <ArduboyTones.h>

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

const unsigned char PROGMEM Witch_Default[] =
{
// width, height,
24, 20,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0xf0, 0xfc, 0x12, 0x31, 0x12, 0xbc, 0x50, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x94, 0xd8, 0x24, 0xa8, 0x50, 0x20, 0x20, 0xa0, 0x61, 0x30, 0x0e, 0x01, 0x01, 0x01, 0x3e, 0xd2, 0x14, 0x18, 0x10, 0x10, 0x10, 0x10, 0x18, 0x08, 
0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x09, 0x05, 0x03, 0x01, 0x05, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};


const unsigned char PROGMEM bat_outline[] =
{
// width, height,
12, 8,
0x7f, 0x18, 0xbe, 0xd9, 0x2a, 0x7e, 0x7e, 0x2a, 0xd9, 0xbe, 0x18, 0x7f, 
};


int witchWidth = Witch_Default[0];
int witchHeight = Witch_Default[1];
int witchX = 0;
int witchY = 0;
int witchSpeed =1;

bool AButtonPressed = false;
bool BButtonPressed = false;
bool UpButtonPressed = false;
bool DownButtonPressed = false;
bool LeftButtonPressed = false;
bool RightButtonPressed = false;



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


        if(arduboy.pressed(UP_BUTTON)) {
           UpButtonPressed = true;
           witchY = witchY - witchSpeed;
           if(witchY<0){
               witchY=0; //screen limit (top)
           }
        }
        if(arduboy.pressed(DOWN_BUTTON)) {
          DownButtonPressed = true;
          witchY = witchY + witchSpeed;
          if(witchY>(64-witchHeight)){
            witchY=(64-witchHeight); //screen limit (bottom, factors cursor size)
          }
        }
        if(arduboy.pressed(LEFT_BUTTON)) {
          LeftButtonPressed = true;
          witchX = witchX - witchSpeed;
          if(witchX<0){
            witchX=0; //screen limit (left)
          }
        }
        if(arduboy.pressed(RIGHT_BUTTON)) {
          RightButtonPressed = true;
          witchX = witchX + witchSpeed;
          if(witchX>(128-witchWidth)){
            witchX=(128-witchWidth); //screen limit (right, factors in cursor size)
          }
        }
        
        if(arduboy.pressed(A_BUTTON)and AButtonPressed == false) {
          AButtonPressed = true;
          witchSpeed++;
        }
        if(arduboy.pressed(B_BUTTON)and BButtonPressed == false) {
          BButtonPressed = true;
          witchSpeed--;
        }

     if(witchSpeed<1){ witchSpeed = 1; }
     if(witchSpeed>5){ witchSpeed = 5; }
arduboy.setCursor(0, 0);
arduboy.println(witchSpeed);

Sprites::drawOverwrite(witchX, witchY, Witch_Default, 0);
Sprites::drawOverwrite(64, 10, bat_outline, 0);
 arduboy.display();

 //resets the bools that say a button is pressed (makes it wait for release to stop multiple hit on every press)
  if(arduboy.notPressed(A_BUTTON)) {
    AButtonPressed = false;
  }
  if(arduboy.notPressed(B_BUTTON)) {
    BButtonPressed = false;
  }  
  if(arduboy.notPressed(UP_BUTTON)) {
    UpButtonPressed = false;
  }  
  if(arduboy.notPressed(DOWN_BUTTON)) {
    DownButtonPressed = false;
  }  
  if(arduboy.notPressed(LEFT_BUTTON)) {
    LeftButtonPressed = false;
  }
  if(arduboy.notPressed(RIGHT_BUTTON)) {
    RightButtonPressed = false;
  }


 
}
