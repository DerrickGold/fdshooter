#include <stdio.h>
#include <math.h>
#include "globals.h"

#define FLYSPEED 8.0f
#define DECELSPEED 0.1f

SDL_Event GlobalEvents;

int flyingMovement(struct Enemy *e, void *whatever);

void initData(void) {

  for (int i = 0; i < ENEMIES_PER_LEVEL; i++)
  GlobalLevels[0].enemy_template[i] = 
    (Enemy) { .health = 1, .movement=&flyingMovement, .speed=FLYSPEED, .deceleration = DECELSPEED };
}



int flyingMovement(struct Enemy *e, void *whatever) {
  int wd = MYSDL_Sprite_getRenderWd(&e->gfx);
  int ht = MYSDL_Sprite_getRenderHt(&e->gfx);
  switch(e->state) {
    //pick a direction
  case 0: {
    int deg = rand() % 360;
    e->xVel = cos(DEG_TO_RAD(deg));
    e->yVel = sin(DEG_TO_RAD(deg));
    e->state++;
  } break;

    //general movement
  case 1:
    e->x += (e->xVel * e->speed) * DeltaTime;
    e->y += (e->yVel * e->speed) * DeltaTime;
    if (e->x - wd <= 0)
      e->state = 3;
    else if  (e->x + (wd << 1) > INTERNAL_RES_X)
      e->state = 4;
    else if (e->y - ht < 0)
      e->state = 5;
    else if (e->y + (ht << 1) > INTERNAL_RES_Y)
      e->state = 6;
    break;
    //turn around x
  case 3:
    e->xVel += e->speed * e->deceleration * DeltaTime;
    e->state = 1;
    break;
  case 4:
    e->xVel -= e->speed * e->deceleration * DeltaTime;
    e->state = 1;
    break;
    //turn around y
  case 5:
    e->yVel += e->speed * e->deceleration * DeltaTime;
    e->state = 1;
    break;
  case 6:
    e->yVel -= e->speed *e ->deceleration * DeltaTime;
    e->state = 1;
    break;    
  }
  return 0;
}



int loadPlayer(Player players[MAX_PLAYERS]) {
  for (int i = 0; i < MAX_PLAYERS; i++) {
    memset(&players[i], 0, sizeof(Player));
    players[i].id = i;
    players[i].x = 0;
    players[i].y = 0;
    players[i].speed = BASE_PLAYER_SPEED;
    players[i].lastShot = SHOT_TIMER;
    MYSDL_Sprite_load(&players[i].gfx, MYSDL_getMainRenderer(), PLAYER_SPRITE_FILE);
    MYSDL_Sprite_setSourceFrame(&players[i].gfx, 0, 0, 16, 16);

    for (int x = 0; x < MAX_BULLETS; x++) {
      Bullet *b = &players[i].bullets[x];
      MYSDL_Sprite_load(&b->gfx, MYSDL_getMainRenderer(), OBJECTS_SPRITE_FILE);
      MYSDL_Sprite_setSourceFrame(&b->gfx, 0, 48, 16, 16);
      MYSDL_Sprite_scaleTo(&b->gfx, BULLET_WIDTH, BULLET_HEIGHT);
    }
  }

  MYSDL_Sprite_setSourceFrame(&players[0].gfx, 0, 16, 16, 16);
  MYSDL_Sprite_setSourceFrame(&players[1].gfx, 0, 64, 16, 16);

  for (int i = 0; i < MAX_PLAYERS; i++)
    MYSDL_Sprite_scaleTo(&players[i].gfx, PLAYER_WIDTH, PLAYER_HEIGHT);
  
  return 0;
}


