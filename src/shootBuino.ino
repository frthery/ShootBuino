//imports the SPI library (needed to communicate with Gamebuino's screen)
#include <SPI.h>
//importe the Gamebuino library
#include <Gamebuino.h>

//creates a Gamebuino object named gb
Gamebuino gb = Gamebuino();

#define SAVE_EEPROM 0 //0:false|1:true

//declare all the sprites which are in the "sprites" tab
extern const byte PROGMEM logo[];

extern const byte PROGMEM score_0[];
extern const byte PROGMEM score_1[];
extern const byte PROGMEM score_2[];
extern const byte PROGMEM score_3[];
extern const byte PROGMEM score_4[];
extern const byte PROGMEM score_5[];
extern const byte PROGMEM score_6[];
extern const byte PROGMEM score_7[];
extern const byte PROGMEM score_8[];
extern const byte PROGMEM score_9[];

extern const byte PROGMEM spaceship[];
extern const byte PROGMEM invader_0[];
extern const byte PROGMEM invader_explose_0[];
extern const byte PROGMEM invader_1[];
extern const byte PROGMEM invader_explose_1[];

//game variables
boolean initialize = false;
boolean initialize_highscore = false;
int game_frame_rate = 20;//default 20
boolean game_menu = false;
boolean game_highscore = false;
boolean game_show_highscore = true;
boolean game_over = false;

short game_force_level = 0;
short game_level = 1;//1,2,3,4,5,6,7,8,9 Levels
short game_menu_level = game_level;
#define GAME_LEVEL_MAX 9
int levels[GAME_LEVEL_MAX] = { 0, 1000, 2000, 4000, 6000, 8000, 10000, 15000, 20000 };

const int game_delai = 11;//increase delai=slower
int game_score = 0;
int chain_combo = 0;
int max_chain_combo = 0;
//boolean has_megabomb = true;

uint8_t game_animation_default_counter = 5;
uint8_t game_animation_counter = game_animation_default_counter;
//END game variables

//player variables
#define PLAYER_H 8
#define PLAYER_W 8
short player_h = PLAYER_H;
short player_w = PLAYER_W;
float player_x = (LCDWIDTH / 2) - (PLAYER_W / 2);
float player_y = LCDHEIGHT - PLAYER_H;
const float player_vy = 2;
const float player_vx = 2;
const uint8_t player_life_default = 10;
uint8_t player_life = player_life_default;
boolean player_explosed = false;

#define NUMBULLETS 10
int bullets_x[NUMBULLETS];
int bullets_y[NUMBULLETS];
//const float bullets_vx = 2;
const float bullets_vy = 2.5;
boolean bullets_active[NUMBULLETS];
short last_invader_shoot = -1;
//END player variables

//invaders variables
short invader_h = 8;
short invader_w = 8;

#define NUMINVADERS 10
boolean invaders_levelup = false;
int invaders_x[NUMINVADERS];
int invaders_y[NUMINVADERS];
float invaders_v = 0.2;
const float invaders_default_vy = 1;
float invaders_vy = invaders_default_vy;
const short invaders_default_shoot_delai = 5;
short invaders_shoot_delai = invaders_default_shoot_delai;
boolean invaders_active[NUMINVADERS];
int invaders_explosed[NUMINVADERS][4];//type,x,y,animation
int invaders_type[NUMINVADERS];
int nbActiveInvaders = 0;

#define NUMINVBULLETS 12
int invaders_bullets_x[NUMINVBULLETS];
int invaders_bullets_y[NUMINVBULLETS];
float invaders_bullets_v = 0.25;
const float invaders_default_bullets_vy = 1;
float invaders_bullets_vy = invaders_default_bullets_vy;
boolean invaders_bullets_active[NUMINVBULLETS];
//END invaders variables

//field variables
const int field_h = LCDHEIGHT;
const int field_w = 1;
const int field_x1 = 15;
const int field_x2 = 69;
const int field_y = 0;
const int field_play = field_x2 - field_x1;
//END field variables

