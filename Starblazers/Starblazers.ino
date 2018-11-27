//LIBRARY INCS
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
//EXTERNAL HEADERS
#include "bitmaps.h"
#include "globals.h"
#include "music.h"
// OLED DISPLAY TWI ADDRESS
#define OLED_ADDR 0x3C

#define WIDTH 128

// RESET PIN NOT USED ON DISPLAY
Adafruit_SSD1306 display(-1);  // -1 = no reset pin

// 128 X 64 PIXEL DISPLAY
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

//OVERALL INITIAL SETUP
void setup() {
  Serial.begin(9600);
  pinsSetup();
  while(!eeprom_is_ready());
  highScore = eeprom_read_word(0x0000);
  //initalize the display
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  //then clear
  display.clearDisplay();
  display.display();
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.setTextWrap(false);
  display.clearDisplay();
  playerLives = 0;
  playerStatus = 0;
  menuPosition = 0;
  curScreen = titleScreen;
  musicInit;
  spawnTimer = 100;
}

//HOLDS MENU HANDLER -> ALSO HANDS GENERAL SCREEN REFRESH
void loop() {
  //playMusic(2,900);
  //MENU SWITCH
  switch(curScreen){
    case titleScreen:
      display.clearDisplay();
      drawTitle();
      break;
    case menuScreen:
      display.clearDisplay();
      drawM();
      break;
    case overScreen:
      display.clearDisplay();
      drawGO();
      break;
    case scoreScreen:
      display.clearDisplay();
      drawHS();
      break;
    case infoScreen:
      display.clearDisplay();
      spawnStars();
      drawInfo();
      break;
    case gameScreen:
      display.clearDisplay();
      updateG();
      drawG();
      asteroidSpawn();
      break;
      
  } 
  display.display();
  }

//SETS UP BACKGROUND, AND HANDLES PLAYER MOVE FRAME
void drawG(){
  spawnStars();
  drawAsteroid();
  if(leftIng){
  display.drawBitmap(playerX,playerY,playerLeftVeer, 11, 7, WHITE);
  }
  if(rightIng){
  display.drawBitmap(playerX,playerY,playerRightVeer, 11, 7, WHITE);
  }
  else{
  display.drawBitmap(playerX,playerY,playerForward, 11, 6, WHITE);
  }
  display.drawBitmap(playerShotX,playerShotY,playerShot, 3, 3, WHITE);
}

//UPDATE ASTEROID POSITION, SUMS SCORE, AND CREATES BOUNDARIES
void updateG(){
  buttonChecks();
  moveState = 1;
  shotCheck();
  asteroidMove();
  if(addScore> 0){
    currentScore += addScore;
    addScore -= addScore;
  }
  if(playerStatus){
    if(leftIng) {
      playerX -= 6;
      moveState = 2;
    }
    if(rightIng) {
      playerX += 6;
      moveState = 3;
    }
    if(playerX < 3) playerX = 2;
    if(playerX > 115) playerX = 115;
      }
   drawUI();
   spawnTimer += 1;
}

//CHECKS IF SHOT IS CURRENTLY OUT
void shotCheck(){
  if(shootIng && !shotOut){
      shootBeep();
      playerShotX = playerX + 4;
      playerShotY = playerY + 10;
      shotOut = true;
    }
   if(shotOut){
    playerShotY -= 3;  
    if(playerShotY <= 0){
      shotOut = false;
      playerShotY = -5;
      playerShotX = -5;
    }
   }
}

//DRAWS MAIN MENU AND HANDLES POINTER/MENU SELECTION
void drawM(){
  int x = 24;
  int y = 19;
  passiveAnimate();
  buttonChecks();
  if(leftIng == HIGH){
    menuBeep();
    menuPosition += 1;
      if(menuPosition > 2) {
        menuPosition = 2;
      }
  }
  if(rightIng == HIGH){
    menuBeep();
    menuPosition -= 1;
      if(menuPosition < 0) {
        menuPosition = 0;
    }
  }
  if(shootIng == HIGH){
    menuBeep();
    if(menuPosition == 0){
      display.clearDisplay();
      delay(10);
      curScreen = gameScreen;
      playerLives = 3;
      currentScore = 0;
      resetGame();
      //game
    }
    if(menuPosition == 1){
      menuPosition == 1;
      display.clearDisplay();
      delay(10);
      curScreen = scoreScreen;
      //highscore 
    }
    if(menuPosition == 2){
      menuPosition = 0;
      infoAnimate = 0;
      display.clearDisplay();
      delay(10);
      curScreen = infoScreen;
      //infoscreen
     }
   }
   display.fillRect(x,y, 80, 36, BLACK);
   display.drawRect(x,y, 80, 36, WHITE);
   display.fillRect(x+2, y+2 + menuPosition * 11, 76, 10, WHITE);
   display.setTextColor(INVERSE);
   display.setCursor(x + 11, y + 3);
   display.print(F("Start game"));
   display.setCursor(x + 18, y + 14);
   display.print(F("Hi-score"));
   display.setCursor(x + 26, y + 25);
   display.print(F("About"));
   display.setTextColor(WHITE);
}

