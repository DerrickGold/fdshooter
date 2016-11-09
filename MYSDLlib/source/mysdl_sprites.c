//
//  mysdl_sprites.c
//  sdl2-test
//
//  Created by Derrick on 2014-03-27.
//  Copyright (c) 2014 test. All rights reserved.
//

#include <stdio.h>
#include "mysdl_sprites.h"


/*===============================================================
 Sprite Functions



 ===============================================================*/
/*
MYSDL_Sprite_load:

    Loads a sprite into a sprite handle. Supports PNG and BMP file
    formats with alpha channels.

    arguments:
        sprite: pointer to sprite handle to load sprite into
        renderer: which renderer to use for converting surface to texture
        path: filepath (relative or absolute) to image file to load

    return:
        returns 1 if sprite has loaded, 0 if failed
*/
int MYSDL_Sprite_load(MYSDL_Sprite *sprite, MYSDL_Renderer *renderer, const char *path){
    SDL_Texture *finalSpr = NULL;

    SDL_Surface *temp = IMG_Load(path);
    if (!temp){
        printf( "Unable to load image %s! SDL Error: %s\n", path, SDL_GetError() );
        return 0;
    }

    //set RGB 255,0,255 to transparent
    SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGBA(temp->format, 255,0,255, 255));


	//Convert surface to screen format
    finalSpr = SDL_CreateTextureFromSurface(MYSDL_getSDLRenderer(renderer), temp);
    if( finalSpr == NULL ) {
        printf( "Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError() );
    }
    //Get rid of old loaded surface
    SDL_FreeSurface(temp);
    sprite->texture = finalSpr;
    SDL_SetTextureBlendMode(sprite->texture, SDL_BLENDMODE_BLEND);

    //store original sprite width and height
    SDL_QueryTexture(sprite->texture, NULL, NULL, &sprite->position.w, &sprite->position.h);

    //reset frame size to default image size
    sprite->frame.srcPos.w = sprite->position.w;
    sprite->frame.srcPos.h = sprite->position.h;
    sprite->scaleWd = sprite->position.w;
    sprite->scaleHt = sprite->position.h;
    sprite->isCloned = 0;
    sprite->rotPoint.x = sprite->frame.srcPos.w >> 1;
    sprite->rotPoint.y = sprite->frame.srcPos.h >> 1;
    sprite->angle = 0;

    return sprite->texture != NULL;
}

/*
MYSDL_Sprite_clone:

    Clones the data from a source sprite to another.

    Both sprites share a texture source, deleting the
    original sprite will delete the texture used by the clone.

    Otherwise, both sprite properties are completely independent
    from each other.

    arguments:
        dest: pointer to destination sprite to clone into
        src: source sprite to clone from

    return:
        currently only returns 1, never fails

*/
int MYSDL_Sprite_clone(MYSDL_Sprite *dest, MYSDL_Sprite *src){
    memcpy(&dest->position, &src->position, sizeof(SDL_Rect));
    memcpy(&dest->frame, &src->frame, sizeof(MYSDL_Sprite_Frame));
    dest->scaleHt = src->scaleHt;
    dest->scaleWd = src->scaleWd;
    dest->texture = src->texture;
    dest->isCloned = 1;
    return 1;
}


/*
MYSDL_Sprite_delete:

    Destroys the texture used by a sprite, frees any
    memory used by the sprite and resets the sprite
    handle back to its initialized value.

    arguments:
        sprite: target sprite to delete


    Currently does not delete animation data.

*/
void MYSDL_Sprite_delete(MYSDL_Sprite *sprite){
    if (!sprite->isCloned){
        SDL_DestroyTexture(sprite->texture);
    }
    memset(&sprite->frame, 0, sizeof(MYSDL_Sprite_Frame));
    memset(&sprite->position, 0, sizeof(SDL_Rect));
    sprite->isCloned = 0;
    sprite->scaleWd = 0;
    sprite->scaleHt = 0;
    sprite->texture = NULL;
}

/*
MYSDL_Sprite_scaleTo:

    Stretches and scales a sprite to a new width and height.
    The scaling is done on draw, so the source texture
    reamins in its original size.

    arguments:
        sprite: target sprite to scale
        newWd: new width to resize sprite to
        newHt: new height to resize sprite to

*/
void MYSDL_Sprite_scaleTo(MYSDL_Sprite *sprite, int newWd, int newHt) {
    sprite->scaleWd = newWd;
    sprite->scaleHt = newHt;
}


