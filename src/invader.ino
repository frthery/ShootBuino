void DrawInvader(int x, int y, int type) {
  //draw rectangle (DEBUG)
  //gb.display.fillRect(x, y, player_w, player_h);

  if (type == 1) {
    gb.display.drawBitmap(x, y, invader_1);
  }
  else {
    gb.display.drawBitmap(x, y, invader_0);
  }
}

void DrawInvaderExplosion(int index) {
  if (invaders_explosed[index][0] == 1) {
    gb.display.drawBitmap(invaders_explosed[index][1], invaders_explosed[index][2], invader_explose_1);
  }
  else {
    gb.display.drawBitmap(invaders_explosed[index][1], invaders_explosed[index][2], invader_explose_0);
  }
  
  //TODO improve explosion
  //DrawExplosion(invaders_explosed[index][1], invaders_explosed[index][2], invaders_explosed[index][3]);
    
  invaders_explosed[index][3]--;
}

void DrawInvaders() {
  for(byte thisInvader = 0; thisInvader < NUMINVADERS; thisInvader++) {
    if (invaders_active[thisInvader]) {
      DrawInvader(invaders_x[thisInvader], invaders_y[thisInvader], invaders_type[thisInvader]);

      //check Player collision (center)
      if (CheckPlayerCollision(invaders_x[thisInvader] + (invader_w / 2), invaders_y[thisInvader] + (invader_h / 2)))
        break;
    } 
    else if (invaders_explosed[thisInvader][3] > 0) {
      //DRAW animation explosion
      DrawInvaderExplosion(thisInvader);
    }
  }
}

void MoveInvaders() {
  int posX = 0;
  int posY = 0;

  int type = 0;
  int reverse = random(0, 2);//invaders right(true) : left(left)
  int align = random(0, 2);//invaders horizontal align : true : false
  boolean color = false;//true : white | false : black

  //when levelup, random type invaders
  if (game_level <= 2) {
    type = random(0, 3);
  }
  else if (game_level <= 5) {
    type = random(0, 4);
  }
  else if (game_level < 8) {
    type = random(0, 5);
  }
  else if (game_level >= 8) {
    type = random(0, 6);
  }

  //type = 5;//FORCE

  //color depends on last invader color shoot
  if (last_invader_shoot == 1) {
    color = true;
  }
  else {
    color = false;
  }

  //speed limite
  if (GetGameSpeed() == 0 || ((gb.frameCount % GetGameSpeed()) == 0)) {
    for(byte thisInvader = 0; thisInvader < NUMINVADERS; thisInvader++) {
      if (invaders_active[thisInvader]) { 
        invaders_y[thisInvader] = invaders_y[thisInvader] + invaders_vy;//(invaders_vy + (game_level * invaders_v));

        if (invaders_y[thisInvader] > LCDHEIGHT) {
          invaders_x[thisInvader] = 0;
          invaders_y[thisInvader] = 0;
          invaders_active[thisInvader] = false;
          nbActiveInvaders--;
        }
        else {
          InvaderShoot(invaders_x[thisInvader], invaders_y[thisInvader]);
        }
      }
    }

    if (nbActiveInvaders == 0) {
      if (invaders_levelup) {
        invaders_vy = invaders_default_vy + (game_level * invaders_v);
        invaders_bullets_vy = invaders_default_bullets_vy + (game_level * invaders_bullets_v);
        //invaders_shoot_delai = 5;// TODO invaders_shoot_delai

        invaders_levelup = false;
      }

      switch(type)  {
      case 1 :
        posX = random(((reverse == 0) ? field_x1 : (field_x1 + (invader_w * 2))), ((reverse == 0) ? (field_x2 - (invader_w * 2)) : (field_x2 - invader_w)));
        LaunchInvader(0, 0, posX, posY, (reverse == 1), (align == 1), color);
        LaunchInvader(1, 1, posX, posY, (reverse == 1), (align == 1), !color);
        break;
      case 2 :
        posX = random(((reverse == 0) ? field_x1 : (field_x1 + (invader_w * 3))), ((reverse == 0) ? (field_x2 - (invader_w * 3)) : (field_x2 - invader_w)));
        LaunchInvader(0, 0, posX, posY, (reverse == 1), (align == 1), !color);
        LaunchInvader(1, 1, posX, posY, (reverse == 1), (align == 1), color);
        LaunchInvader(2, 2, posX, posY, (reverse == 1), (align == 1), !color);
        break;
      case 3 :
        posX = random(((reverse == 0) ? field_x1 : (field_x1 + (invader_w * 4))), ((reverse == 0) ? (field_x2 - (invader_w * 4)) : (field_x2 - invader_w)));
        LaunchInvader(0, 0, posX, posY, (reverse == 1), (align == 1), color);
        LaunchInvader(1, 1, posX, posY, (reverse == 1), (align == 1), !color);
        LaunchInvader(2, 2, posX, posY, (reverse == 1), (align == 1), color);
        LaunchInvader(3, 3, posX, posY, (reverse == 1), (align == 1), !color);
        break;
      case 4 :
        posX = field_x1 + 2;
        LaunchInvader(0, 0, posX, posY, 0, (align == 1), color);
        LaunchInvader(1, 1, posX, posY, 0, (align == 1), !color);
        LaunchInvader(2, 2, posX, posY, 0, (align == 1), color);

        posX = field_x2 - (invader_w * 3) - 1;
        LaunchInvader(3, 0, posX, posY, 0, (align == 1), !color);
        LaunchInvader(4, 1, posX, posY, 0, (align == 1), color);
        LaunchInvader(5, 2, posX, posY, 0, (align == 1), !color);
        break;
      case 5 :
        posX = random(((reverse == 0) ? field_x1 : (field_x1 + ((invader_w * 3)  + (invader_w / 2)))), ((reverse == 0) ? (field_x2 - ((invader_w * 3) + (invader_w / 2))) : (field_x2 - invader_w)));
        LaunchInvader(0, 0, posX, posY, (reverse == 1), (align == 1), !color);
        LaunchInvader(1, 1, posX, posY, (reverse == 1), (align == 1), color);
        LaunchInvader(2, 2, posX, posY, (reverse == 1), (align == 1), !color);

        posX = posX + ((reverse == 0) ? (invader_w / 2) : -(invader_w / 2));
        posY -= (align == 1) ? invader_h + 2 : invader_h + (invader_h / 2) + 1;
        LaunchInvader(3, 0, posX, posY, (reverse == 1), (align == 1), color);
        LaunchInvader(4, 1, posX, posY, (reverse == 1), (align == 1), !color);
        LaunchInvader(5, 2, posX, posY, (reverse == 1), (align == 1), color);
        break;
      default:
        posX = random(field_x1, (field_x2 - invader_w));
        LaunchInvader(0, 0, posX, posY, false, false, !color);
      }
    }
  }
}