//DRAWS GAME OVER SCREEN AND CHECKS FOR NEW HISGHSCORE
void drawGO(){
  int x = 24;
  int y = 19;
  passiveAnimate();
  buttonChecks();
  if(shootIng == HIGH){
    menuBeep();
    display.clearDisplay();
    curScreen = menuScreen;
    menuPosition = 0;
  }
  display.fillRect(x,y, 80, 36, BLACK);
  display.drawRect(x,y, 80, 36, WHITE);
  display.fillRect(x+2,y+2, 75, 10, WHITE);
  display.setTextColor(INVERSE);
  display.setCursor(x+14,y+3);
  display.print(F("GAME OVER"));
  display.setCursor(x+4,y+14);
  if(currentScore > highScore){
    display.print(F("New Hi-Score:"));
    eeprom_write_word(0x0000, currentScore);
  }
  else {
    display.print(F("Score:"));
}
  display.setCursor(x+4,y+25);
  display.print(String(currentScore));
  display.setTextColor(WHITE);
}

//DRAWS HIGHSCORE SCREEN AND ALLOWS FOR EEPROM CLEARING
void drawHS(){
  int x = 20;
  int y = 16;
  passiveAnimate();
  buttonChecks();
  if(leftIng == HIGH){
    menuBeep();
    menuPosition += 1;
      if(menuPosition > 1) {
        menuPosition = 1;
      }
  }
  if(rightIng == HIGH){
    menuBeep();
    menuPosition -= 1;
      if(menuPosition < 0) {
        menuPosition = 0;
  }
  if(shootIng == HIGH){
    menuBeep();
    curScreen = menuScreen;
    delay(10);
    menuPosition = 0;
    display.clearDisplay();
  }
 }

  display.fillRect(x,y, 97, 36, BLACK);
  display.drawRect(x,y, 97, 36, WHITE);
  if(menuPosition == 0){
    display.fillRect(x+2, y+2 + (menuPosition + 1) * 11, 40, 10, WHITE);
  }
  else{
  display.fillRect(x+2, y+2 + (menuPosition + 1) * 11, 95, 10, WHITE);
  }
  display.setTextColor(INVERSE);
  display.setCursor(x+5,y+3);
  display.print(F("Hi-Score:"));
  display.setCursor(x+5, y+14);
  if(menuPosition == 0) {
    display.print(F("Clear?"));
    }else {
      display.print(String(eeprom_read_word(0x0000)));
    }
    display.setCursor(x+5, y+25);
    display.print(F("Return to menu?"));
    display.setTextColor(WHITE);

    if(shootIng == HIGH){
      menuBeep();
      switch(menuPosition){
        case 0:
          highScore = 0;
          eeprom_write_word(0x0000, 0);
          menuPosition = 1;
          break;
        case 1:
          menuPosition = 0;
          curScreen = menuScreen;
          break;
      }
    }
  }


//FUNCTION TO RESET GLOBALS PERTAINING TO GAMEPLAY
 void resetGame(){
  playerX = 60;
  playerY = 56;
  playerStatus = true;
  spawnTimer = 100;
  for(int i = 0; i <= 17; i++){
  enemiesArr[i] = enemiesArrInit[i];
     }
  }

//DRAWS THE GAME FRAME
 void drawGame(){
    int i;
    display.clearDisplay();
    drawUI();
    spawnStars();
  } 

//CHECKS FOR BUTTON INPUT
void buttonChecks(){
  shootIng = digitalRead(shootButton);
  rightIng = digitalRead(rightButton);
  leftIng = digitalRead(leftButton);
  delay(100);
}

//DRAWS THE TITLE SCREEN
void drawTitle(){
  buttonChecks();
  const int width = 128;
  const int height = 17;
  passiveAnimate();
  display.drawBitmap(
    (display.width() - width)/10,
    (display.height() - height)/10,
    startTitle, width, height, 1);
  
  if(shootIng == HIGH){
    menuBeep();
    curScreen = menuScreen;
    menuPosition = 0;
  }
  if(flashTimer >= 5){
    const int widthIn = 40;
    const int heightIn = 12;
    display.drawBitmap(
      (display.width() - widthIn)/2,
      (display.height() - heightIn)/1.5,
      startType, widthIn, heightIn, (startFlash-1));
      flashTimer = 0;
  }
  startFlash += 1;
  flashTimer += 1;
  if(startFlash > 2){
    startFlash = 1;
  }
}

//EASY ACCESS ANIMATE
void passiveAnimate(){
 animateAsteroid();
 spawnStars();
}


