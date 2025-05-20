#pragma once

#include "Input.h"
#include "Render.h"
#include "Module.h"

#include "Vector2D.h"

enum class GuiControlType
{
	BUTTON,
	TOGGLE,
	CHECKBOX,
	SLIDER,
	SLIDERBAR,
	COMBOBOX,
	DROPDOWNBOX,
	INPUTBOX,
	VALUEBOX,
	SPINNER
};

enum class GuiControlState
{
	DISABLED,
	NORMAL,
	FOCUSED,
	PRESSED,
	SELECTED
};

enum class GuiControlId
{
	DEFAULT,
	NEW_GAME,
	CONTINUE,
	OPTIONS,
	CREDITS,
	QUIT,
	BACK,

	MUSIC,
	SFX,
	FULLSCREEN,
	
	RESUME,
	BACKTOTITLE,
	RETRY,

	TEST_LEVEL,
	FIRST_CAPE,

	CHANGE_MENU,

	UPG1, UPG2, UPG3, UPG4, UPG5, UPG6, UPG7, UPG8, 
};

class GuiControl
{
public:

	// Constructor
	GuiControl(GuiControlType bullet_direction, GuiControlId id = GuiControlId::DEFAULT, const char* name= "") : bullet_direction(bullet_direction), id(id), name(name), state(GuiControlState::NORMAL) {}

	// Constructor
	GuiControl(GuiControlType bullet_direction, const char* name, SDL_Rect bounds, SDL_Rect sliderBounds, const char* text, SDL_Texture* tex) :
		name(name),
		bullet_direction(bullet_direction),
		state(GuiControlState::NORMAL),
		bounds(bounds),
		sliderBounds(sliderBounds),
		text(text)
		

	{
		color.r = 255; color.g = 255; color.b = 255;
		texture = tex;
	}

	virtual ~GuiControl(){}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// 
	void SetTexture(SDL_Texture* tex)
	{
		texture = tex;
		section = { 0, 0, 0, 0 };
	}

	 
	void SetObserver(Module* module)
	{
		observer = module;
	}

	// 
	void NotifyObserver()
	{
		observer->OnGuiMouseClickEvent(this);
	}

public:

	bool active;

	bool useCamera;
	
	GuiControlId id;
	std::string name;
	GuiControlType bullet_direction;
	GuiControlState state;

	std::string text;           // Control text (if required)
	SDL_Rect bounds;        // Position and size
	SDL_Rect sliderBounds;
	SDL_Color color;        // Tint color

	SDL_Texture* texture;   // Texture atlas reference
	/*SDL_Texture* sliderTexture;*/
	SDL_Rect section;       // Texture atlas base section

	Module* observer;        // Observer 
};