void LaunchInvader(int index, int number, int posX, int posY, boolean reverse, boolean align, boolean color) {
  if (!reverse) {
    invaders_x[index] = posX + (number * invader_w);
    invaders_y[index] = !align ? (posY + (number * -5)) : posY;
  }
  else {
    invaders_x[index] = posX - (number * invader_w);
    invaders_y[index] = !align ? (posY + (number * -5)) : posY;
  }

  invaders_active[index] = true;
  invaders_type[index] = color ? 1 : 0;
  nbActiveInvaders++;
}

void MoveInvadersBullets() {
  for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++) {
    if (invaders_bullets_active[thisBullet]) { 
      //if (GetGameSpeed() == 0 || ((gb.getFrameCount() % (GetGameSpeed() / 2)) == 0)) {
      invaders_bullets_y[thisBullet] = invaders_bullets_y[thisBullet] + invaders_bullets_vy;
      //}

      //gb.display.drawPixel(invaders_bullets_x[thisBullet], invaders_bullets_y[thisBullet]);
      gb.display.fillRect(invaders_bullets_x[thisBullet], invaders_bullets_y[thisBullet], 1, -2);

      //check Player collision (center)
      CheckPlayerCollision(invaders_bullets_x[thisBullet], invaders_bullets_y[thisBullet]);

      if (invaders_bullets_y[thisBullet] >= LCDHEIGHT) {
        invaders_bullets_active[thisBullet] = false;
        invaders_bullets_x[thisBullet] = 0;
        invaders_bullets_y[thisBullet] = 0;
      }
    }
  }
}

int GetInvaderShootDelai() {
  if (game_level <= 5) {
    return GetGameSpeed() * invaders_shoot_delai;
  }
  else {
    return 30; // max delai (6 * 5)
  }
}

void InvaderShoot(int invaderPosX, int invaderPosY) {
  if (GetGameSpeed() == 0 || ((gb.frameCount % GetInvaderShootDelai()) == 0)) {
    for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++) {
      if (!invaders_bullets_active[thisBullet]) {
        // new bullet
        invaders_bullets_x[thisBullet] = invaderPosX + (invader_w / 2);
        invaders_bullets_y[thisBullet] = (invaderPosY + (invader_h / 2)) + 3;

        invaders_bullets_active[thisBullet] = true;
        break;
      }
    }
  }
}

boolean CheckInvaderCollision(int x, int y) {
  boolean collision = false;

  int x1 = 0;
  int x2 = 0;
  int y1 = 0;
  int y2 = 0;

  for(byte thisInvader = 0; thisInvader < NUMINVADERS; thisInvader++) {
    if (invaders_active[thisInvader]) { 
      x1 = invaders_x[thisInvader];
      x2 = invaders_x[thisInvader] + invader_w;
      y1 = invaders_y[thisInvader];
      y2 = invaders_y[thisInvader] + invader_h;
      if ( x >= x1 && x <= x2 && y <= y2 && y >= y1) {
        invaders_active[thisInvader] = false;
        nbActiveInvaders--;

        PlaySoundFxInvaderHit();
        InvaderExplosed(thisInvader, invaders_x[thisInvader], invaders_y[thisInvader], invaders_type[thisInvader]);
        
        if (invaders_type[thisInvader] != last_invader_shoot) {
          UpdateGameScore(10, true);
        }
        else {
          UpdateGameScore(10, false);
        }

        last_invader_shoot = invaders_type[thisInvader];

        //TODO megabomb

        collision = true;
        break;
      }
    }
  }

  return collision;
}

void InvaderExplosed(int index, int x, int y, int type) {
  invaders_explosed[index][0] = type;
  invaders_explosed[index][1] = x;
  invaders_explosed[index][2] = y;
  invaders_explosed[index][3] = game_animation_counter;//set animation duration
}

