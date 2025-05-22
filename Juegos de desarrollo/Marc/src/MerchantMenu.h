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


class MerchantMenu : public Module
{
public:
	//Constructor
	MerchantMenu(bool startEnabled);

	//Destructor
	virtual ~MerchantMenu();

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
	void ShowInfo(int id);
	/*void ShowSettings();*/

public:
	// The scene sprite sheet loaded into an SDL_Texture
	pugi::xml_document configFile;
	pugi::xml_node rootNode;
	SDL_Texture* bgTex, * btTex, * merchantPanel;

	TTF_Font* btFont;
	bool merchantPanelOpen = false;
	//bool fullScreen;
	bool saved;

	//int musicVolume, sfxVolume;
	int merchantPanelX, merchantPanelY;
	float _dt;

	bool confirmClick;
	int beforeId;


	bool unlocked[8];
	GuiControlButton* backBt, * firstUpgradeBt, * secondUpgradeBt, * thirdUpgradeBt, * fourthUpgradeBt, * fifthUpgradeBt, * sixthUpgradeBt, * seventhUpgradeBt, * eighthUpgradeBt;

	int testSound;

	std::list<GuiControl*> merchantGUI;

	//std::map<std::string, GuiControlButton*> buttons;

};

