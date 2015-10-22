const int soundfx[9][8] = {
  {1,57,57,1,1,1,5,6}, // 0 = sound shoot (channel 0) (SND_FX_PLAYER_SHOOT)
  {0,0,68,1,0,0,7,4}, // 1 = sound invader hit (channel 0) (SND_FX_INVADER_HIT_1)
  {1,15,57,1,1,2,7,15}, // 2 = sound player hit (channel 2) (SND_FX_INVADER_HIT_2)
  {0,10,60,1,0,0,7,6},  // 3 = sound saucer (channel 0)
  {0,5,58,0,1,5,5,2}, // 4 = sound invaders 1 (channel 1)
  {0,4,58,0,1,5,5,2}, // 5 = sound invaders 2 (channel 1)
  {0,2,58,0,1,5,5,2}, // 6 = sound invaders 3 (channel 1)
  {0,1,58,0,1,5,5,2}, // 7 = sound invaders 4 (channel 1)
  //{0,14,46,6,0,0,7,11} // SND_FX_GAME_OVER
  {0,30,34,10,0,1,7,25} // SND_FX_GAME_OVER
};

//sounds
enum soundsFxChannel_t {
  SND_FX_CHANNEL_1 = 0,
  SND_FX_CHANNEL_2 = 1,
  SND_FX_CHANNEL_3 = 2,
  SND_FX_CHANNEL_4 = 3
};

//TODO
enum soundsFx_t {
  SND_FX_PLAYER_SHOOT = 0,
  SND_FX_INVADER_HIT_1 = 1,
  SND_FX_INVADER_HIT_2 = 2,
  SND_FX_GAME_OVER = 8
};

void PlaySoundFxPlayerShoot() {
  gb.sound.playTick();
  //PlaySoundFx(SND_FX_PLAYER_SHOOT, SND_FX_CHANNEL_1);
}

void PlaySoundFxInvaderHit() {
  //PlaySoundFx(SND_FX_INVADER_HIT_1, SND_FX_CHANNEL_1);
}

void PlaySoundFxGameOver() {
  PlaySoundFx(SND_FX_GAME_OVER, SND_FX_CHANNEL_1);
}

//Play Gambuino sound effect (http://www.yodasvideoarcade.com/gamebuino.php)
void PlaySoundFx(int fxno, int channel) {
  gb.sound.command(0,soundfx[fxno][6],0,channel); // set volume
  gb.sound.command(1,soundfx[fxno][0],0,channel); // set waveform
  gb.sound.command(2,soundfx[fxno][5],-soundfx[fxno][4],channel); // set volume slide
  gb.sound.command(3,soundfx[fxno][3],soundfx[fxno][2]-58,channel); // set pitch slide
  gb.sound.playNote(soundfx[fxno][1],soundfx[fxno][7],channel); // play note
}

