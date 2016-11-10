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
	int id;
	float health, damage;
	float x, y;
	float xVel, yVel;
	float speed, deceleration;
	float angle, angleDir;
	float timer, hitTimer;
	MYSDL_Sprite gfx;
	ENEMY_PROPS properties;
	ENEMY_STATE state;
	int(*spawn) (struct Enemy *, void *);
	int(*updateTradjectory) (struct Enemy *, void *);
	int(*attack) (struct Enemy *, void *);
} Enemy;

int Enemy_AttackFor(Enemy *e, float damage);
int Enemy_Spawn(int type, Enemy templates[ENEMIES_PER_LEVEL], Enemy levelSlots[MAX_ENEMIES]);
int Enemy_StateMachine(Enemy *e, void *level);
#endif //__ENEMEY_H__