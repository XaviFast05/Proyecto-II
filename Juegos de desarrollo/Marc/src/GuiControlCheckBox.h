#pragma once

#include "GuiControl.h"
#include "Vector2D.h"


//enum CheckBoxState {
//	CHECKED,
//	UNCHECKED
//};

class GuiControlCheckBox : public GuiControl
{

public:
	GuiControlCheckBox(const char* name, SDL_Rect bounds, const char* text, SDL_Texture* tex);
	virtual ~GuiControlCheckBox();
	
	// Called each loop iteration
	bool Update(float dt);
	void SetChecked(bool b);


	bool isChecked;
private:

	TTF_Font* font;
	

};
