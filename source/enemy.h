#ifndef __ENEMEY_H__
#define __ENEMEY_H__

#include "globals.h"

typedef struct Enemy {
	int id, health, damage, state;
	float x, y;
	float xVel, yVel;
	float speed;
	float deceleration;
	MYSDL_Sprite gfx;
	int(*movement) (struct Enemy *, void *);
	int(*attack) (struct Enemy *, void *);
} Enemy;


#endif //__ENEMEY_H__