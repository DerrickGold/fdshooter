#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "enemy.h"

typedef enum {
	TILE_NULL,
	TILE_NONE,
	TILE_TYPE1,
} TILE_DATA;

typedef struct TileRow {
	char alive;
	float ypos;
	TILE_DATA tiles[TILES_PER_ROW];
} TileRow;

typedef struct Level {
	char *backdrop_p;
	char *tiles_p;
	char *enemies_p;
	int enemy_types[ENEMIES_PER_LEVEL];
	char *boss_p;
	char *music_p;

	float scroll, speed;
	unsigned int score;
	float difficulty;

	Mix_Chunk *music;
	MYSDL_Sprite bg;
	MYSDL_Sprite tiles;
	Enemy enemy_template[ENEMIES_PER_LEVEL];
	Enemy boss;

	TileRow rows[MAX_TILE_ROWS];
	Enemy enemies[MAX_ENEMIES];
} Level;


typedef struct PlatCollision {
	TILE_DATA tile;
	float yOffset;
	float xOffset;
} PlatCollision;

int Level_Load(Level *lvl);
void Level_UpdatePlatforms(Level *lvl);
void Level_DrawPlatforms(Level *lvl);
void Level_UpdateEnemies(Level *lvl);
PlatCollision Level_GetPlatformTile(Level *lvl, float xpos, float ypos, int wd, int ht);

#endif //__LEVEL_H__