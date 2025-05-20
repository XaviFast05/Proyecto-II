#pragma once

#include "Module.h"
#include "Animation.h"
#include "GuiControlButton.h"
#include "GuiControlSlider.h"
#include "GuiControlCheckBox.h"
#include "GuiControl.h" 


#include <list>
#include <map>


struct SDL_Texture;


class UpgradesMenu : public Module
{
public:
	//Constructor
	UpgradesMenu(bool startEnabled);

	//Destructor
	virtual ~UpgradesMenu();

	bool Awake();
	bool Start();

	bool PreUpdate() override;
	bool Update(float dt);
	bool PostUpdate();

	bool CleanUp() override;

	bool OnGuiMouseClickEvent(GuiControl* control);
	void SetGuiParameters(GuiControl* bt, std::string btName, pugi::xml_node parameters);

	void SavePrefs();
	void LoadPrefs();

public:
	// The scene sprite sheet loaded into an SDL_Texture
	pugi::xml_document configFile;
	pugi::xml_node rootNode;
	SDL_Texture* bgTex, * btTex, * upgPanel, * pausePanel;

	TTF_Font* btFont;
	bool upgradesOpen = false;
	bool saved;

	int upgPanelX, upgPanelY;
	float _dt;

	GuiControlButton* backBt;

	std::list<GuiControl*> upgradesGUI;
};
