//
//  mysdl_tilebg.c
//  MYSDLtest
//
//  Created by Derrick on 2014-03-27.
//  Copyright (c) 2014 test. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "mysdl_tilebg.h"
#include "mysdl_sprites.h"

#define FLIPPED_HORIZONTALLY_FLAG 0x80000000
#define FLIPPED_VERTICALLY_FLAG 0x40000000
#define TRANSPARENT_FLAG 0x20000000

/*
MYSDL_TileBG_loadTiles:

    Loads a tile set into a MYSDL_TileBG object

    arguments:
        bg: background to load tile set into
        renderer: renderer for converting surface to texture
        path: file path to tile set to load


    return:
        1 if loaded successfully, 0 if not
*/

int MYSDL_TileBG_loadTiles(MYSDL_TileBG *bg, MYSDL_Renderer *renderer, const char *path) {
    //load bg tile set
    if(!MYSDL_Sprite_load(&bg->bgTiles, renderer, path)) {
        printf("Error loading bg tiles\n");
        return 0;
    } else printf("Tiles loaded\n");

    return 1;
}


/*
 MYSDL_TileBG_setTileWd:

    Sets the width for each tile used on the map

    arguments:
        bg: bg to set tile width for
        width: tile width in pixels

 */
void MYSDL_TileBG_setTileWd(MYSDL_TileBG *bg, int tileWidth) {
    bg->tileWd = tileWidth;
}



/*
 MYSDL_TileBG_getTileWd:

    Returns the tile width for the tile bg

    arguments:
        bg: bg to query tile width for

    return:
        tile width in pixels
 */
int MYSDL_TileBG_getTileWd(MYSDL_TileBG *bg) {
    return bg->tileWd;
}

void MYSDL_TileBG_setTileScaleWd(MYSDL_TileBG *bg, int newWd) {
    bg->tileScaleX = newWd;
}

int MYSDL_TileBG_getTileScaledWd(MYSDL_TileBG *bg){
    return bg->tileScaleX;
}


/*
 MYSDL_TileBG_setTileHt:

    Sets the height for each tile used on the map

    arguments:
        bg: bg to set tile height for
        height: tile height in pixels

 */
void MYSDL_TileBG_setTileHt(MYSDL_TileBG *bg, int tileHeight) {
    bg->tileHt = tileHeight;
}



/*
 MYSDL_TileBG_getTileHt:

    Returns the tile height for the tile bg

    arguments:
        bg: bg to query tile height for

    return:
        tile height in pixels
 */
int MYSDL_TileBG_getTileHt(MYSDL_TileBG *bg) {
    return bg->tileWd;
}



void MYSDL_TileBG_setTileScaleHt(MYSDL_TileBG *bg, int newHt) {
    bg->tileScaleY = newHt;
}

int MYSDL_TileBG_getTileScaledHt(MYSDL_TileBG *bg){
    return bg->tileScaleY;
}




void MYSDL_TileBG_setProperties(MYSDL_TileBG *bg, int rows, int columns, int offsetX, int offsetY){
	bg->rowHt = rows;
	bg->colWd = columns;
	bg->offsetX = offsetX;
	bg->offsetY = offsetY;
}



int MYSDL_TileBG_getPixWd(MYSDL_TileBG *bg) {
    return MYSDL_TileBG_getTileScaledWd(bg) * bg->map.wd;
}

int MYSDL_TileBG_getPixHt(MYSDL_TileBG *bg) {
    return MYSDL_TileBG_getTileScaledHt(bg) * bg->map.ht;
}


/*
MYSDL_TileBG_setMap:

    Assign a tile map to a specified tile backgorund

    arguments:
        bg: bg to load map into
        map: pointer to tile map
        width: width in tile count of the map
        height: height in tile count of the map


    ToDo: Need to fix up tile map format eventually me thinks
*/
void MYSDL_TileBG_setMap(MYSDL_TileBG *bg, unsigned int *map, int width, int height) {
    bg->map.data = map;
    bg->map.wd = width;
    bg->map.ht = height;
}

unsigned int MYSDL_TileBG_getMapTile(MYSDL_TileMap *map, int x, int y) {
    if(x < 0 || x >= map->wd || y < 0 || y >= map->ht)
        return 0;

    unsigned int tile = map->data[x + (y * map->wd)];
    tile &= ~(FLIPPED_VERTICALLY_FLAG | FLIPPED_HORIZONTALLY_FLAG | TRANSPARENT_FLAG);
    return tile;
}


