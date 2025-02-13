#pragma once

#include "GuiControl.h"
#include "Vector2D.h"

class GuiControlSlider : public GuiControl
{

public:

	GuiControlSlider(int id, SDL_Rect bounds, SDL_Rect sliderBounds, const char* text, SDL_Texture* texture);
	virtual ~GuiControlSlider();

	// Called each loop iteration
	bool Update(float dt);

private:

	bool canClick = true;
	bool drawBasic = false;
	SDL_Rect slideBounds;
	int clickOffsetX;
	int volumePower;

};