/*
MYSDL_Sprite_rotateTo:

    A sprite will be drawn rotated to the given angle.

    arguments:
        sprite: target sprite to rotate
        angle: angle between 0.0 - 1.0 to rotate sprite to

*/
void MYSDL_Sprite_rotateTo(MYSDL_Sprite *sprite, double angle){
    sprite->angle = angle;
}


/*
MYSDL_Sprite_getRotAngle:

    Returns the current angle a sprite is rotated at.

    arguements:
        sprite: target sprite to query angle from

    return:
        angle (0.0 - 1.0) at which the sprite is rotated to
*/
double MYSDL_Sprite_getRotAngle(MYSDL_Sprite *sprite){
    return sprite->angle;
}

/*
MYSDL_Sprite_setSourceFrame:

    Selects a rectangular area from the source texture
    to draw as the sprite.

    arguments:
        sprite: target sprite to set frame source
        xpos: left x position in source texture of frame
        ypos: top y position in source texture of frame
        width: width from xpos to include in frame
        height: height from ypos to include in frame
*/
void MYSDL_Sprite_setSourceFrame(MYSDL_Sprite *sprite, int xpos, int ypos, int width, int height){
    sprite->frame.srcPos.x = xpos;
    sprite->frame.srcPos.y = ypos;
    sprite->frame.srcPos.w = width;
    sprite->frame.srcPos.h = height;

    //MYSDL_Sprite_scaleTo(sprite, width, height);
}

/*
MYSDL_Sprite_setSourceFrameR:

    A wrapper for MYSDL_Sprite_setSourceFrame that accepts
    an SDL_Rect as a argument.

    arguments:
        sprite: target sprite to set frame source
        frame: rectangular position in source texture
*/
void MYSDL_Sprite_setSourceFrameR(MYSDL_Sprite *sprite, SDL_Rect frame) {
    MYSDL_Sprite_setSourceFrame(sprite, frame.x, frame.y, frame.w, frame.h);
}



inline MYSDL_Animate *MYSDL_Sprite_getAnimation(MYSDL_Sprite *sprite) {
    return &sprite->curAnimation;
}
/*
MYSDL_Sprite_draw:

    Draws a sprite to a target renderer.

    arguments:
        dest: SDL_Renderer to draw sprite to
        sprite: sprite to draw
        xPos: x position in dest to draw sprite at
        yPos: y position in dest to draw sprite at
*/
void MYSDL_Sprite_draw(MYSDL_Renderer *dest, MYSDL_Sprite *sprite, int xPos, int yPos){
    MYSDL_Animate_update(MYSDL_Sprite_getAnimation(sprite));

    sprite->position.x = xPos;
    sprite->position.y = yPos;
    SDL_Rect destPos = {.x = xPos, .y = yPos, .w = sprite->scaleWd, .h = sprite->scaleHt };

    //account for animation
    SDL_Rect srcPos = sprite->frame.srcPos;

    srcPos.y += sprite->curAnimation.curFrameNum * sprite->frame.srcPos.h;
    SDL_RenderCopyEx(MYSDL_getSDLRenderer(dest), sprite->texture, &srcPos, &destPos,
                     sprite->angle, NULL, sprite->flipMode);
}


/*
MYSDL_Sprite_getBaseWd:
    Returns the width of the specified sprites texture

    arguments:
        sprite: the sprite to query

    return:
        pixel width of sprites texture in pixels

*/
int MYSDL_Sprite_getBaseWd(MYSDL_Sprite *sprite) {
    return sprite->position.w;
}



/*
 MYSDL_Sprite_getRenderWd:
    Returns the width of the specified sprite
    as rendered on screen.

    arguments:
        sprite: the sprite to query

    return:
        pixel width of a sprite on screen according
        to it's scale settings.

 */
int MYSDL_Sprite_getRenderWd(MYSDL_Sprite *sprite) {
    return sprite->scaleWd;
}


/*
 MYSDL_Sprite_getBaseHt:
    Returns the height of the specified sprites texture

    arguments:
        sprite: the sprite to query

    return:
        pixel height of sprites texture in pixels

 */
int MYSDL_Sprite_getBaseHt(MYSDL_Sprite *sprite) {
    return sprite->position.h;
}



/*
 MYSDL_Sprite_getRenderHt:
    Returns the height of the specified sprite
    as rendered on screen.

    arguments:
        sprite: the sprite to query

    return:
        pixel height of a sprite on screen according
        to it's scale settings.

 */
int MYSDL_Sprite_getRenderHt(MYSDL_Sprite *sprite) {
    return sprite->scaleHt;
}









