void DrawPlayer() {
  if (!player_explosed) {
    //Draw rectangle (DEBUG)
    //gb.display.fillRect(player_x, player_y, player_w, player_h);
    gb.display.drawBitmap(player_x, player_y, spaceship);
  }
  else {
    DrawPlayerExplosion();
  }
}

void DrawPlayerExplosion() {
  if (game_animation_counter > 0) {
    //TODO improve explosion
    //gb.display.drawBitmap(player_x, player_y, spaceship);
    DrawExplosion(player_x, player_y, game_animation_counter);
    
    game_animation_counter--;
  }
  else {
    game_over = true;//END GAME
  }
}

void MovePlayer() {
  //move the player
  if(gb.buttons.repeat(BTN_RIGHT, 1)){
    player_x = min(field_x2 - player_w, player_x + player_vx);
  }
  if(gb.buttons.repeat(BTN_LEFT, 1)){
    player_x = max(field_x1 + 1, player_x - player_vx);
  }
  if(gb.buttons.repeat(BTN_UP, 1)){
    player_y = max(1, player_y - player_vy);
  }
  if(gb.buttons.repeat(BTN_DOWN, 1)){
    player_y = min(LCDHEIGHT - player_h, player_y + player_vy);
  }
  //if(gb.buttons.repeat(BTN_A, 1)) && !gb.buttons.repeat(BTN_B, 1)){
  if(gb.buttons.repeat(BTN_A, 5)) {
    PlayerShoot();
  }
  //TODO
  //  if(gb.buttons.repeat(BTN_B, 5)) {
  //    PlayerMegaBomb();
  //  }
}

void MoveBullets() {
  for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++) {
    if (bullets_active[thisBullet]) {
      bullets_y[thisBullet] = bullets_y[thisBullet] - bullets_vy;

      //gb.display.drawPixel (bullets_x[thisBullet], bullets_y[thisBullet]);
      gb.display.fillRect(bullets_x[thisBullet], bullets_y[thisBullet], 1, 3);

      // CheckCollision
      if (CheckInvaderCollision(bullets_x[thisBullet], bullets_y[thisBullet])) {
        bullets_active[thisBullet] = false;
      }

      if (bullets_y[thisBullet] <= 0) {
        bullets_active[thisBullet] = false;
        bullets_x[thisBullet] = 0;
        bullets_y[thisBullet] = 0;
      }
    }
  }
}

void PlayerShoot() {
  for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++) {
    if (!bullets_active[thisBullet]) {
      // new bullet
      bullets_x[thisBullet] = player_x + (player_w / 2);
      bullets_y[thisBullet] = player_y;// + 2;

      PlaySoundFxPlayerShoot();

      bullets_active[thisBullet] = true;
      break;
    }
  }
}

void PlayerMegaBomb() {
}

boolean CheckPlayerCollision(int x, int y) {
  boolean collision = false;

  int x1 = player_x + 2;
  int x2 = player_x + player_w - 2;
  int y1 = player_y;// + 2;
  int y2 = player_y + player_h;// - 2;
  
  //TODO collision with object
  if (!player_explosed && (x >= x1 && x <= x2 && y <= y2 && y >= y1)) {
    collision = true;
    
    if ((player_life - 1) >= 0) {
      player_life--;
    }
    
    if (player_life == 0) {
      player_explosed = true;
      //game_over = true;//TODO
      
      PlaySoundFxGameOver();
    }
    
    UpdateGameScore(0, false);
  }

  return collision;
}

