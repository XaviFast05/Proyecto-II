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

	int SetVolume(GuiControlSlider* slider);
	void SavePrefs();
	void LoadPrefs();
	/*void ShowSettings();*/

public:
	// The scene sprite sheet loaded into an SDL_Texture
	pugi::xml_document configFile;
	pugi::xml_node rootNode;
	SDL_Texture* bgTex, * btTex, * optPanel, * pausePanel, * controlsKeyboardPanel;

	TTF_Font* btFont;
	TTF_Font* titleFont;
	TTF_Font* controlsFont;
	std::string titleText;
	std::string controlsText;
	bool settingsOpen = false;
	bool fullScreen;
	bool saved;

	int musicVolume, sfxVolume;
	int optPanelX, optPanelY, optPanelW, optPanelH;
	int controlsKeyboardPanelX, controlsKeyboardPanelY, controlsKeyboardPanelW, controlsKeyboardPanelH;
	float _dt;

	bool controlsOpen = false;

	GuiControlSlider* musicSlider, * sfxSlider;
	GuiControlCheckBox* fullScreenBox;
	GuiControlButton* backBt;
	GuiControlButton* controlsBt;
	GuiControlButton* espBt;

	int currentLanguage = 1;

	int testSound;
	int titleVerticalDisplacement;
	int controlsVerticalDisplacement;

	std::list<GuiControl*> settingsGUI;

	//std::map<std::string, GuiControlButton*> buttons;

};
