#include "globals.h"

//Globally Accessible values
int NumPlayers = 1;
double DeltaTime = 0.0f;
Uint8 *GlobalKeyState;
SDL_Event GlobalEvents;
Player GlobalPlayers[MAX_PLAYERS];

//private values for updating DeltaTime
static unsigned int oldTicks = 0;
static unsigned int targetFPS = 60;


Level GlobalLevels[LVL_COUNT] = {
  (Level) {
    .backdrop_p = "./gfx/bg1.bmp",
    .tiles_p = "./gfx/tiles1.png",
    .enemies_p = PLAYER_SPRITE_FILE,
    .enemy_types = {22, 22, 22, 22, 22},
    .boss_p = "./gfx/boss1.bmp",
    .music_p = "./gfx/music1.wav",
  },

  (Level) {0}
};


void Global_Update(void) {
  GlobalKeyState = SDL_GetKeyboardState(NULL);
  unsigned int curTicks = SDL_GetTicks();
  if(curTicks > oldTicks) {
    DeltaTime = (double)(curTicks - oldTicks)/targetFPS;
    oldTicks = curTicks;
  }
}

int box_collision(MYSDL_Sprite *s1, MYSDL_Sprite *s2) {
  int w1 = MYSDL_Sprite_getRenderWd(s1);
  int h1 = MYSDL_Sprite_getRenderHt(s1);

  int w2 = MYSDL_Sprite_getRenderWd(s1);
  int h2 = MYSDL_Sprite_getRenderHt(s1);

  return ((s1->position.x < s2->position.x + w2 && s1->position.x + w1 > s2->position.x) &&
          (s1->position.y < s2->position.y + h2 && s1->position.y + h1 > s2->position.y));
}