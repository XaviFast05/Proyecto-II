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


class Settings : public Module
{
public:
	//Constructor
	Settings(bool startEnabled);

	//Destructor
	virtual ~Settings();

	bool Awake();
	bool Start();

	bool PreUpdate() override;
	bool Update(float dt);
	bool PostUpdate();

	bool CleanUp() override;

	bool OnGuiMouseClickEvent(GuiControl* control);
	void SetGuiParameters(GuiControl* bt, std::string btName, pugi::xml_node parameters);

	int SetVolume(GuiControlSlider* slider);
	void SavePrefs();
	void LoadPrefs();
	/*void ShowSettings();*/

public:
	// The scene sprite sheet loaded into an SDL_Texture
	pugi::xml_document configFile;
	pugi::xml_node rootNode;
	SDL_Texture* bgTex, * btTex, * optPanel, * pausePanel;

	TTF_Font* btFont;
	bool settingsOpen = false;
	bool fullScreen;
	bool saved;

	int musicVolume, sfxVolume;
	int optPanelX, optPanelY;
	float _dt;


	GuiControlSlider* musicSlider, * sfxSlider;
	GuiControlCheckBox* fullScreenBox;
	GuiControlButton* backBt;

	int testSound;

	std::list<GuiControl*> settingsGUI;

	//std::map<std::string, GuiControlButton*> buttons;

};
