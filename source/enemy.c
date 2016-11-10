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
	if (e->y < 0) return e->y;
	return 0;
}

static float bottomScreenDetect(Enemy *e) {
	return (e->y + MYSDL_Sprite_getRenderHt(&e->gfx)) - INTERNAL_RES_Y;
}

static PlatCollision detectGround(Level *lvl, Enemy *e, float xTileOff, float yTileOff) {
	return Level_GetPlatformTile(lvl, 
		e->x + (xTileOff * TILE_WIDTH), e->y + (yTileOff * TILE_HEIGHT) , 
		MYSDL_Sprite_getRenderWd(&e->gfx), MYSDL_Sprite_getRenderHt(&e->gfx));
}

static void applyGravity(Level *lvl, Enemy *e) {
	e->y += e->yVel;
	if (e->yVel < MAX_FALL_VELOCITY)
		e->yVel += GRAVITY * DeltaTime;

	PlatCollision collision = detectGround(lvl, e, 0, 0);
	if (collision.tile > TILE_NONE) {
		if (e->yVel > 0) {
			e->yVel = 0;
			e->y -= collision.yOffset;
		}
		e->properties |= ENEMY_PROP_GROUNDED;
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


int Enemy_StateMachine(Enemy *e, void *level) {
	Level *lvl = (Level *)level;
	switch (e->state) {
		//pick a direction
	case ENEMY_STATE_NULL:
		break;
	case ENEMY_STATE_SPAWN:
		printf("Spawned new enemy\n");
		e->state = ENEMY_STATE_ACTIVE;
		break;
	case ENEMY_STATE_ACTIVE:

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
		e->y += (e->yVel * e->speed) * DeltaTime;

		if (e->properties & ENEMY_PROP_USEGRAVITY) applyGravity(lvl, e);

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