int loadLevel(Level *lvl) {
  //load the backdrop
  MYSDL_Sprite_load(&lvl->bg, MYSDL_getMainRenderer(), lvl->backdrop_p);
  MYSDL_Sprite_setSourceFrame(&lvl->bg, 0, 0, 256, 192);
  MYSDL_Sprite_scaleTo(&lvl->bg, TARGET_RES_X, TARGET_RES_Y);

  //load the tileset
  MYSDL_Sprite_load(&lvl->tiles, MYSDL_getMainRenderer(), lvl->tiles_p);
  MYSDL_Sprite_scaleTo(&lvl->tiles, TILE_WIDTH, TILE_HEIGHT);
  //MYSDL_Sprite_setSourceFrame(&lvl->tiles, 0, 0, TILE_WIDTH, TILE_HEIGHT);

  //load up the enemies
  for (int i = 0; i < ENEMIES_PER_LEVEL; i++) {
    Enemy *e = &lvl->enemy_template[i];
    
    MYSDL_Sprite_load(&e->gfx, MYSDL_getMainRenderer(), lvl->enemies_p);
    int type = lvl->enemy_types[i];
    MYSDL_Sprite_setSourceFrame(&e->gfx, 0, 16*type, 16, 16);
    MYSDL_Sprite_scaleTo(&e->gfx, ENEMY_WIDTH, ENEMY_HEIGHT);
  }

  //load up the boss
  //MYSDL_Sprite_load(&lvl->boss, MYSDL_getMainRenderer(), lvl->boss_p);
  //MYSDL_Sprite_setSourceFrame(&lvl->boss, 0, 0, BOSS_WIDTH, BOSS_HEIGHT);

  //load up the music for the level
  lvl->music = Mix_LoadWAV(lvl->music_p);
  lvl->speed = BASE_SCROLL_SPEED;
  memset(&lvl->rows, 0, sizeof(lvl->rows));
  return 0;
}

void generateTileRow(TileRow *row, int minGapSize) {
  row->alive = 1;
  row->ypos = INTERNAL_RES_Y + TILE_HEIGHT;
  for (int i = 0; i < TILES_PER_ROW; i++)
    row->tiles[i] = TILE_TYPE1;

  int numGaps = (rand() % MAX_GAP_COUNT) + 1;
  for (int z = 0; z < numGaps; z++) {
    //add the gap in place
    int gapPos = (rand() % (TILES_PER_ROW - minGapSize));
    for (int i = gapPos; i < gapPos + minGapSize; i++)
      row->tiles[i]= TILE_NONE;
  }
}


void tileManager(Level *lvl) {
  float gap = CALC_ROW_GAP(lvl->difficulty);
  TileRow *lastRow = NULL, *freeRow = NULL;

  int shiftUp = 0;
  for (int i = 0; i < MAX_TILE_ROWS, freeRow == NULL; i++) {
    TileRow *curRow = &lvl->rows[i];
    if (curRow->alive) {
      if (curRow->ypos <= -TILE_HEIGHT) {
        curRow->alive = 0;
        shiftUp = 1;
      }
      
      curRow->ypos -= lvl->speed;
      if (!lastRow || curRow->ypos > lastRow->ypos)
        lastRow = curRow;

      if (shiftUp && i > 0) {
        memcpy(&lvl->rows[i - 1], curRow, sizeof(TileRow));
        curRow->alive = 0;
      }
    } else
      freeRow = curRow;
    
  }

  if (!lastRow || (lastRow->ypos < INTERNAL_RES_Y - gap && freeRow))
    generateTileRow(freeRow, MIN_GAP_SIZE);
}
 
void enemyProcess(Level *lvl) {
  int doSpawn = rand() % 100000;
  for (int i = 0; i < MAX_ENEMIES; i++) {
    Enemy *e = &lvl->enemies[i];
    if (e->health > 0) {
      //process an already active enemy
      if (e->movement) e->movement(e, NULL);
      MYSDL_Sprite_draw(MYSDL_getMainRenderer(), &e->gfx, e->x, e->y);
    } else {
      if (doSpawn * DeltaTime < 5000) {
        continue;
      }
      doSpawn = 0;
      printf("SPAWNING\n");
      //spawn an enemy
      int type = rand() % ENEMIES_PER_LEVEL;
      memcpy(e, &lvl->enemy_template[type], sizeof(Enemy));
      e->x = rand() % INTERNAL_RES_X;
      e->y = rand() % INTERNAL_RES_Y;
    }
  }
}



 
void drawRows(Level *lvl) {
  for (int i = 0; i < MAX_TILE_ROWS; i++) {
    TileRow *curRow = &lvl->rows[i];
    if (curRow->alive) {
      for (int x = 0; x < TILES_PER_ROW; x++) {
        if (curRow->tiles[x] != TILE_NONE) 
          MYSDL_Sprite_draw(MYSDL_getMainRenderer(), &lvl->tiles, TILE_WIDTH * x, curRow->ypos);
      }
    }
  }
}


