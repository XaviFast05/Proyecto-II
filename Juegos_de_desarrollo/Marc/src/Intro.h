#pragma once

#include "Module.h"
#include "Animation.h"
#include "GuiControlButton.h"
#include "GuiControl.h" 
#include "Timer.h"


#include <list>
#include <map>


struct SDL_Texture;


class Intro : public Module
{
public:
	//Constructor
	Intro(bool startEnabled);

	//Destructor
	virtual ~Intro();

	bool Awake();
	bool Start();

	bool PreUpdate() override;
	bool Update(float dt);
	bool PostUpdate();

	bool CleanUp() override;


public:

	SDL_Texture* intro;
	Timer introTimer;
	int maxIntroTime;

};
