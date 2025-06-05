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
	SDL_Texture* bgTex, * btTex, * optPanel, * pausePanel, * controlsKeyboardPanel, * controlsControllerPanel;

	TTF_Font* btFont;
	TTF_Font* titleFont;
	TTF_Font* controlsFont;
	TTF_Font* controls2Font;
	std::string titleText;
	std::string controlsText;
	std::string controls2Text;
	bool settingsOpen = false;
	bool fullScreen;
	bool saved;

	int musicVolume, sfxVolume;
	int optPanelX, optPanelY, optPanelW, optPanelH;
	int controlsKeyboardPanelX, controlsKeyboardPanelY, controlsKeyboardPanelW, controlsKeyboardPanelH;
	int controlsControllerPanelX, controlsControllerPanelY, controlsControllerPanelW, controlsControllerPanelH;
	float _dt;

	bool controlsOpen = false;
	bool xbox = false;

	GuiControlSlider* musicSlider, * sfxSlider;
	GuiControlCheckBox* fullScreenBox;
	GuiControlButton* backBt;
	GuiControlButton* controlsBt;
	GuiControlButton* espBt;
	GuiControlButton* changeControlBt;

	int currentLanguage = 1;

	int testSound;
	int titleVerticalDisplacement;
	int controlsVerticalDisplacement;
	int controls2VerticalDisplacement;

	std::list<GuiControl*> settingsGUI;

	Vector2D prevMousePos = { -1, -1 };
    bool mouseMoved   = false;
    bool mandoMoved   = false;
    int  selectedIndex = 0;
	int miniIndex = 0;

	//std::map<std::string, GuiControlButton*> buttons;

};
