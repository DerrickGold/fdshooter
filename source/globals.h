#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#if defined (__WIN32__)
#define SDL_MAIN_HANDLED
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "mysdl_main.h"
#include "mysdl_sprites.h"
#include "mysdl_animate.h"
#include "mysdl_tilebg.h"

#define TICKS_PER_SECOND 1000.0f

#define RESOURCE_DIR "./GameResources"
#define PLAYER_SPRITE_FILE RESOURCE_DIR"/player.png"
#define OBJECTS_SPRITE_FILE RESOURCE_DIR"/items.png"
#define HEALTHBAR_GFX RESOURCE_DIR"/healthbar.png"


#define PI 3.14159265

#define LVL_COUNT 2

//target resolution to scale to
#define TARGET_RES_X 1280
#define TARGET_RES_Y 720

//internal full screen resolution
#define INTERNAL_RES_X 1280
#define INTERNAL_RES_Y 720

#define DIRECTION_LEFT 0
#define DIRECTION_RIGHT 1

//Default sizes for various things
#define TILE_WIDTH 64
#define TILE_HEIGHT (TILE_WIDTH>>1)


#define TILES_PER_ROW (INTERNAL_RES_X / TILE_WIDTH)
#define MAX_TILE_ROWS (INTERNAL_RES_Y / TILE_HEIGHT)

#define ENEMY_WIDTH 32
#define ENEMY_HEIGHT 32

#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 32

#define BULLET_WIDTH 16
#define BULLET_HEIGHT 16

#define HEALTHBAR_WIDTH 32
#define HEALTHBAR_HEIGHT 4
#define HEALTHBAR_OFFSET 4


//bosses are at least 2 times bigger than enemies
#define BOSS_WIDTH (ENEMY_WIDTH<<1)
#define BOSS_HEIGHT (ENEMY_HEIGHT<<1)


#define ENEMIES_PER_LEVEL 5
#define BASE_DAMAGE 1

#define MAX_BULLETS 10
//bullet should take 2 seconds to cross the screen
#define BULLET_SPEED (INTERNAL_RES_X / 2)

#define MAX_ENEMIES 30

#define MIN_ROW_GAP (TILE_HEIGHT<<1)
#define MAX_GAP_COUNT 3
#define MIN_GAP_SIZE 3
#define MAX_DIFFICULTY 1.0f
#define BASE_SCROLL_SPEED 50.0f
//should take player 4 seconds to go from one end of the screen to the other
#define BASE_PLAYER_SPEED (INTERNAL_RES_X / 3)

#define GRAVITY 1800.f
#define JUMP_POWER 700.f
//from the top of the screen, it should take X seconds to freefall at 
//maximum velocity
#define MAX_FALL_VELOCITY (INTERNAL_RES_Y/2.5f)

#define MAX_PLAYERS 2

//limit to 2 shots per second
#define SHOT_TIMER 2.5f
#define BASE_SPAWN_TIMER 5.0f

//when a player is hit, they are invulnerable for
//HIT_TIMEOUT seconds to prevent repeating hits
#define HIT_TIMEOUT 0.5f
#define ENEMY_HIT_TIMEOUT 0.1f
#define PLAYER_BASE_HEALTH 2.0f
#define PLAYER_BASE_ATTACKPWR 0.5f

#define CALC_ROW_GAP(diff) (((MAX_DIFFICULTY - diff) + 1.0f) * MIN_ROW_GAP)
#define DEG_TO_RAD(deg) (deg * (PI / 180))




#include "level.h"
#include "enemy.h"
#include "bullet.h"
#include "player.h"
#include "healthbar.h"



extern double DeltaTime;
extern Uint8 *GlobalKeyState;
extern Player GlobalPlayers[MAX_PLAYERS];
extern Level GlobalLevels[LVL_COUNT];
extern int NumPlayers;

int box_collision(MYSDL_Sprite *s1, MYSDL_Sprite *s2);
void Global_Update(void);
#endif //__GLOBALS_H__
