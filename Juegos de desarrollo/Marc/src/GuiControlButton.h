#pragma once

#include "GuiControl.h"
#include "Vector2D.h"

class GuiControlButton : public GuiControl
{

public:

	GuiControlButton(const char* name,SDL_Rect bounds, const char* text,  SDL_Texture* tex);
	virtual ~GuiControlButton();

	// Called each loop iteration
	bool Update(float dt);

private:

	bool canClick = true;
	bool drawBasic = false;
	/*TTF_Font* font;*/

};