//the setup routine runs once when Gamebuino starts up
void setup(){
  //initialize the Gamebuino object
  gb.begin();
  gb.pickRandomSeed();
  
  //display the main menu
  gb.titleScreen(logo);

  gb.battery.show = false;//hide the battery indicator
}

//the loop routine runs over and over again forever
void loop(){
  if (!initialize) {
    InitGame();
    gb.setFrameRate(game_frame_rate);
  }

  if(gb.update()){
    if (!game_over && !game_menu && !game_highscore) {
      if(gb.buttons.pressed(BTN_C)){
        gb.sound.playCancel();
        game_menu = true;
      }

      Play();
    }
    else if (game_menu) {
      GameMenu();
    }
    else if (game_highscore) {
      DrawHighscore();
    }
    else if (game_over) {
      GameOver();
    }
  }
}

void InitGame() {
  if (!initialize_highscore) {
    InitHighscore();
  }

  //initialize Game
  for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++){
    bullets_active[thisBullet] = false;
    bullets_x[thisBullet] = 0;
    bullets_y[thisBullet] = 0;
  }

  for(byte thisInvader = 0; thisInvader < NUMINVADERS; thisInvader++){
    invaders_active[thisInvader] = false;
    invaders_type[thisInvader] = 0;
    invaders_x[thisInvader] = 0;
    invaders_y[thisInvader] = 0;
    invaders_explosed[thisInvader][0] = 0;
    invaders_explosed[thisInvader][1] = 0;
    invaders_explosed[thisInvader][2] = 0;//inactive
  }
  invaders_vy  = invaders_default_vy;
  invaders_levelup = false;
  invaders_shoot_delai = invaders_default_shoot_delai;

  nbActiveInvaders = 0;

  for(byte thisBullet = 0; thisBullet < NUMINVBULLETS; thisBullet++){
    invaders_bullets_active[thisBullet] = false;
    invaders_bullets_x[thisBullet] = 0;
    invaders_bullets_y[thisBullet] = 0;
  }
  invaders_bullets_vy  = invaders_default_bullets_vy;

  //reset Player position
  player_h = PLAYER_H;
  player_w = PLAYER_W;
  player_x = (LCDWIDTH / 2) - (PLAYER_W / 2);
  player_y = LCDHEIGHT - player_h;
  player_explosed = false;
  
  last_invader_shoot = -1;
  max_chain_combo = 0;
  game_score = 0;
  chain_combo = 0;
  if (game_force_level > 0) {
    game_level = game_force_level;
  }
  else if (game_menu_level != 1) {
    game_level = game_menu_level;
  }
  else {
    game_level = 1; // reset game_level
  }
  invaders_levelup = true;
  game_menu_level = game_level;
  game_show_highscore = true;

  game_animation_counter = game_animation_default_counter;

  initialize = true;
}

void Play() {
  if (!player_explosed) {
    MovePlayer();
  }
  
  MoveInvaders();
  MoveInvadersBullets();
  MoveBullets();

  DrawField();
  DrawPlayer();
  DrawInvaders();

  DrawScore();
}

void GameOver() {
  gb.display.cursorX = 22;
  gb.display.cursorY = 1;
  gb.display.print("!GAME OVER!");

  gb.display.cursorX = 0;
  gb.display.cursorY = 10;
  gb.display.print("Score: ");
  gb.display.print(game_score);

  gb.display.cursorX = 0;
  gb.display.cursorY = 20;
  gb.display.print("Max. Chain: ");
  gb.display.print(max_chain_combo);

  gb.display.cursorX = 0;
  gb.display.cursorY = 40;
  gb.display.print(F("\x16:accept"));
  if(gb.buttons.pressed(BTN_B)){
    gb.sound.playOK();

    SaveAllHighscore(game_score, max_chain_combo);

    game_highscore = true;
    initialize = false;
    game_over = false;
  }
}

