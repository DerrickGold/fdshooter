#include <stdio.h>
#include "globals.h"
#include "level.h"

static float leftScreenDetect(Enemy *e) {
	if (e->x < 0) return e->x;
	return 0;
}

static float rightScreenDetect(Enemy *e) {
	return (e->x + MYSDL_Sprite_getRenderWd(&e->gfx)) - INTERNAL_RES_X;
}

static float topScreenDetect(Enemy *e) {
	int pos = e->y + MYSDL_Sprite_getRenderHt(&e->gfx);
	if (pos < 0) return pos;
	return 0;
}

static float bottomScreenDetect(Enemy *e) {
	int ht = MYSDL_Sprite_getRenderHt(&e->gfx);
	return (e->y + ht) - (INTERNAL_RES_Y + ht);
}

static PlatCollision detectGround(Level *lvl, Enemy *e, float xTileOff, float yTileOff) {
	return Level_GetPlatformTile(lvl, 
		e->x + (xTileOff * TILE_WIDTH), e->y + (yTileOff * TILE_HEIGHT) , 
		MYSDL_Sprite_getRenderWd(&e->gfx), MYSDL_Sprite_getRenderHt(&e->gfx));
}

static void applyGravity(Level *lvl, Enemy *e) {
	e->y += e->yVel * DeltaTime;
	//if (e->yVel < MAX_FALL_VELOCITY)
	e->yVel += GRAVITY * DeltaTime;

	PlatCollision collision = detectGround(lvl, e, 0, 0);
	if (collision.tile > TILE_NONE) {
		if (e->yVel > 0) {
			e->yVel = 0;
			e->y -= collision.yOffset;
		}
		e->properties |= ENEMY_PROP_GROUNDED;
	}


	if (e->y > INTERNAL_RES_Y) {
		float offset = e->y  - INTERNAL_RES_Y;
		e->y -= offset;
		e->yVel = 0;
	}
}

static void clearProps(Enemy *e) {
	/*
		The first 2 bits (KEEPONSCREEN and USEGRAVITY) are persistent properties, 
		the rest are refreshed every frame
	*/
	ENEMY_PROPS oldProperties = e->properties & (ENEMY_PROP_KEEPONSCREEN | ENEMY_PROP_USEGRAVITY);
	e->properties = oldProperties;
}

static void enemyHit(Enemy *e, float damage) {
	printf("Enemy invulnerable!\n");
	e->health -= damage;
	if (e->health <= 0) 
		e->state = ENEMY_STATE_DEATH;
}

static void resetEnemyHit(Enemy *e) {
	printf("Enemy vulnerable!\n");
}

static void updateHitTimer(Enemy *e) {
	if (e->hitTimer > 0) {
		e->hitTimer -= DeltaTime;
		if (e->hitTimer <= 0) {
			e->hitTimer = 0;
			resetEnemyHit(e);
		}
	}
}

int Enemy_AttackFor(Enemy *e, float damage) {
	if (e->hitTimer > 0) return 0;

	e->hitTimer = ENEMY_HIT_TIMEOUT;
	enemyHit(e, damage);
	return 0;
}

int Enemy_Spawn(int type, Enemy templates[ENEMIES_PER_LEVEL], Enemy levelSlots[MAX_ENEMIES]) {
	if (type >= ENEMIES_PER_LEVEL) return -1;

	for (int i = 0; i < MAX_ENEMIES; i++) {
		//find an open slot
		Enemy *e = &levelSlots[i];
		if (e->state == ENEMY_STATE_NULL) {
			memcpy(e, &templates[type], sizeof(Enemy));
			e->healthbar = HealthBar_Create(e->health, HEALTHBAR_WIDTH, HEALTHBAR_HEIGHT);
			e->state = ENEMY_STATE_SPAWN;
			printf("doin the spawn\n");
			return 0;
		}
	}
	printf("fucked the spawn\n");
	return -1;
}

void Enemy_Draw(Enemy *e) {
	if (e->state == ENEMY_STATE_NULL) return;

	int eWidth = MYSDL_Sprite_getRenderWd(&e->gfx);
	float xCenter = (e->x + (eWidth / 2)) - (e->healthbar.width / 2);
 
	HealthBar_Draw(&e->healthbar, e->health, xCenter, e->y - HEALTHBAR_OFFSET - e->healthbar.height);
	MYSDL_Sprite_draw(MYSDL_getMainRenderer(), &e->gfx, e->x, e->y);
}



int Enemy_StateMachine(Enemy *e, void *level) {
	Level *lvl = (Level *)level;
	switch (e->state) {
		//pick a direction
	case ENEMY_STATE_NULL:
		break;
	case ENEMY_STATE_SPAWN:
		printf("Spawned new enemy\n");
		if (e->spawn) e->spawn(e, level);
		e->state = ENEMY_STATE_ACTIVE;
		break;
	case ENEMY_STATE_ACTIVE:
		updateHitTimer(e);

		//update x and y velocities
		if (e->updateTradjectory) e->updateTradjectory(e, level);
		//clear props once the tradjectory function has finished with them
		clearProps(e);

		float speed = e->speed - e->deceleration;
		if (speed < 0) {
			speed = 0;
			//cap the deceleration to the max speed of the player
			e->deceleration = -e->speed;
		}

		//update movement based on updated tradjectory
		e->x += (e->xVel * e->speed) * DeltaTime;
		if (e->properties & ENEMY_PROP_USEGRAVITY) applyGravity(lvl, e);
		else e->y += (e->yVel * e->speed) * DeltaTime;

		if (e->properties & ENEMY_PROP_KEEPONSCREEN) {
			//check left and right boundaries
			float xDiff = leftScreenDetect(e);
			if (xDiff) {
				e->x -= xDiff;
				e->properties |= ENEMY_PROP_HIT_LEFTSCREEN;
			}
			else {
				xDiff = rightScreenDetect(e);
				if (xDiff > 0) {
					e->x -= xDiff;
					e->properties |= ENEMY_PROP_HIT_RIGHTSCREEN;
				}
			}
			//check top and bottom boundaries
			float yDiff = topScreenDetect(e);
			if (yDiff) {
				e->y -= yDiff;
				e->properties |= ENEMY_PROP_HIT_TOPSCREEN;
			}
			else {
				yDiff = bottomScreenDetect(e);
				if (yDiff > 0) {
					e->y -= yDiff;
					e->properties |= ENEMY_PROP_HIT_BOTSCREEN;
				}
			}
		}
		break;
	case ENEMY_STATE_DEATH:
		printf("ENEMY DIED\n");
		e->state = ENEMY_STATE_DESPAWN;
		break;
	case ENEMY_STATE_DESPAWN:
		printf("DESPAWN ENEMY\n");
		e->state = ENEMY_STATE_NULL;
		break;
	}
	return 0;
}
