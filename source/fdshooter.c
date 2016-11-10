#include <stdio.h>
#include <math.h>
#include "globals.h"

#define FLYSPEED 8.0f
#define DECELSPEED 0.1f

SDL_Event GlobalEvents;

int flyingTrajectory(struct Enemy *e, void *level);

void initData(void) {

  for (int i = 0; i < ENEMIES_PER_LEVEL; i++)
  GlobalLevels[0].enemy_template[i] = (Enemy) { 
	.state = ENEMY_STATE_ACTIVE,
	.health = 1,
	.updateTradjectory = &flyingTrajectory,
	.speed = FLYSPEED,
	.properties = ENEMY_PROP_KEEPONSCREEN,
	.angleDir = 1.0
  };
}



int flyingTrajectory(struct Enemy *e, void *level) {

	e->angle += e->angleDir;
	if (e->properties & ENEMY_PROP_HIT_LEFTSCREEN || e->properties & ENEMY_PROP_HIT_RIGHTSCREEN ||
		e->properties & ENEMY_PROP_HIT_TOPSCREEN || e->properties & ENEMY_PROP_HIT_BOTSCREEN)
	{
		e->angleDir = -e->angleDir;
		e->angle += 180;
	}


    e->xVel = cos(DEG_TO_RAD(e->angle));
    e->yVel = sin(DEG_TO_RAD(e->angle));

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