void GameMenu() {
  gb.display.cursorX = 5;
  gb.display.cursorY = 1;
  gb.display.print("-CHOOSE GAME LEVEL-");

  gb.display.fillTriangle(30, 10, 25, 15, 35, 15);
  gb.display.fillTriangle(30, 28, 25, 23, 35, 23);

  gb.display.cursorX = 0;
  gb.display.cursorY = 17;
  gb.display.print("LEVEL: " + String(game_menu_level));

  gb.display.cursorX = 0;
  gb.display.cursorY = 40;
  gb.display.print(F("\x15:accept \x16:cancel"));

  if(gb.buttons.pressed(BTN_UP)){
    if ((game_menu_level + 1) <= GAME_LEVEL_MAX) {
      game_menu_level++;
    }
  }
  if(gb.buttons.pressed(BTN_DOWN)){
    if ((game_menu_level - 1) >= 1) {
      game_menu_level--;
    }
  }
  if(gb.buttons.pressed(BTN_A)){
    gb.sound.playOK();

    //initialize = false;
    game_level = game_menu_level;
    initialize = false;
    game_menu = false;
  }
  if(gb.buttons.pressed(BTN_B)){
    gb.sound.playOK();

    game_menu_level = game_level;
    //initialize = false;
    game_menu = false;
  }
  if(gb.buttons.pressed(BTN_C)){
    gb.sound.playOK();
    
    gb.changeGame();
  }
}

void GamePause() {
  gb.display.cursorX = 5;
  gb.display.cursorY = 1;
  gb.display.print(F("-GAME PAUSE-"));

  gb.display.cursorX = 0;
  gb.display.cursorY = 40;
  gb.display.print(F("\x16:accept"));

  if(gb.buttons.pressed(BTN_B)){
    gb.sound.playOK();

    game_menu = false;
  }
}

void DrawField() {
  //draw the field
  gb.display.drawLine(field_x1, field_y, field_x1, LCDHEIGHT);
  gb.display.drawLine(field_x2, field_y, field_x2, LCDHEIGHT);
}

void DrawScore() {
  //draw the score
  //gb.display.setTextSize(1);
  //gb.display.setCursor(0, 0);
  //gb.display.print(game_score);

  DrawGraphicalScore();
  DrawChain();
  //DrawPlayerLife();
  //DrawMegaBomb();
}

void DrawChain() {
  //gb.display.fontSize = 1;
  gb.display.cursorX = 72;
  gb.display.cursorY = 5;
  gb.display.print(F("ch."));
  gb.display.cursorX = 72;
  gb.display.cursorY = 13;
  gb.display.print(String(chain_combo));

  if (last_invader_shoot == 0) {
    gb.display.drawBitmap(72, 20, invader_0);
  }
  else if (last_invader_shoot == 1) {
    gb.display.drawBitmap(72, 20, invader_1);
  }

  gb.display.cursorX = 72;
  gb.display.cursorY = 40;
  if (game_level < GAME_LEVEL_MAX) {
    gb.display.print(String("S.")+""+String(game_level));
  }
  else {
    gb.display.print(String("Max"));
  }
}

//void DrawMegaBomb() {
//  if (has_megabomb) {
//    gb.display.setCursor(5, 40);
//    gb.display.print("B!");
////    gb.display.drawBitmap(72, 20, invader_1);
//  }
//}

void DrawPlayerLife() {
  //TODO draw life
  gb.display.cursorX = 60;
  gb.display.cursorY = 3;
  gb.display.print("L");
  gb.display.drawRect(65, 3, player_life_default, 4);
  //gb.display.fillRect((73 + (3 - player_life) * 3), 3, (3 * player_life), 4);
  gb.display.fillRect((65 + (player_life_default - player_life)), 3, player_life, 4);
} 

void DrawGraphicalScore() {
  char buf[6];
  sprintf(buf, "%06i", game_score);

  int d1, d2, d3, d4, d5, d6; // 6 digits MAX score 999999

  d1 = buf[0] - '0';
  d2 = buf[1] - '0';
  d3 = buf[2] - '0';
  d4 = buf[3] - '0';
  d5 = buf[4] - '0';
  d6 = buf[6] - '0';

  DrawDigit(0, d6);
  DrawDigit(1, d5);
  DrawDigit(2, d4);
  DrawDigit(3, d3);
  DrawDigit(4, d2);
  DrawDigit(5, d1);
}

