#include <EEPROM.h>

boolean show_highscore = false;
short highscore_newindex = -1;
short highchain_newindex = -1;

#define NUM_HIGHSCORE 5
int highscore[NUM_HIGHSCORE] = { 0, 0, 0, 0, 0};
int highchain[NUM_HIGHSCORE] = { 0, 0, 0, 0, 0};

void InitHighscore() {
  for(byte thisScore = 0; thisScore < NUM_HIGHSCORE; thisScore++) {
    highscore[thisScore] = 0;

//    highscore[thisScore] = EEPROM.read(NAMELENGTH + thisScore*(NAMELENGTH+2)) & 0x00FF; //LSB
//    highscore[thisScore] += (EEPROM.read(NAMELENGTH+1 + thisScore*(NAMELENGTH+2)) << 8) & 0xFF00; //MSB
//    highscore[thisScore] = (highscore[thisScore]==0) ? 9999 : highscore[thisScore];

#if SAVE_EEPROM
    //read EEPROM  
    highscore[thisScore] = EEPROM.read(thisScore*2) & 0x00FF;
    highscore[thisScore] += (EEPROM.read(1 + (thisScore*2)) << 8) & 0xFF00; //MSB
    highscore[thisScore] = (highscore[thisScore]==0 || !highscore[thisScore]) ? 0 : highscore[thisScore];
#endif
  }

  for(byte thisScore = 0; thisScore < NUM_HIGHSCORE; thisScore++) {
    highchain[thisScore] = 0;

#if SAVE_EEPROM
    //read EEPROM  
    highchain[thisScore] = EEPROM.read((NUM_HIGHSCORE * 2) + (thisScore*2)) & 0x00FF;
    highchain[thisScore] += (EEPROM.read((NUM_HIGHSCORE * 2) + 1 + (thisScore*2)) << 8) & 0xFF00; //MSB
    highchain[thisScore] = (highchain[thisScore]==0 || !highchain[thisScore]) ? 0 : highchain[thisScore];
#endif
  }

  initialize_highscore = true;
}

void SaveAllHighscore(int score, int chain) {
  SaveHighscore(highscore, &highscore_newindex, score, 0);
  SaveHighscore(highchain, &highchain_newindex, chain, (NUM_HIGHSCORE * 2));

#if SAVE_EEPROM
  if (highscore_newindex != -1 || highchain_newindex != -1) {
    gb.popup(F("HIGHSCORE SAVED!"), 30);
  }
#endif
}

void SaveHighscore(int scores[], short *index, int score, int eeprom_index) {
  int tmpCurrentScore = 0;
  int tmpLastScore = 0;

  *index = -1;

  if(score > scores[NUM_HIGHSCORE - 1]) { 
    //if it's an highscore
    for(byte thisScore = 0; thisScore < NUM_HIGHSCORE; thisScore++) {
      if (*index == -1 && scores[thisScore] <= score) {
        tmpLastScore = scores[thisScore]; 
        scores[thisScore] = score;

        *index = thisScore;
      }
      else if (scores[thisScore] <= score) {
        tmpCurrentScore = scores[thisScore];
        scores[thisScore] = tmpLastScore;
        tmpLastScore = tmpCurrentScore;
      }
    }

//        EEPROM.write(NAMELENGTH + thisScore*(NAMELENGTH+2), highscore[thisScore] & 0x00FF); //LSB
//        EEPROM.write(NAMELENGTH+1 + thisScore*(NAMELENGTH+2), (highscore[thisScore] >> 8) & 0x00FF); //MSB

#if SAVE_EEPROM
    //save EEPROM
    for(byte thisScore = 0; thisScore < NUM_HIGHSCORE; thisScore++){
      EEPROM.write(eeprom_index + (thisScore*2), scores[thisScore] & 0x00FF); //LSB
      EEPROM.write(eeprom_index + 1 + (thisScore*2), (scores[thisScore] >> 8) & 0x00FF); //MSB
    }
#endif
  }
}

void DrawHighscore() {
  String rank = "";

  gb.display.cursorX = 16;
  gb.display.cursorY = 1;
  if (game_show_highscore) {
    gb.display.print(F("-HIGH SCORES-"));
  }
  else {
    gb.display.print(F("-HIGH CHAINS-"));
  }

  for(int thisScore = 0; thisScore < NUM_HIGHSCORE; thisScore++) {
    gb.display.cursorX = 3;
    gb.display.cursorY = (8 + (thisScore * 6));

    switch(thisScore) {
    case 0 : 
      rank = String(thisScore + 1)+"ST"; 
      break;
    case 1 : 
      rank = String(thisScore + 1)+"ND"; 
      break;
    case 2 : 
      rank = String(thisScore + 1)+"RD"; 
      break;
    case 3 : 
      rank = String(thisScore + 1)+"TH"; 
      break;
    case 4 : 
      rank = String(thisScore + 1)+"TH"; 
      break;
    } 

    if (game_show_highscore) {  
      if (highscore_newindex != -1 && highscore_newindex == thisScore) {
        gb.display.print(rank + " - " + highscore[thisScore] + "(NEW)");
      }
      else {
        gb.display.print(rank + " - " + highscore[thisScore]);
      }
    }
    else {
      if (highchain_newindex != -1 && highchain_newindex == thisScore) {
        gb.display.print(rank + " - " + highchain[thisScore] + "(NEW)");
      }
      else {
        gb.display.print(rank + " - " + highchain[thisScore]);
      }
    }
  }

  gb.display.cursorX = 0;
  gb.display.cursorY = 40;
  gb.display.print(F("\x16:accept"));

  if(gb.buttons.pressed(BTN_B)){
    gb.sound.playOK();

    if (game_show_highscore) {
      highscore_newindex = -1;
      game_show_highscore = false;
    }
    else {
      highchain_newindex = -1;
      game_highscore = false;
    }
  }
}

