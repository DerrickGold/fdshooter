#ifndef __HEALTH_BAR_H__
#define __HEALTH_BAR_H__

#include "globals.h"

typedef struct HealthBar{
	float width, height;
	float maxValue;
	MYSDL_Sprite backdrop;
	MYSDL_Sprite health;
} HealthBar;

int HealthBar_Load(void);
HealthBar HealthBar_Create(float maxHealth, float width, float height);
void HealthBar_Draw(HealthBar *bar, float curHealth, float xpos, float ypos);

#endif //__HEALTH_BAR_H__