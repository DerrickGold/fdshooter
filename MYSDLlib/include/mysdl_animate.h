//
//  mysdl_animate.h
//  sdl2-test
//
//  Created by Derrick on 2014-03-27.
//  Copyright (c) 2014 test. All rights reserved.
//

#ifndef sdl2_test_mysdl_animate_h
#define sdl2_test_mysdl_animate_h

#include <SDL2/SDL.h>

typedef enum {
    ANIMATE_EMPTY,
    ANIMATE_FOREVER,
    ANIMATE_ONESHOT,
    //basically animate forever, but the done
    //method will be called each time the animation
    //completes a loop
    ANIMATE_ONESHOT_REPEAT,
}Animation_Style;

typedef enum {
    ANIMATE_STOP = 0,
    ANIMATE_START,
    ANIMATE_PAUSE
}Animation_Status;

typedef struct MYSDL_Animate {
    Animation_Style type;
    char play;
    unsigned int totalTime;
    unsigned int perFrameMS;
    int startFrameNum, endFrameNum, curFrameNum;
    unsigned int oldTicks;
    
    //data to pass into done function
    void *param;
    //function to run when done
    void (*done)(void *);
}MYSDL_Animate;



//unfinished
extern void MYSDL_Animate_setType(MYSDL_Animate *anim, Animation_Style type);
extern Animation_Style MYSDL_Animate_getType(MYSDL_Animate *anim);

extern void MYSDL_Animate_setStatus(MYSDL_Animate *anim, Animation_Status status);
extern Animation_Status MYSDL_Animate_getStatus(MYSDL_Animate *anim);

extern void MYSDL_Animate_setTimeLength(MYSDL_Animate *anim, unsigned int ms);
extern unsigned int MYSDL_Animate_getTimeLength(MYSDL_Animate *anim);

extern int MYSDL_Animate_getCurrentFrame(MYSDL_Animate *anim);

extern void MYSDL_Animate_update(MYSDL_Animate *animation);
extern void MYSDL_Animate_delete(MYSDL_Animate *animation);
extern void MYSDL_Animate_setFrameRange(MYSDL_Animate *animation, int firstFrame, int lastFrame);

extern void MYSDL_Animate_reset(MYSDL_Animate *animation);

extern void MYSDL_Animate_setCallback(MYSDL_Animate *anim, void (*fn)(void*), void *param);

#endif
