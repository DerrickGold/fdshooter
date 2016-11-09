#ifndef sdl2_test_mysdl_main_h
#define sdl2_test_mysdl_main_h

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_mixer/SDL_mixer.h>

typedef struct MYSDL_Renderer {
    SDL_Renderer *renderer;
    //logical x and y output
    int logX, logY;
}MYSDL_Renderer;

extern SDL_Window *mainWindow;
extern MYSDL_Renderer *mainRenderer;



extern SDL_Window *MYSDL_initDefaultWindow(const char *title, int wd, int ht);
extern SDL_Window *MYSDL_getMainWindow(void);
extern int MYSDL_initMainWindow(const char *title, int wd, int ht);


extern MYSDL_Renderer *MYSDL_initRenderer(SDL_Window *window);
extern void MYSDL_freeRenderer(MYSDL_Renderer *render);
extern MYSDL_Renderer *MYSDL_getMainRenderer(void);
extern SDL_Renderer *MYSDL_getSDLRenderer(MYSDL_Renderer *render);
extern void MYSDL_getRendererSize(MYSDL_Renderer *render, int *w, int *h);
extern void MYSDL_setRendererSize(MYSDL_Renderer *render, int w, int h);
extern void MYSDL_scaleRenderer(MYSDL_Renderer *render, int xRes, int yRes);
extern int MYSDL_initMainRenderer(int xRes, int yRes);

extern void MYSDL_clearRenderer(MYSDL_Renderer *renderer);
extern void MYSDL_renderPresent(MYSDL_Renderer *renderer);


extern void MYSDL_scaleRes(int x, int y);
extern int MYSDL_init(int resx, int resy, char defWindow);
extern void MYSDL_Quit();

#endif
