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
		players[i].health = PLAYER_BASE_HEALTH;
		players[i].attackPwr = PLAYER_BASE_ATTACKPWR;
		players[i].healthbar = HealthBar_Create(players[i].health , PLAYER_HEALTHBAR_WIDTH, PLAYER_HEALTHBAR_HEIGHT);
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
		b->attackPwr = player->attackPwr;
		return;
	}
}

static int bulletCollision(Bullet *b, Level *lvl) {
	for (int i = 0; i < MAX_ENEMIES; i++) {
		Enemy *e = &lvl->enemies[i];
		if (e->state != ENEMY_STATE_ACTIVE) continue;
		if (box_collision(&b->gfx, &e->gfx)) {
			b->alive = 0;
			//e->state = ENEMY_STATE_DEATH;
			Enemy_AttackFor(e, b->attackPwr);
			break;
		}
	}
	return 0;
}

static void playerDeath(Player *player) {
	printf("Player is now dead!\n");
	player->health = PLAYER_BASE_HEALTH;
}


static void playerHit(Player *player, float damage) {
	printf("Player invulnerable!\n");
	player->health -= damage;
	if (player->health <= 0) 
		playerDeath(player);
}

static void resetPlayerHit(Player *player) {
	printf("player now vulnerable\n");
}

static void playerUpdateHitTimer(Player *player) {
	if (player->hitTimer > 0) {
		player->hitTimer -= DeltaTime;
		if (player->hitTimer <= 0) {
			resetPlayerHit(player);
			player->hitTimer = 0;
		}
	}
}

static void playerEnemyCollision(Player *player, Level *lvl) {
	if (player->hitTimer > 0) return;

	for (int i = 0; i < MAX_ENEMIES; i++) {
		Enemy *e = &lvl->enemies[i];
		if (e->state != ENEMY_STATE_ACTIVE) continue;
		if (box_collision(&player->gfx, &e->gfx)) {
			Player_AttackFor(player, e->damage);
			break;
		}
	}
}

int Player_AttackFor(Player *player, float damage) {
	if (player->hitTimer > 0) return 0;

	player->hitTimer = HIT_TIMEOUT;
	playerHit(player, damage);
	return 0;
}


int Player_Handler(Player *player, Level *lvl) {
	int right = (GlobalKeyState[SDL_SCANCODE_RIGHT] > 0);
	int left = (GlobalKeyState[SDL_SCANCODE_LEFT] > 0);


	player->x += ((right - left) * player->speed * DeltaTime);
	if (right || left)
		player->direction = (right > 0) ? DIRECTION_RIGHT : DIRECTION_LEFT;


	player->y += player->yVel * DeltaTime;
	//if (player->yVel < MAX_FALL_VELOCITY)

		player->yVel += (GRAVITY * DeltaTime);


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


	PlatCollision collision = Level_GetPlatformTile(lvl, player->x, player->y,
		MYSDL_Sprite_getRenderWd(&player->gfx), MYSDL_Sprite_getRenderHt(&player->gfx));

	if (collision.tile > TILE_NONE) {
		if (player->yVel > 0) {
			player->yVel = 0;
			player->y -= collision.yOffset;
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
		b->x += b->xVel * DeltaTime;
		b->y += b->yVel * DeltaTime;

		int width = MYSDL_Sprite_getRenderWd(&b->gfx);
		int height = MYSDL_Sprite_getRenderHt(&b->gfx);
		if (b->x + width  < 0 || b->x >= INTERNAL_RES_X ||
			b->y + height < 0 || b->y > INTERNAL_RES_Y) {
			b->alive = 0;
		}
		bulletCollision(b, lvl);
		MYSDL_Sprite_draw(MYSDL_getMainRenderer(), &b->gfx, b->x, b->y);
	}

	playerUpdateHitTimer(player);
	playerEnemyCollision(player, lvl);

	float hbx = (player->id == 0) ? PLAYER_HEALTHBAR_X1 : PLAYER_HEALTHBAR_X2;
	
	HealthBar_Draw(&player->healthbar, player->health, hbx, PLAYER_HEALTHBAR_Y);
	MYSDL_Sprite_draw(MYSDL_getMainRenderer(), &player->gfx, player->x, player->y);
	return 0;
}