void playerShoot(Player *player) {
  for (int i = 0; i < MAX_BULLETS; i++) {
    Bullet *b = &player->bullets[i];
    if (b->alive) continue;

    b->x = player->x;
    b->y = player->y;
    if (player->direction == DIRECTION_LEFT)
      b->xVel = -BULLET_SPEED;
    else
      b->xVel = BULLET_SPEED;

    b->yVel = 0;
    b->alive = 1;
    return;
  }
}

int bulletCollision(Bullet *b, Level *lvl) {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    Enemy *e = &lvl->enemies[i];
    if (e->health <= 0) continue;
    if (box_collision(&b->gfx, &e->gfx)) {
      b->alive = 0;
      e->health = 0;
      break;
    }
  }
  return 0;
}

int playerHandler(Player *player, Level *lvl) {
  int right = GlobalKeyState[SDL_SCANCODE_RIGHT];
  int left = GlobalKeyState[SDL_SCANCODE_LEFT];

  player->x += (right - left) * (player->speed * DeltaTime);
  if (right || left)
    player->direction = (right > 0) ? DIRECTION_RIGHT : DIRECTION_LEFT;
  
  player->y += player->yVel;
  if (player->yVel < MAX_FALL_VELOCITY)
    player->yVel += GRAVITY * DeltaTime;

  if (GlobalKeyState[SDL_SCANCODE_Z]) {
    player->lastShot -= DeltaTime;
    if (!player->shooting || player->lastShot <= 0) {
      playerShoot(player);
      player->lastShot = SHOT_TIMER;
      player->shooting = 1;
    }
  } else {
    player->shooting = 0;
  }
  

  //check for platform collisions
  int playerCenter = player->x + (MYSDL_Sprite_getRenderWd(&player->gfx) >> 1);
  for (int i = 0; i < MAX_TILE_ROWS; i++) {
    TileRow *curRow = &lvl->rows[i];
    if (player->y < curRow->ypos) {
      
      float diff = (player->y + PLAYER_HEIGHT) - curRow->ypos;
      if (diff < 0 || diff > (PLAYER_HEIGHT >> 1)) break;

      int xpos = floor(playerCenter) / TILE_WIDTH;
      if (curRow->tiles[xpos] != TILE_NONE) {
        if (player->yVel > 0) {
          player->yVel = 0;
          player->y -= diff;
        }
      }
      break;
    }
  }
  
  if (player->y + PLAYER_HEIGHT > INTERNAL_RES_Y) {
    float offset = (player->y + PLAYER_HEIGHT) - INTERNAL_RES_Y;
    player->y -= offset;
    player->yVel = 0;
  }

  player->inAir = player->yVel != 0.0;
  if (!player->inAir && GlobalKeyState[SDL_SCANCODE_SPACE]) {
    player->yVel = -JUMP_POWER;
  }

  for (int i = 0; i < MAX_BULLETS; i++) {
    Bullet *b = &player->bullets[i];
    if (!b->alive) continue;
    b->x += b->xVel;
    b->y += b->yVel;

    int width = MYSDL_Sprite_getRenderWd(&b->gfx);
    int height = MYSDL_Sprite_getRenderHt(&b->gfx);
    if (b->x + width  < 0 || b->x >= INTERNAL_RES_X ||
        b->y + height < 0 || b->y > INTERNAL_RES_Y) {
      b->alive = 0;
    }
    bulletCollision(b, lvl);
    MYSDL_Sprite_draw(MYSDL_getMainRenderer(), &b->gfx, b->x, b->y);
  }

  MYSDL_Sprite_draw(MYSDL_getMainRenderer(), &player->gfx, player->x, player->y);
  return 0;
}



int renderLevel(Level *lvl) {

  tileManager(lvl);
  //render background
  MYSDL_Sprite_draw(MYSDL_getMainRenderer(), &lvl->bg, 0, 0);
  drawRows(lvl);
  //render the player
  for (int p = 0; p < NumPlayers; p++)
    playerHandler(&GlobalPlayers[p], lvl);

  enemyProcess(lvl);
  return 0;
}