/*
MYSDL_TileBG_getSrcTilePos:

    locates a tile frame from the source texture by tile number.

    arguments:
        bg: background to get tile frame from
        tileNum: tile number to find frame for
        srcX: pointer to src x position storage
        srcY: pointer to src y position storage
*/
void MYSDL_TileBG_getSrcTilePos(MYSDL_TileBG *bg, unsigned int tileNum, int *srcX, int *srcY) {
    int tilesPerRow = MYSDL_Sprite_getBaseWd(&bg->bgTiles) / MYSDL_TileBG_getTileWd(bg);

    *srcY = tileNum / tilesPerRow;
    *srcX = abs(tileNum - ((*srcY) * tilesPerRow));

    *srcX *= MYSDL_TileBG_getTileWd(bg);
    *srcY *= MYSDL_TileBG_getTileHt(bg);
}



/*
 MYSDL_TileBG_BlankMap:

    Creates a blank tiled background for drawing on.

    arguments:
        bg: background to create blank map for
        tWd: width of each tile
        tHt: height of each tile
        wd: how many tiles wide the background is
        ht: how many tiles high the background is

    returns:
        tile count of map
 */
unsigned int MYSDL_TileBG_BlankMap (
    MYSDL_TileBG *bg,
    int tWd, int tHt,
    int wd,  int ht
) {
    //allocate the map
    unsigned int mapSize = wd * ht;
    unsigned int *map = calloc(4, mapSize);
    if(!map){
        printf("Error allocating blank map\n");
        return 0;
    }

    MYSDL_TileBG_setMap(bg, map, wd, ht);
    //MYSDL_Sprite_scaleTo(&bg->bgTiles, tWd, tHt);
    MYSDL_TileBG_setTileWd(bg, tWd);
    MYSDL_TileBG_setTileHt(bg, tHt);
    MYSDL_TileBG_setTileScaleHt(bg, tHt);
    MYSDL_TileBG_setTileScaleWd(bg, tWd);
    return mapSize;
}


/*
 MYSDL_TileBG_LoadMap:

    Loads a tile map to a tiled background.

    arguments:
        bg: background to load tile map for
        mapPath: file path to background map

    returns:
        1 if unsuccessful, 0 if successful
 */
char MYSDL_TileBG_LoadMap(MYSDL_TileBG *bg, const char *mapPath){
    //load tile map data here
    FILE *file = fopen(mapPath, "rb");
    if(!file){
        printf("error opening map file\n");
        return 1;
    }

    char mapData[12];
    fread(mapData, 1, 12, file);
    int *wd = (int*)&mapData[0];
    int *ht = (int*)&mapData[4];
    short *tWd = (short*)&mapData[8];
    short *tHt = (short*)&mapData[10];

    unsigned int mapSize = MYSDL_TileBG_BlankMap(bg, *tWd, *tHt, *wd, *ht);
    if(!mapSize){
        printf("error allocating new map\n");
        fclose(file);
        return 1;
    } else {
        printf("reading map data\n");
        fread(bg->map.data, 4, mapSize, file);
        fclose(file);
        printf("map read successfully\n");
    }
    return 0;
}


void MYSDL_TileBG_drawTile (
    MYSDL_Renderer *dest,
    int destWd, int destHt,
    MYSDL_TileBG *bg,
    unsigned int tileNum,
    int x, int y
) {

    //get the tile width and height
    //scaled width and heights are used for calculating screen positioning
    //regular width and heights are used for texture source positioning
    //to get the correct tile graphics
    int scaledWd = MYSDL_TileBG_getTileScaledWd(bg),
        scaledHt = MYSDL_TileBG_getTileScaledHt(bg),
        wd = MYSDL_TileBG_getTileHt(bg),
        ht = MYSDL_TileBG_getTileWd(bg);


    //do not draw tiles that aren't on screen
    if(x + scaledWd < 0 || x > destWd || y + scaledHt < 0 || y > destHt)
        return;

    tileNum &= ~(FLIPPED_VERTICALLY_FLAG | FLIPPED_HORIZONTALLY_FLAG | TRANSPARENT_FLAG);

    int xSrc = 0, ySrc = 0;
    MYSDL_TileBG_getSrcTilePos(bg, tileNum, &xSrc, &ySrc);
    MYSDL_Sprite_setSourceFrame(&bg->bgTiles, xSrc, ySrc, wd, ht);

    MYSDL_Sprite_scaleTo(&bg->bgTiles, scaledWd, scaledHt);
    MYSDL_Sprite_draw(dest, &bg->bgTiles, x, y);
}



