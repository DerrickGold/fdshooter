#include <stdlib.h>
#include "globals.h"
#include "enemy.h"

static void generateTileRow(TileRow *row, int minGapSize) {
	row->alive = 1;
	row->ypos = INTERNAL_RES_Y + TILE_HEIGHT;
	for (int i = 0; i < TILES_PER_ROW; i++)
		row->tiles[i] = TILE_TYPE1;

	int numGaps = (rand() % MAX_GAP_COUNT) + 1;
	for (int z = 0; z < numGaps; z++) {
		//add the gap in place
		int gapPos = (rand() % (TILES_PER_ROW - minGapSize));
		for (int i = gapPos; i < gapPos + minGapSize; i++)
			row->tiles[i] = TILE_NONE;
	}
}


int Level_Load(Level *lvl) {
	//load the backdrop
	MYSDL_Sprite_load(&lvl->bg, MYSDL_getMainRenderer(), lvl->backdrop_p);
	MYSDL_Sprite_setSourceFrame(&lvl->bg, 0, 0, 256, 192);
	MYSDL_Sprite_scaleTo(&lvl->bg, TARGET_RES_X, TARGET_RES_Y);

	//load the tileset
	MYSDL_Sprite_load(&lvl->tiles, MYSDL_getMainRenderer(), lvl->tiles_p);
	MYSDL_Sprite_scaleTo(&lvl->tiles, TILE_WIDTH, TILE_HEIGHT);
	//MYSDL_Sprite_setSourceFrame(&lvl->tiles, 0, 0, TILE_WIDTH, TILE_HEIGHT);

	//load up the enemies
	for (int i = 0; i < ENEMIES_PER_LEVEL; i++) {
		Enemy *e = &lvl->enemy_template[i];

		MYSDL_Sprite_load(&e->gfx, MYSDL_getMainRenderer(), lvl->enemies_p);
		int type = lvl->enemy_types[i];
		MYSDL_Sprite_setSourceFrame(&e->gfx, 0, 16 * type, 16, 16);
		MYSDL_Sprite_scaleTo(&e->gfx, ENEMY_WIDTH, ENEMY_HEIGHT);
	}

	//load up the boss
	//MYSDL_Sprite_load(&lvl->boss, MYSDL_getMainRenderer(), lvl->boss_p);
	//MYSDL_Sprite_setSourceFrame(&lvl->boss, 0, 0, BOSS_WIDTH, BOSS_HEIGHT);

	//load up the music for the level
	lvl->music = Mix_LoadWAV(lvl->music_p);
	lvl->speed = BASE_SCROLL_SPEED;
	memset(&lvl->rows, 0, sizeof(lvl->rows));
	return 0;
}



void Level_UpdatePlatforms(Level *lvl) {
	float gap = CALC_ROW_GAP(lvl->difficulty);
	TileRow *lastRow = NULL, *freeRow = NULL;

	int shiftUp = 0;
	for (int i = 0; i < MAX_TILE_ROWS, freeRow == NULL; i++) {
		TileRow *curRow = &lvl->rows[i];
		if (curRow->alive) {
			if (curRow->ypos <= -TILE_HEIGHT) {
				curRow->alive = 0;
				shiftUp = 1;
			}

			curRow->ypos -= lvl->speed * DeltaTime;
			if (!lastRow || curRow->ypos > lastRow->ypos)
				lastRow = curRow;

			if (shiftUp && i > 0) {
				memcpy(&lvl->rows[i - 1], curRow, sizeof(TileRow));
				curRow->alive = 0;
			}
		}
		else
			freeRow = curRow;

	}

	if (!lastRow || (lastRow->ypos < INTERNAL_RES_Y - gap && freeRow))
		generateTileRow(freeRow, MIN_GAP_SIZE);
}


void Level_DrawPlatforms(Level *lvl) {
	for (int i = 0; i < MAX_TILE_ROWS; i++) {
		TileRow *curRow = &lvl->rows[i];
		if (curRow->alive) {
			for (int x = 0; x < TILES_PER_ROW; x++) {
				if (curRow->tiles[x] != TILE_NONE)
					MYSDL_Sprite_draw(MYSDL_getMainRenderer(), &lvl->tiles, TILE_WIDTH * x, curRow->ypos);
			}
		}
	}
}

void Level_UpdateEnemies(Level *lvl) {
	int doSpawn = rand() % 100000;
	for (int i = 0; i < MAX_ENEMIES; i++) {
		Enemy *e = &lvl->enemies[i];
		if (e->state != ENEMY_STATE_NULL) {
			//process an already active enemy
			Enemy_StateMachine(e, (void *)lvl);
			MYSDL_Sprite_draw(MYSDL_getMainRenderer(), &e->gfx, e->x, e->y);
		}
		else {
			if (doSpawn < 50) {
				continue;
			}
			doSpawn = 0;
			//spawn an enemy
			int type = rand() % ENEMIES_PER_LEVEL;
			memcpy(e, &lvl->enemy_template[type], sizeof(Enemy));
			e->x = rand() % INTERNAL_RES_X;
			e->y = rand() % INTERNAL_RES_Y;
			e->state = ENEMY_STATE_SPAWN;
		}
	}
}

PlatCollision Level_GetPlatformTile(Level *lvl, float xpos, float ypos, int wd, int ht) {
	float xCenter = xpos + (wd >> 1);
	PlatCollision col = { .tile = TILE_NULL, 0 };

	for (int i = 0; i < MAX_TILE_ROWS; i++) {
		TileRow *curRow = &lvl->rows[i];
		if (ypos + ht < curRow->ypos + TILE_HEIGHT) {

			float diff = (ypos + ht) - curRow->ypos;
			if (diff < 0 || diff >(ht >> 1)) break;

			int xpos = floor(xCenter) / TILE_WIDTH;
			col.tile = curRow->tiles[xpos];
			col.yOffset = diff;
			return col;
		}
	}
	return col;
}

