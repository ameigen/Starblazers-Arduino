#define MUSIC_H
const int speakPin = 7;
int music[] = {230,138,392,392,440,440,392,349,349,329,329,293,293,330,392,392,349,349,329,329,293,293,293,293};
int musicLoop = 0;
float freqSet = 0;
int beatSet = 0;
void musicInit() {
  freqSet = 1;
  beatSet = 900;
}

  int playMusic(float freq,int beat){
   tone(speakPin, (music[musicLoop]*freq), beat);
   musicLoop+= 1;
   if (musicLoop> 23)
     {
      musicLoop = 0;
     }  
}

  int menuBeep(){
    tone(speakPin,2000,100);
  }

  int shootBeep(){
    tone(speakPin,100,100);
  }

  int boomBeep(){
    tone(speakPin,100,10);
    tone(speakPin,50,10);
    tone(speakPin,100,10);
    tone(speakPin,50,10);
  }
