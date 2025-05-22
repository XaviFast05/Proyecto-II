#pragma once

#include "Input.h"
#include "Render.h"
#include "Module.h"
#include "pugixml.hpp"
#include "Vector2D.h"
#include "Textures.h"
#include "Engine.h"

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

	FIRST_UPGRADE,
	SECOND_UPGRADE,
	THIRD_UPGRADE,
	FOURTH_UPGRADE,
	FIFTH_UPGRADE,
	SIXTH_UPGRADE,
	SEVENTH_UPGRADE,
	EIGHTH_UPGRADE

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

	void SetGuiParameters(std::string btName, pugi::xml_node parameters)
	{
		id = (GuiControlId)parameters.child(btName.c_str()).attribute("id").as_int();
		
		if (bullet_direction == GuiControlType::SLIDER) {
			sliderBounds.x = parameters.child(btName.c_str()).attribute("circleX").as_int();
			sliderBounds.y = parameters.child(btName.c_str()).attribute("circleY").as_int();
			sliderBounds.w = parameters.child(btName.c_str()).attribute("circleW").as_int();
			sliderBounds.h = parameters.child(btName.c_str()).attribute("circleH").as_int();
			sliderTexture = Engine::GetInstance().textures.get()->Load(parameters.child(btName.c_str()).attribute("circleTexture").as_string());
		}

		bounds.x = parameters.child(btName.c_str()).attribute("x").as_int();
		bounds.y = parameters.child(btName.c_str()).attribute("y").as_int();
		bounds.w = parameters.child(btName.c_str()).attribute("w").as_int();
		bounds.h = parameters.child(btName.c_str()).attribute("h").as_int();

		texture = Engine::GetInstance().textures.get()->Load(parameters.child(btName.c_str()).attribute("texture").as_string());
		text = parameters.child(btName.c_str()).attribute("text").as_string();
		font = TTF_OpenFont(parameters.child(btName.c_str()).attribute("font").as_string(), parameters.child(btName.c_str()).attribute("fontSize").as_int());
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
	TTF_Font* font;

	SDL_Texture* texture;   // Texture atlas reference
	SDL_Texture* sliderTexture;
	/*SDL_Texture* sliderTexture;*/
	SDL_Rect section;       // Texture atlas base section

	Module* observer;        // Observer 
};
