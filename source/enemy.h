#ifndef __ENEMEY_H__
#define __ENEMEY_H__

#include "globals.h"

typedef enum {
	ENEMY_PROP_KEEPONSCREEN = (1<<0),
	ENEMY_PROP_USEGRAVITY = (1<<1),
	ENEMY_PROP_HIT_LEFTSCREEN = (1<<2),
	ENEMY_PROP_HIT_RIGHTSCREEN = (1<<3),
	ENEMY_PROP_HIT_TOPSCREEN = (1<<4),
	ENEMY_PROP_HIT_BOTSCREEN = (1<<5),
	ENEMY_PROP_GROUNDED = (1<<6)
} ENEMY_PROPS;

typedef enum {
	ENEMY_STATE_NULL,
	ENEMY_STATE_SPAWN,
	ENEMY_STATE_ACTIVE,
	ENEMY_STATE_DEATH,
	ENEMY_STATE_DESPAWN
} ENEMY_STATE;

typedef struct Enemy {
	int id, health, damage;
	float x, y;
	float xVel, yVel;
	float speed;
	float deceleration;
	float angle;
	float angleDir;
	MYSDL_Sprite gfx;
	ENEMY_PROPS properties;
	ENEMY_STATE state;
	int(*updateTradjectory) (struct Enemy *, void *);
	int(*attack) (struct Enemy *, void *);
} Enemy;

int Enemy_StateMachine(Enemy *e, void *level);
#endif //__ENEMEY_H__