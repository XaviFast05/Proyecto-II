#pragma once

#include "GuiControl.h"
#include "Vector2D.h"

class GuiControlButton : public GuiControl
{

public:

	GuiControlButton(int id, SDL_Rect bounds, const char* text);
	virtual ~GuiControlButton();

	// Called each loop iteration
	bool Update(float dt);

private:

	bool canClick = true;
	bool drawBasic = false;
	bool chargeSounds = false;
	bool focused = false;
	bool pressed = false;

	//Colors for the button
	SDL_Color white = { 255, 255, 255, 255 };
	SDL_Color black = { 0, 0, 20, 0 };
	SDL_Color grey = { 100, 100, 100, 100 };
	SDL_Color yellow = { 255, 255, 0, 255 };

	//button FX
	int focusedFxId;
	int pressedFxId;

};
