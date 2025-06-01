#pragma once

#include "Module.h"
#include "Animation.h"
#include "GuiControlButton.h"
#include "GuiControlSlider.h"

#include <list>
#include <map>


struct SDL_Texture;

class WinMenu : public Module
{
public:

	WinMenu(bool startEnabled);

	~WinMenu();

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

	TTF_Font* halloweenPixels, * corvidConspirator;
	bool quit = false;

	float _dt;
	std::string candyCount;
	int finalCandyNum;

	GuiControlButton* replayBt, * backToTitleBt, * exitBt;

	std::map<std::string, GuiControlButton*> winButtons;

};