void DrawExplosion(int x, int y, int animation_counter) {
  int tx = -1;
  //int y = -1;
  
  DrawExplosionPixel((x + ((10 - animation_counter) / 3)), y);
  DrawExplosionPixel((x - ((10 - animation_counter) / 3)), y);
  DrawExplosionPixel(x, (y + ((10 - animation_counter) / 3)));
  DrawExplosionPixel(x, (y - ((10 - animation_counter) / 3)));
  DrawExplosionPixel((x + ((10 - animation_counter) / 2)), (y + ((10 - animation_counter) / 3)));
  DrawExplosionPixel((x + ((10 - animation_counter) / 3)), (y + ((10 - animation_counter) / 2)));
  DrawExplosionPixel((x - ((10 - animation_counter) / 2)), (y - ((10 - animation_counter) / 3)));
  DrawExplosionPixel((x - ((10 - animation_counter) / 3)), (y - ((10 - animation_counter) / 2)));
  
  /*gb.display.drawPixel(x + ((10 - animation_counter) / 3), y);
  gb.display.drawPixel(x - ((10 - animation_counter) / 3), y);
  gb.display.drawPixel(x, y + ((10 - animation_counter) / 3));
  gb.display.drawPixel(x, y - ((10 - animation_counter) / 3));
  gb.display.drawPixel(x + ((10 - animation_counter) / 2), y + ((10 - animation_counter) / 3));
  gb.display.drawPixel(x + ((10 - animation_counter) / 3), y + ((10 - animation_counter) / 2));
  gb.display.drawPixel(x - ((10 - animation_counter) / 2), y - ((10 - animation_counter) / 3));
  gb.display.drawPixel(x - ((10 - animation_counter) / 3), y - ((10 - animation_counter) / 2));*/
}

void DrawExplosionPixel(int x, int y) {
  //check if pixel is out of field
  if (x >= field_x1 && x <= field_x2) {
    gb.display.drawPixel(x, y);
  }
}

void DrawDigit(int index, int value) {
  switch(value) {
  case 1 : 
    gb.display.drawBitmap(6, index * 5, score_1);
    break;
  case 2 : 
    gb.display.drawBitmap(6, index * 5, score_2);
    break;
  case 3 : 
    gb.display.drawBitmap(6, index * 5, score_3);
    break;
  case 4 : 
    gb.display.drawBitmap(6, index * 5, score_4);
    break;
  case 5 : 
    gb.display.drawBitmap(6, index * 5, score_5);
    break;
  case 6 : 
    gb.display.drawBitmap(6, index * 5, score_6);
    break;
  case 7 : 
    gb.display.drawBitmap(6, index * 5, score_7);
    break;
  case 8 : 
    gb.display.drawBitmap(6, index * 5, score_8);
    break;
  case 9 : 
    gb.display.drawBitmap(6, index * 5, score_9);
    break;
  default :
    gb.display.drawBitmap(6, index * 5, score_0);
  }
}

void UpdateGameScore(int value, boolean combo) {
  int bonus = 0;

  if (combo) {
    chain_combo++; // continue
  }
  else {
    if (chain_combo > max_chain_combo) {
      max_chain_combo = chain_combo;
    }

    bonus = chain_combo * 10;  // bonus combo
    chain_combo = 0;
  }

  game_score += (bonus + value);

  if (game_score > levels[game_level]) {
    if (game_level < GAME_LEVEL_MAX) {
      invaders_levelup = true;
      game_level++;
    }
  }
}

int GetGameSpeed() {
  if (game_level == 1) {
    return game_delai;
  }
  else {
    return game_delai - game_level;
  }
}

void ShowFrame() {
  //declare a variable named count of type integer :
  int count;
  //get the number of frames rendered and assign it to the "count" variable
  count = gb.frameCount;
  //prints the variable "count"
  //gb.display.setTextSize(0.5);
  gb.display.cursorX = 1;
  gb.display.cursorY = 1;
  ShowDebug(String(count));
}

void ShowDebug(String message) {
  //draw debug line
  //gb.display.setTextSize(1);
  gb.display.cursorX = 1;
  gb.display.cursorY = 1;
  gb.display.print("[ " + message + " ]");
}


