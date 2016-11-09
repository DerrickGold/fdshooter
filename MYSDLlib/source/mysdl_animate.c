//
//  mysdl_animate.c
//  sdl2-test
//
//  Created by Derrick on 2014-03-27.
//  Copyright (c) 2014 test. All rights reserved.
//
// Utilizes SDL_Timers to update animations

#include <stdio.h>
#include <string.h>
#include "mysdl_animate.h"



/*
MYSDL_Animate_setType:

    Specify what kind of animation to set the animate object to use.

    arguments:
        anim: animation object to modify
        type: either ANIMATE_EMPTY, ANIMATE_FOREVER, ANIMATE_ONESHOT, or ANIMATE_ONESHOT_REPEAT

    -ANIMATE_EMPTY
        indicates that the animation has been emptied
    -ANIMATE_FOREVER
        sets the animation into an endless loop
    -ANIMATE_ONESHOT
        causes the animation to execute once, then calls the animate->done function on completion
    -ANIMATE_ONESHOT_REPEAT
        sets the animation into an endless loop AND calls the animate->done function each
        time the animation completes one cycle.

*/
void MYSDL_Animate_setType(MYSDL_Animate *anim, Animation_Style type) {
    anim->type = type;
}




/*
 MYSDL_Animate_getType:

 Returns what type of animation an animation object is performing.

 arguments:
    anim: animation object to query

 return:
    Only one of the following:

    -ANIMATE_EMPTY
        indicates that the animation has been emptied
    -ANIMATE_FOREVER
        sets the animation into an endless loop
    -ANIMATE_ONESHOT
        causes the animation to execute once, then calls the
        animate->done function on completion
    -ANIMATE_ONESHOT_REPEAT
        sets the animation into an endless loop AND calls the
        animate->done function each time the animation
        completes one cycle.

*/
Animation_Style MYSDL_Animate_getType(MYSDL_Animate *anim) {
    return anim->type;
}



/*
MYSDL_Animate_status:

    Trigger an animation to start.
    If an animation is stopped, an animations
    timer ID is no longer valid.

    arguments:
        anim: animation to modify
        status: status to set animation

*/
void MYSDL_Animate_setStatus(MYSDL_Animate *anim, Animation_Status status) {
    //prevent overlapping animations
    if(anim->play == ANIMATE_START && status == ANIMATE_START) return;

    anim->play = status;

    if(anim->play == ANIMATE_START) {
        anim->oldTicks = 0;
        anim->perFrameMS = anim->totalTime / ((anim->endFrameNum - anim->startFrameNum));
       // anim->timer_id = SDL_AddTimer(
       //                         anim->totalTime/((anim->endFrameNum - anim->startFrameNum) + 1),
       //                         &MYSDL_Animate_update, anim);
    } else if (anim->play == ANIMATE_STOP) {
        //SDL_RemoveTimer(anim->timer_id);
    }
}


/*
MYSDL_Animate_getStatus:

    Returns the current status of an animation.

    arguments:
        anim: animation set to query

    return:
        One of the following:

        -ANIMATE_STOP
            indicates the animation has finished or has not started.
            With this status, the animation timer for the animation set,
            does not exist.

        -ANIMATE_START
            indicates the animation is currently running and there
            is and SDL_Timer associated with this animation.

        -ANIMATE_PAUSE
            currently not implemented.

*/
Animation_Status MYSDL_Animate_getStatus(MYSDL_Animate *anim) {
    return anim->play;
}



/*
MYSDL_Animate_setTimeLength:

    Sets the total time it takes for an animation
    to execute one animation cycle

    arguments:
        anim: animation to modify
        ms: time in milliseconds for animation to take
*/
void MYSDL_Animate_setTimeLength(MYSDL_Animate *anim, unsigned int ms) {
    anim->totalTime = ms;
}



/*
MYSDL_Animate_getTimeLength:
    Returns the total time it takes for an animation to
    complete one animation cycle.

    arguments:
        anim: animation set to query

    return:
        a time in milliseconds
*/
unsigned int MYSDL_Animate_getTimeLength(MYSDL_Animate *anim) {
    return anim->totalTime;
}



/*
 MYSDL_Animate_setFrameRange:

 Set the first and last frames for an animation to loop through.

 arguments:
 animation: animation to modify
 firstFrame: the first frame of the animation
 lastFrame: the last frame of the animation

 */
void MYSDL_Animate_setFrameRange(MYSDL_Animate *animation, int firstFrame, int lastFrame) {
    animation->startFrameNum = firstFrame;
    animation->endFrameNum = lastFrame;

}

/*
MYSDL_Animate_getCurrentFrame:

    Returns the current frame in the animation.

    arguments:
        anim: animation set to query

    return:
        an integer indicating the current frame.
*/

int MYSDL_Animate_getCurrentFrame(MYSDL_Animate *anim) {
    return anim->curFrameNum;
}


/*
MYSDL_Animate_update:

    A basic update function that is called every frame.
    Using SDL_Ticks, the animation is updated in real terms
    of miliseconds rather than by the frame limit

    arguments:
        interval: the interval that has been hit
        data: MYSDL_Animate data structure for the
                currently updated animation.


    return:
        returns the interval
*/
void MYSDL_Animate_update(MYSDL_Animate *animation) {
    unsigned int curTicks = SDL_GetTicks();
    unsigned int timeElapsed = curTicks - animation->oldTicks;
    if(timeElapsed < animation->perFrameMS) return;
    animation->oldTicks = curTicks;


    //wait until animation has been started before continuing
    if(animation->play != ANIMATE_START) return;

    //make sure the animation skips the needed frames
    animation->curFrameNum ++;
    //if we hit end of animation, set it back to it's original frame
    if(animation->curFrameNum > animation->endFrameNum){
        animation->curFrameNum = animation->startFrameNum;

        if(animation->type == ANIMATE_ONESHOT || animation->type == ANIMATE_ONESHOT_REPEAT){
            if(animation->done)
                (*animation->done)(animation->param);

            //only if the animation is a oneshot, stop
            //the animation when its done
            if(animation->type == ANIMATE_ONESHOT)
                MYSDL_Animate_setStatus(animation, ANIMATE_STOP);
        }
    }
}



/*
MYSDL_Animate_delete:

    Clears an animation and removes its timer.
    If an animation is stopped, it's timer is
    no longer valid

    arguments:
        animation: the target animation to delete
*/
void MYSDL_Animate_delete(MYSDL_Animate *animation){
    //if(animation->play != ANIMATE_STOP)
    //    SDL_RemoveTimer(animation->timer_id);

    memset(animation, 0, sizeof(MYSDL_Animate));
}




/*
MYSDL_Animate_reset:

    Resets an animation back to its start, and stops the animation

    arguments:
        animation: the target animation to reset

*/
void MYSDL_Animate_reset(MYSDL_Animate *animation) {
    //set the animation to it's first frame
    if(MYSDL_Animate_getStatus(animation) != ANIMATE_STOP)
        MYSDL_Animate_setStatus(animation, ANIMATE_STOP);

    animation->curFrameNum = animation->startFrameNum;
}


/*
MYSDL_Animate_setCallback:

    Set which function and what data to pass to said
    function when an animation completes one cycle.

    The animation type must be set to ANIMATE_ONESHOT
    or ANIMATE_ONESHOT_REPEAT for the callback
    to be called.

    arguments:
        anim: animation to set callback for
        fn: function to call when animation cycle completes
        param: data to pass to fn when animation cycle completes
*/
void MYSDL_Animate_setCallback(MYSDL_Animate *anim, void (*fn)(void*), void *param) {
    anim->done = fn;
    anim->param = param;
}
