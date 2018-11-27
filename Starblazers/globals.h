#define GLOBALS_H

//game definitions
#define titleScreen 0x05
#define menuScreen 0x00
#define gameScreen 0x01
#define overScreen 0x02
#define scoreScreen 0x03
#define infoScreen 0x04

//pin initializations
const int shootButton = 13;
  int shootIng = 0;
const int rightButton = 12;
  int rightIng = 0;
const int leftButton = 11;
  int leftIng = 0;
const int speakerPin = 7;

//pinsetup function
void pinsSetup() {
  pinMode(shootButton, INPUT);
  pinMode(rightButton, INPUT);
  pinMode(leftButton, INPUT);
}

//generalized global variables
int playerX = 60;
int playerY = 56;
unsigned int currentScore = 0;
unsigned int highScore = 0;
unsigned int addScore = 0;
byte playerLives = 3;
bool playerStatus = true;
int playerShotX = 0;
int playerShotY = 0;
byte curScreen = scoreScreen;
int menuPosition = 0;
unsigned int aboutInfo = 0;
int FPS = 0;
int animatePointX = 128;
int animatePointY = 32;
int infoAnimate = 0;
int startFlash = 0;
int flashTimer = 0;
int spawnAbleT = 1;
unsigned int spawnTY;
int starStart = 1;
int starPos[50];
int currentLevel = 1;
bool shotOut = false;
int moveState = 1;
int spawnTimer = 100;
int spawnX;
int enemiesArrInit[] = {
  0,0,0, //1  >0
  0,0,0, //2  >3
  11,11,11, //3  >6
  11,11,11, //4  >9
  0,0,0, //5  >12
  0,0,0, //6  >15
};
int enemiesArr[] = {
  0,0,0, //1  >0
  0,0,0, //2  >3
  11,11,11, //3  >6
  11,11,11, //4  >9
  0,0,0, //5  >12
  0,0,0, //6  >15
};
int asteroidMoveSpeed = 1;
