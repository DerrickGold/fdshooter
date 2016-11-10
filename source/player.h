#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "globals.h"
#include "bullet.h"

typedef struct Player {
	int id;
	float health;
	float speed, yVel;
	float x, y;
	float attackPwr;
	char inAir;
	char direction;
	char shooting;
	double hitTimer;

	MYSDL_Sprite gfx;
	double lastShot;
	Bullet bullets[MAX_BULLETS];
} Player;

int Player_Load(Player players[MAX_PLAYERS]);
int Player_AttackFor(Player *player, float damage);
int Player_Handler(Player *player, Level *lvl);

#endif //__PLAYER_H__