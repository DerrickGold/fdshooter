#include <stdio.h>
#include <stdlib.h>
#include "mysdl_main.h"

SDL_Window *mainWindow = NULL;
MYSDL_Renderer *mainRenderer = NULL;
/*
 Window functions


*/

//SDL_Window *initDefaultWindow(const char *title, int wd, int ht){
SDL_Window *MYSDL_initDefaultWindow(const char *title, int wd, int ht) {
    return SDL_CreateWindow( title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, wd, ht,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
                            //SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);
}


//SDL_Window *getMainWindow(){
inline SDL_Window *MYSDL_getMainWindow(void) {
    return mainWindow;
}

int MYSDL_initMainWindow(const char *title, int wd, int ht){
    mainWindow = MYSDL_initDefaultWindow(title, wd, ht);
    return mainWindow != NULL;
}



/*
 Renderer functions


 */

MYSDL_Renderer *MYSDL_initRenderer(SDL_Window *window){
    MYSDL_Renderer *render = malloc(sizeof(MYSDL_Renderer));
    if (!render){
        printf("error allocating renderer\n");
        return NULL;
    }
    render->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!render->renderer){
        free(render);
        render = NULL;
        printf("error allocating SDL_Renderer\n");
    }
    return render;
}

void MYSDL_freeRenderer(MYSDL_Renderer *render){
    if (render->renderer){
        SDL_DestroyRenderer(render->renderer);
        render->renderer = NULL;
    }
    render->logX = 0;
    render->logY = 0;

    free(render);
}

inline MYSDL_Renderer *MYSDL_getMainRenderer(void){
    return mainRenderer;
}

inline SDL_Renderer *MYSDL_getSDLRenderer(MYSDL_Renderer *render){
    return MYSDL_getMainRenderer()->renderer;
}

void MYSDL_getRendererSize(MYSDL_Renderer *render, int *w, int *h){
    *w = render->logX;
    *h = render->logY;
}

void MYSDL_setRendererSize(MYSDL_Renderer *render, int w, int h){
    render->logX = w;
    render->logY = h;
    SDL_RenderSetLogicalSize(MYSDL_getSDLRenderer(render), render->logX, render->logY);
}

void MYSDL_scaleRenderer(MYSDL_Renderer *render, int xRes, int yRes){
    MYSDL_setRendererSize(render, xRes, yRes);
}

int MYSDL_initMainRenderer(int xRes, int yRes){
    mainRenderer = MYSDL_initRenderer(MYSDL_getMainWindow());
    MYSDL_scaleRenderer(mainRenderer, xRes, yRes);
    return mainRenderer != NULL;
}

inline void MYSDL_clearRenderer(MYSDL_Renderer *renderer) {
    SDL_RenderClear(MYSDL_getSDLRenderer(renderer));
}

inline void MYSDL_renderPresent(MYSDL_Renderer *renderer) {
    SDL_RenderPresent(MYSDL_getSDLRenderer(renderer));
}



void MYSDL_scaleRes(int x, int y){
    int oldX, oldY;
    MYSDL_getRendererSize(MYSDL_getMainRenderer(), &oldX, &oldY);
    double xSize = ((double)x/oldX);
    double ySize = ((double)y/oldY);

    printf("scale size: %f, %f\n", 1.0+ xSize, 1.0+ ySize);
    SDL_RenderSetScale(MYSDL_getSDLRenderer(mainRenderer), xSize, ySize);
	SDL_SetWindowSize(MYSDL_getMainWindow(), x, y);
}

void MYSDL_ToggleFullScreen(void) {
	static unsigned int status = 0;
	status = (status == 0) ? SDL_WINDOW_FULLSCREEN : 0;
	SDL_SetWindowFullscreen(MYSDL_getMainWindow(), status);
}

int MYSDL_init(int resx, int resy, char defWindow) {

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO ) < 0 ) {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    //Enable VSync
    if( !SDL_SetHint( SDL_HINT_RENDER_VSYNC, "1" ) )
    {
        printf( "Warning: VSync not enabled!" );
    }
    if( !SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1") )
    {
        printf("Warning: Scale not enabled!");
    }

    //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
        return 4;
    }

    //if default window creation disabled, return fine here
    if(!defWindow) return 0;
    //otherwise...
    //Create window
    if( !MYSDL_initMainWindow("test window", resx, resy)) {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return 2;
    }

    if (!MYSDL_initMainRenderer(resx, resy)){
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 3;
    }

    return 0;
}

void MYSDL_Quit(){
    //Destroy window
    MYSDL_freeRenderer(MYSDL_getMainRenderer());
    SDL_DestroyWindow(MYSDL_getMainWindow()); //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}
