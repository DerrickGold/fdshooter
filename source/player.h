#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "globals.h"
#include "bullet.h"

typedef struct Player {
	int id;
	int health, bonus;
	float speed;
	float yVel;
	float x, y;
	char inAir;
	char direction;
	char shooting;
	MYSDL_Sprite gfx;
	double lastShot;
	Bullet bullets[MAX_BULLETS];
} Player;

int Player_Load(Player players[MAX_PLAYERS]);
int Player_Handler(Player *player, Level *lvl);

#endif //__PLAYER_H__