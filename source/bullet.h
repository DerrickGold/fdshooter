#ifndef __BULLET_H__
#define __BULLET_H__

typedef struct Bullet {
	char alive;
	MYSDL_Sprite gfx;
	float x, y;
	float xVel, yVel;
	float attackPwr;
} Bullet;

#endif //__BULLET_H__