/*

void update_sprites(void){
    int wd, ht;
    MYSDL_getRendererSize(MYSDL_getMainRenderer(), &wd, &ht);

    for(int i = 0; i < sprite_count; i++){
        SpriteObj *obj = &_spriteStack[i];
        int scaledX = MYSDL_Sprite_getRenderWd(&obj->sprite) + 1;
        int scaledY = MYSDL_Sprite_getRenderHt(&obj->sprite) + 1;

        obj->sprite.position.x += obj->xVel;
        obj->sprite.position.y += obj->yVel;

        if (obj->sprite.position.x + MYSDL_Sprite_getRenderWd(&obj->sprite) >= wd || obj->sprite.position.x <= 0){
            obj->xVel = -obj->xVel;
        }
        if (obj->sprite.position.y + MYSDL_Sprite_getRenderHt(&obj->sprite) >= ht || obj->sprite.position.y <= 0){
            obj->yVel = -obj->yVel;
        }

        scaledX = (scaledX > 512) ? 128 : scaledX;
        scaledY = (scaledY > 512) ? 128 : scaledY;

        //MYSDL_Sprite_rotateTo(&obj->sprite, MYSDL_Sprite_getRotAngle(&obj->sprite) + 1.0);
        //MYSDL_Sprite_scaleTo(&obj->sprite, scaledX, scaledY);
        MYSDL_Sprite_draw(MYSDL_getMainRenderer(), &obj->sprite, obj->sprite.position.x, obj->sprite.position.y);

    }
}

void spawn_sprite(int spriteNum){
    MYSDL_Sprite_clone(&_spriteStack[spriteNum].sprite, &template);
    MYSDL_Sprite_setSourceFrame(&_spriteStack[spriteNum].sprite, 0, 0, 32, 32);
    _spriteStack[spriteNum].xVel = (rand()%2) + 1;
    _spriteStack[spriteNum].yVel = (rand()%2) + 1;

    _spriteStack[spriteNum].sprite.position.x = rand() % (SCREEN_WIDTH - 32);
    _spriteStack[spriteNum].sprite.position.y = rand() % (TARGET_RES_Y - 32);


    MYSDL_Sprite_scaleTo(&_spriteStack[spriteNum].sprite, 256, 256);
    MYSDL_Animate *anim = &_spriteStack[spriteNum].sprite.curAnimation;

    MYSDL_Animate_setCallback(anim, &spriteDone, NULL);
    MYSDL_Animate_setTimeLength(anim, 350);
    MYSDL_Animate_setFrameRange(anim, 0, 2);
    MYSDL_Animate_setType(anim, ANIMATE_FOREVER);
    MYSDL_Animate_reset(anim);
    MYSDL_Animate_setStatus(anim, ANIMATE_START);


    printf("sprites spawned: %d\n", spriteNum);
}
*/
int main(int argc, char *argv[]) {
    if(MYSDL_init(TARGET_RES_X, TARGET_RES_Y, 1)) {
        printf("Error init lib\n");
        return 0;
    }
    
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 );
    initData();
    
    loadPlayer(GlobalPlayers);
    loadLevel(&GlobalLevels[0]);
    Mix_PlayChannel( -1, GlobalLevels[0].music, 0 );

    bool quit = false;

    while (!quit) {
        //Handle events on queue
        while( SDL_PollEvent(&GlobalEvents) != 0 ){
            //User requests quit
            if( GlobalEvents.type == SDL_QUIT ) {
                quit = true;
            }
            //User presses a key
            else if( GlobalEvents.type == SDL_KEYDOWN ) {
                //Select surfaces based on key press
                switch( GlobalEvents.key.keysym.sym ) {
                    case SDLK_ESCAPE:
                        quit++;
                    break;
                }
            }
        }
        Global_Update();
        MYSDL_clearRenderer(MYSDL_getMainRenderer());
        renderLevel(&GlobalLevels[0]);

        //Get window surface
        MYSDL_renderPresent(MYSDL_getMainRenderer());
    }

    //program cleanup on exit
    MYSDL_Quit();
    return 0;
}

