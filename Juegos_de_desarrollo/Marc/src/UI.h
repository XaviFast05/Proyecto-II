#pragma once
#include "Module.h"
#include "Timer.h"

//#define HELP_MENU_X_DISPLACEMENT 125
//#define HELP_MENU_Y_DISPLACEMENT 10
//
//#define PAUSE_MENU_X_DISPLACEMENT 150
//#define PAUSE_MENU_Y_DISPLACEMENT 10
struct SDL_Texture;

class UI : public Module
{
public:

	UI(bool startEnabled);

	// Destructor
	virtual ~UI();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();




private:


	SDL_Texture* intro;
	Timer introTimer;
	int maxIntroTime;
	/*pugi::xml_node parameters;*/

};