#include "globals.h"
#include "healthbar.h"

#define HEALTH_BORDER 1

static MYSDL_Sprite gfxTemplate;

int HealthBar_Load(void) {
	MYSDL_Sprite_load(&gfxTemplate, MYSDL_getMainRenderer(), HEALTHBAR_GFX);
	MYSDL_Sprite_setSourceFrame(&gfxTemplate, 0, 0, 16, 8);
	return 0;
}



HealthBar HealthBar_Create(float maxHealth, float width, float height) {
	HealthBar newBar = {
		.width = width,
		.height = height,
		.maxValue = maxHealth
	};

	MYSDL_Sprite_clone(&newBar.backdrop, &gfxTemplate);
	MYSDL_Sprite_setSourceFrame(&newBar.backdrop, 0, 0, 16, 8);

	MYSDL_Sprite_clone(&newBar.health, &gfxTemplate);
	MYSDL_Sprite_setSourceFrame(&newBar.health, 0, 8, 16, 8);

	MYSDL_Sprite_scaleTo(&newBar.backdrop, newBar.width, newBar.height);
	//keep 1 pixel boarder around the health for visibility
	MYSDL_Sprite_scaleTo(&newBar.health, newBar.width - (HEALTH_BORDER * 2), newBar.height - (HEALTH_BORDER * 2));

	return newBar;
}

void HealthBar_Draw(HealthBar *bar, float curHealth, float xpos, float ypos) {

	float healthWidth = (curHealth / bar->maxValue) * (bar->width - (HEALTH_BORDER * 2));
	MYSDL_Sprite_scaleTo(&bar->health, healthWidth, bar->height - (HEALTH_BORDER * 2));

	MYSDL_Sprite_draw(MYSDL_getMainRenderer(), &bar->backdrop, xpos, ypos);
	MYSDL_Sprite_draw(MYSDL_getMainRenderer(), &bar->health, xpos + HEALTH_BORDER, ypos + HEALTH_BORDER);
}