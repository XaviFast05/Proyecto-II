#pragma once

#include "Module.h"
#include "Animation.h"
#include "GuiControlButton.h"

#include "GuiControlSlider.h"

#include <list>
#include <map>


struct SDL_Texture;

class MainMenu : public Module
{
public:
	//Constructor
	MainMenu(bool startEnabled);

	//Destructor
	~MainMenu();

	// Called when the module is activated
	// Loads the necessary textures for the map background
	bool Start() override;

	// Called at the middle of the application loop
	// Updates the scene's background animations
	bool Update(float dt) override;

	bool CleanUp() override;

	// Called at the end of the application loop.
	// Performs the render call of all the parts of the scene's background
	bool PostUpdate() override;

	bool OnGuiMouseClickEvent(GuiControl* control);

	void SetGuiParameters(GuiControl* bt, std::string btName, pugi::xml_node buttonParameters);

	/*void ShowSettings();*/

public:
	// The scene sprite sheet loaded into an SDL_Texture
	pugi::xml_document configFile;
	pugi::xml_node rootNode;
	SDL_Texture* bgTex, * btTex, *optPanel, *pausePanel, *credits;

	TTF_Font* btFont;
	bool quit = false;
	bool saved;
	bool settingsOpen = false;
	bool creditsOpen = false;

	float titleVolume;
	int optPanelX, optPanelY;
	float _dt;


	GuiControlSlider* musicSlider, *sfxSlider;
	GuiControlButton* backBt;

	std::map<std::string, GuiControlButton*> buttons;
};