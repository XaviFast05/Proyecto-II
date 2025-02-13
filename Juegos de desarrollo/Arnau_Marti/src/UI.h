#pragma once
#include "Module.h"
#include "Player.h"

#define OFFSET_X -120
#define OFFSET_Y -60

class UI : public Module
{
public:

	UI();

	virtual ~UI();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();




private:


	SDL_Texture* helpMenu;

	bool help;

};