//
//  mysdl_tilebg.h
//  MYSDLtest
//
//  Created by Derrick on 2014-03-27.
//  Copyright (c) 2014 test. All rights reserved.
//

#ifndef MYSDLtest_mysdl_tilebg_h
#define MYSDLtest_mysdl_tilebg_h

#if defined (__WIN32__)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "mysdl_sprites.h"
#include "mysdl_main.h"

typedef struct MYSDL_TileMap {
    unsigned int *data;
    int wd, ht;
}MYSDL_TileMap;

typedef struct MYSDL_TileBG {
    bool isStatic;
    SDL_Renderer *tbgRenderer;
    SDL_Texture  *tbgDisp;
    
    MYSDL_Sprite bgTiles;
    int tileWd, tileHt;
    int colWd, rowHt;
    int offsetX, offsetY;
    MYSDL_TileMap map;
    
    int tileScaleX, tileScaleY;
}MYSDL_TileBG;


extern int MYSDL_TileBG_loadTiles(MYSDL_TileBG *bg, MYSDL_Renderer *renderer, const char *path);

extern void MYSDL_TileBG_setTileWd(MYSDL_TileBG *bg, int tileWidth);
extern int MYSDL_TileBG_getTileWd(MYSDL_TileBG *bg);
extern void MYSDL_TileBG_setTileScaleWd(MYSDL_TileBG *bg, int newWd);
extern int MYSDL_TileBG_getTileScaledWd(MYSDL_TileBG *bg);

extern void MYSDL_TileBG_setTileHt(MYSDL_TileBG *bg, int tileHeight);
extern int MYSDL_TileBG_getTileHt(MYSDL_TileBG *bg);
extern void MYSDL_TileBG_setTileScaleHt(MYSDL_TileBG *bg, int newHt);
extern int MYSDL_TileBG_getTileScaledHt(MYSDL_TileBG *bg);

extern void MYSDL_TileBG_setMap(MYSDL_TileBG *bg, unsigned int *map, int width, int height);
extern unsigned int MYSDL_TileBG_BlankMap (MYSDL_TileBG *bg, int tWd, int tHt, int wd,  int ht);

extern char MYSDL_TileBG_LoadMap(MYSDL_TileBG *bg, const char *mapPath);
extern char MYSDL_TileBG_InitStaticBG(MYSDL_Renderer *dest, MYSDL_TileBG *bg);


extern void MYSDL_TileBG_drawBG(MYSDL_Renderer *dest, int destWd, int destHt, MYSDL_TileBG *bg, int scrollX, int scrollY);

extern void MYSDL_TileBG_drawIsometricBG(
                                  MYSDL_Renderer *dest,
                                  int destWd, int destHt,
                                  MYSDL_TileBG *bg,
                                  int scrollX, int scrollY
                                         );

extern void MYSDL_TileBG_setProperties(MYSDL_TileBG *bg, int rows, int columns, int offsetX, int offsetY);

#endif
