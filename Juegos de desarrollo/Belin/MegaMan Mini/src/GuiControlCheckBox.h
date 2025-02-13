#pragma once

#include "GuiControl.h"
#include "Vector2D.h"

class GuiControlCheckBox : public GuiControl
{

public:

	GuiControlCheckBox(int id, SDL_Rect bounds, const char* text, SDL_Texture* texture);
	virtual ~GuiControlCheckBox();

	// Called each loop iteration
	bool Update(float dt);

	bool activateCheckBox;

private:

	int cursorFx;
	bool drawBasic = false;
};
