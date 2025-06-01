#pragma once

#include "Module.h"
#include "GuiControl.h"

#include <list>
class GuiControl;

class GuiManager : public Module
{
public:

	// Constructor
	GuiManager(bool startEnabled);

	// Destructor
	virtual ~GuiManager();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Additional methods
	GuiControl* CreateGuiControl(GuiControlType bullet_direction, const char* name, const char* text, SDL_Rect bounds, Module* observer, SDL_Rect sliderBounds = { 0,0,0,0 }, SDL_Texture* tex = nullptr, SDL_Texture* sliderTex = nullptr);

public:

	std::list<GuiControl*> guiControlsList;
	SDL_Texture* texture;
	float musicVolume;
	float fxVolume;
};