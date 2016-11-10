#include "globals.h"
#include "bullet.h"
#include "player.h"
#include "enemy.h"

int Player_Load(Player players[MAX_PLAYERS]) {
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


static void shoot(Player *player) {
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

static int bulletCollision(Bullet *b, Level *lvl) {
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


int Player_Handler(Player *player, Level *lvl) {
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
			shoot(player);
			player->lastShot = SHOT_TIMER;
			player->shooting = 1;
		}
	}
	else {
		player->shooting = 0;
	}


	//check for platform collisions
	int playerCenter = player->x + (MYSDL_Sprite_getRenderWd(&player->gfx) >> 1);
	for (int i = 0; i < MAX_TILE_ROWS; i++) {
		TileRow *curRow = &lvl->rows[i];
		if (player->y < curRow->ypos) {

			float diff = (player->y + PLAYER_HEIGHT) - curRow->ypos;
			if (diff < 0 || diff >(PLAYER_HEIGHT >> 1)) break;

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