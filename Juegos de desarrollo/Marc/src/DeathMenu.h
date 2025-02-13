#pragma once

#include "Module.h"
#include "Animation.h"
#include "GuiControlButton.h"
//#include "Render.h"
#include "GuiControlSlider.h"

#include <list>
#include <map>


struct SDL_Texture;

class DeathMenu : public Module
{
public:

	DeathMenu(bool startEnabled);

	~DeathMenu();

	bool Start() override;

	bool Update(float dt) override;

	bool CleanUp() override;


	bool PostUpdate() override;

	bool OnGuiMouseClickEvent(GuiControl* control);

	void SetGuiParameters(GuiControl* bt, std::string btName, pugi::xml_node buttonParameters);



public:

	pugi::xml_document configFile;
	pugi::xml_node rootNode;
	SDL_Texture* screenTex, *candyIcon;
	SDL_Rect candyRect, textRect;

	TTF_Font* halloweenPixels, *corvidConspirator;
	bool quit = false;
	int finalCandyNum;

	float _dt;
	std::string candyCount;

	GuiControlButton* retryBt, *backToTitleBt, *exitBt;

	std::map<std::string, GuiControlButton*> deathButtons;

};
