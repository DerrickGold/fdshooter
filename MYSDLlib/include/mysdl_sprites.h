//
//  mysdl_sprites.h
//  sdl2-test
//
//  Created by Derrick on 2014-03-27.
//  Copyright (c) 2014 test. All rights reserved.
//

#ifndef sdl2_test_mysdl_sprites_h
#define sdl2_test_mysdl_sprites_h

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include "mysdl_animate.h"
#include "mysdl_main.h"


typedef struct MYSDL_Sprite_Frame {
    SDL_Rect srcPos;
}MYSDL_Sprite_Frame;


typedef struct MYSDL_Sprite {
    //original image width
    SDL_Rect position;
    //sprites current frame data
    MYSDL_Sprite_Frame frame;
    MYSDL_Animate curAnimation;

    //scaling info
    int scaleWd, scaleHt;
    //graphic data
    SDL_Texture *texture;

    //rotation stuff
    double angle;
    SDL_Point rotPoint;
    SDL_RendererFlip flipMode;
    char isCloned;
}MYSDL_Sprite;


extern int MYSDL_Sprite_load(MYSDL_Sprite *sprite, MYSDL_Renderer *renderer, const char *path);
extern int MYSDL_Sprite_clone(MYSDL_Sprite *dest, MYSDL_Sprite *src);
extern void MYSDL_Sprite_delete(MYSDL_Sprite *sprite);

extern void MYSDL_Sprite_scaleTo(MYSDL_Sprite *sprite, int newWd, int newHt);

extern void MYSDL_Sprite_rotateTo(MYSDL_Sprite *sprite, double angle);
extern double MYSDL_Sprite_getRotAngle(MYSDL_Sprite *sprite);

extern int MYSDL_Sprite_getBaseWd(MYSDL_Sprite *sprite);
extern int MYSDL_Sprite_getRenderWd(MYSDL_Sprite *sprite);

extern int MYSDL_Sprite_getBaseHt(MYSDL_Sprite *sprite);
extern int MYSDL_Sprite_getRenderHt(MYSDL_Sprite *sprite);

extern MYSDL_Animate *MYSDL_Sprite_getAnimation(MYSDL_Sprite *sprite) ;

extern void MYSDL_Sprite_setSourceFrame(MYSDL_Sprite *sprite, int xpos, int ypos, int width, int height);
extern void MYSDL_Sprite_setSourceFrameR(MYSDL_Sprite *sprite, SDL_Rect frame);
extern void MYSDL_Sprite_draw(MYSDL_Renderer *dest, MYSDL_Sprite *sprite, int xPos, int yPos);


#endif
