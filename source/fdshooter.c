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



int StartLevel(Level *lvl) {
	Level_Load(lvl);
	Mix_PlayChannel(-1, lvl->music, 0);
	return 0;
}


int RunLevel(Level *lvl) {
  Level_UpdatePlatforms(lvl);
  //render background
  MYSDL_Sprite_draw(MYSDL_getMainRenderer(), &lvl->bg, 0, 0);
  Level_DrawPlatforms(lvl);
  //render the player
  for (int p = 0; p < NumPlayers; p++)
	  Player_Handler(&GlobalPlayers[p], lvl);

  Level_UpdateEnemies(lvl);
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
    
	Player_Load(GlobalPlayers);
	StartLevel(&GlobalLevels[0]);

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
		RunLevel(&GlobalLevels[0]);
        MYSDL_renderPresent(MYSDL_getMainRenderer());
    }

    //program cleanup on exit
    MYSDL_Quit();
    return 0;
}