//DRAWS CURRENT SCORE AND LIVES
void drawUI(){
  int i;
  if(playerLives > 0 || playerStatus){
    display.drawRect(0,0,128,64,WHITE);
    display.setCursor(3,3);
    display.print(String("Score:" + String(currentScore, DEC)));
    for(i=0; i<playerLives; i++){
      display.drawBitmap(128-13-i*10,2,playerForward, 11, 6, INVERSE);
    }
  } 
}

//RANDOMLY SEEDS A Y LOCATION FROM A0 PIN, WILL THEN ANIMATE ACROSS SCREEN TO BOUNDS
 void animateAsteroid(){
  animatePointX = (animatePointX-3);
  if(spawnAbleT == 1){
  delay(10);
  randomSeed(analogRead(A0));
  spawnTY = random()%65;
  spawnAbleT = 0;
  }
  if(animatePointX < -11){
    animatePointX = 128;
    spawnAbleT = 1;
  }
  display.drawBitmap(
    (display.width() - (animatePointX+11)),
    (display.height() - (spawnTY)),
    asteroidBase, 11, 11, INVERSE);
  }


//DRAWS ABOUT SCREEN WITH INFO CRAWL
  void drawInfo(){
    buttonChecks();
    display.setCursor(22 - (aboutInfo > 1700 ? 0 : aboutInfo), 32);
    display.print(F("Starblazers   by Alexander Alvarez <ameyeralvarez@gmail.com>   <https://github.com/ameigen>  ")); 
    aboutInfo += 4;
    display.drawBitmap(64,0, head, 53, 59, INVERSE);
    if(shootIng){
      menuBeep();
      curScreen = menuScreen;
      aboutInfo = 0;
    }
  }

//TAKES RANDOM SIGNAL INPUT(A0) AS SEED THEN WILL FILL ARRAY OF STAR POSITIONS
  void spawnStars(){
  if(starStart == 1){
    for(int i = 0; i <= 26; i+=2){
      randomSeed(analogRead(A0));
      starPos[i] = random()%65;
      randomSeed(analogRead(A0));
      starPos[i+1] = random()%129; 
      }
      starStart = 0;
    }
    for(int x = 0; x <= 26; x+=2){
     display.drawBitmap(
      ((starPos[x+1])),
      ((starPos[x])),
    star, 3, 3, WHITE);
    }
  }

//RANDOMLY DECLARE ENEMY SPAWN
  void asteroidSpawn(){
   if(spawnTimer >= 100){
      int num = random(0,5);
      enemiesArr[num] = 1;
      randomSeed(analogRead(A0));
      int spawnX = ((random()%118)+11); 
      enemiesArr[num] = 1;
      enemiesArr[num+12] = spawnX;
      spawnTimer = 0;
  }
 }

//PUSHES ASTEROID DOWN SCREEN, THEN CHECKS FOR POSSIBLE COLLISION
 void asteroidMove(){
  if(currentScore>= 1000)asteroidMoveSpeed = currentScore/1000;
  else asteroidMoveSpeed = 1;
  for(int i = 0; i <= 5; i++){
      enemiesArr[i+6] += asteroidMoveSpeed;
    if((collisionCheckP(playerShotX, playerShotY))){
        boomBeep();
        enemiesArr[i] = 0;
        enemiesArr[i+6] = 11;
        enemiesArr[i+12] = 0;
        shotOut = false;
        playerShotX = -5;
        playerShotY = -5;
        addScore += 50;
        for(int i = 0; i <= 17; i++){
         enemiesArr[i] = enemiesArrInit[i];
           }
        spawnTimer = 99;
        }
    if(enemiesArr[i] == 1){
      if(enemiesArr[i+6] >= 53){
        for(int i = 0; i <= 17; i++){
         enemiesArr[i] = enemiesArrInit[i];
           }
        if(playerLives == 1){
        curScreen = overScreen;
        }
        else playerLives -= 1;
        spawnTimer = 99;
      }
      }
    }
  }

//CHECKS ASTEROID HOLDER ARRAY FOR CURRENT LOCATION
 void drawAsteroid(){
  for(int i = 0; i <= 6; i++){
    if(enemiesArr[i] == 1){
      display.drawBitmap(enemiesArr[i+12],enemiesArr[i+6],asteroidBase, 11, 11, WHITE);
       }
    }
  }
  
//COMPARES PLAYERSHOT LOCATION TO ASTEROID LOCATION
 bool collisionCheckP(int sX,int sY){
  for(int i = 0; i <= 5; i++){
    if(enemiesArr[i] == 1){
  if((sX+1 > enemiesArr[i+12] && sX+1 < ((enemiesArr[i+12])+11) && sY+1 < ((enemiesArr[i+6])+11) && sY+1 > enemiesArr[i+6]))
  {
       return true;
          }
  else return false;
       }
    }
 }