//TODO: Add error checking for this function
//Static bg draws all tiles to a single texture that can be redrawn indefinitely
char MYSDL_TileBG_InitStaticBG(MYSDL_Renderer *dest, MYSDL_TileBG *bg) {
    //setup texture and bind the renderer to the texture
    if(!bg->tbgDisp) {
        SDL_Renderer *sdlrend = MYSDL_getSDLRenderer(dest);

        bg->tbgDisp = SDL_CreateTexture(sdlrend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, MYSDL_TileBG_getPixWd(bg), MYSDL_TileBG_getPixHt(bg));

        SDL_SetTextureBlendMode(bg->tbgDisp, SDL_BLENDMODE_BLEND);


        //setup for static tile bg
        SDL_SetRenderTarget(sdlrend, bg->tbgDisp);

        int tileWd = MYSDL_TileBG_getTileScaledWd(bg),
            tileHt = MYSDL_TileBG_getTileScaledHt(bg);

        unsigned int tileNumber = 0;

        for(int y = 0; y <= bg->map.ht; y++) {
            for(int x = 0; x <= bg->map.wd; x++) {

                if(!(tileNumber = MYSDL_TileBG_getMapTile(&bg->map,x, y))){
                    continue;
                }
                --tileNumber;
                MYSDL_TileBG_drawTile(dest, MYSDL_TileBG_getPixWd(bg), MYSDL_TileBG_getPixHt(bg),
                                      bg, tileNumber, (x * tileWd), (y * tileHt));
            }
        }

        SDL_SetRenderTarget(sdlrend, NULL);
        bg->isStatic = true;
    }


    return 0;
}



void MYSDL_TileBG_drawBG(
    MYSDL_Renderer *dest,
    int destWd, int destHt,
    MYSDL_TileBG *bg,
    int scrollX, int scrollY
)
{
    if(bg->isStatic) {
        SDL_Rect srcpos = {.x = 0, .y = 0, .w = MYSDL_TileBG_getPixWd(bg), .h = MYSDL_TileBG_getPixHt(bg) },
                 destpos = {.x = scrollX, .y = scrollY, .w = MYSDL_TileBG_getPixWd(bg), .h = MYSDL_TileBG_getPixHt(bg) };
        //now copied the rendered background to the main renderer
        SDL_RenderCopyEx(MYSDL_getSDLRenderer(dest), bg->tbgDisp, &srcpos, &destpos, 0, NULL, 0);
        return;
    }

    int tileWd = MYSDL_TileBG_getTileScaledWd(bg),
        tileHt = MYSDL_TileBG_getTileScaledHt(bg),

        baseTileX = scrollX / tileWd,
        baseTileY = scrollY / tileHt,
        drawX = 0, drawY = 0;

    unsigned int tileNumber = 0;


    for(int y = 0; y <= bg->rowHt; y++) {
        for(int x = 0; x <= bg->colWd; x++) {

            if(!(tileNumber = MYSDL_TileBG_getMapTile(&bg->map, baseTileX + x, baseTileY + y))){
                continue;
            }
            --tileNumber;
            //printf("drawing tile: %d\n", tileNumber);
            drawX = ((bg->offsetX + (scrollX)) % tileWd);
            drawY = ((bg->offsetY + (scrollY)) % tileHt);
            MYSDL_TileBG_drawTile(dest, destWd, destHt, bg, tileNumber,
                                  bg->offsetX - drawX + (x * tileWd),
                                  bg->offsetY - drawY + (y * tileHt));

        }
    }

}



void MYSDL_TileBG_drawIsometricBG(
        MYSDL_Renderer *dest,
        int destWd, int destHt,
        MYSDL_TileBG *bg,
        int scrollX, int scrollY
)
{
    int tileWd = MYSDL_TileBG_getTileScaledWd(bg),
        tileHt = MYSDL_TileBG_getTileScaledHt(bg),
        drawY = (scrollX + scrollY),
        drawX = (scrollX - scrollY),
        diamondX = 0,diamondY = 0;

    unsigned int tileNumber = 0;

    //this could be optimized more, but I can't figure it out right now.
    //currently just loops through all the tiles and draws only the ones
    //on screen.
    for(int x = 0; x <= bg->map.wd; x++) {
        for(int y = 0; y <= bg->map.ht; y++) {

            if(!(tileNumber = MYSDL_TileBG_getMapTile(&bg->map,  x,  y))){
                continue;
            }
            --tileNumber;

            diamondX = (x - y)*(tileWd>>1);
            diamondY = (x + y)*(tileHt>>1);

            //int drawY = -(( scrollY) * sin(.75)) + ((scrollX) * sin(.75));
            MYSDL_TileBG_drawTile(dest, destWd, destHt, bg, tileNumber,
                                  bg->offsetX - drawX + diamondX,
                                  bg->offsetY - drawY + diamondY);

        }
    }

}







