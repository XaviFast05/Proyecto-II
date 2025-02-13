#pragma once

#include "Module.h"
#include "Player.h"
#include "MetallC15.h"
#include "BattonBone.h"
#include "AmmoCharger.h"
#include "LifeCharger.h"
#include "PointsContainer.h"
#include <vector>
#include "GuiControlButton.h"
#include "GuiControlCheckBox.h"
#include "GuiControlSlider.h"

struct SDL_Texture;

enum gameState {

	INTRO,
	MENU,
	LEVEL1,
	LEVEL2,
	WIN,
	LOSE
};

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Return the player position
	Vector2D GetPlayerPosition();

	//L15 TODO 1: Implement the Load function
	void LoadState();

	//L15 TODO 2: Implement the Save function
	void SaveState();

	// Handles multiple Gui Event methods
	bool OnGuiMouseClickEvent(GuiControl* control);

public:
	// Get tilePosDebug value
	std::string GetTilePosDebug() {
		return tilePosDebug;

	}

	bool help;

	bool finishLevel;

	bool activationPreparationTimer;

	gameState state;

	bool pauseEntities;

	bool createBullet;

	pugi::xml_node bulletConfig;

	Player* player;

	int scorePoints;

private:
	int minutesTimer;
	float secondsTimer;


	SDL_Texture* startButton;
	SDL_Texture* continueButton;
	SDL_Texture* settingsButton;
	SDL_Texture* creditsButton;
	SDL_Texture* exitButton;
	SDL_Texture* resumeButton;
	SDL_Texture* backToTitleButton;
	SDL_Texture* checkboxButton;
	SDL_Texture* sliderButton;

	Uint32 lastTick;
	float deltaTime;

	SDL_Texture* mouseTileTex = nullptr;
	std::string tilePosDebug = "[0,0]";
	bool once = false;

	//L03: TODO 3b: Declare a Player attribute
	
	std::vector<MetallC15*> metallC15List;
	std::vector<BattonBone*> battonBoneList;
	std::vector<LifeCharger*> lifeChargerList;
	std::vector<AmmoCharger*> ammoChargerList;
	std::vector<PointsContainer*> pointsContainerList;

	
	SDL_Texture* helpMenu;
	
	
	float preparationTimer;
	SDL_Texture* introScreen;
	SDL_Texture* menuScreen;
	SDL_Texture* winScreen;
	SDL_Texture* loseScreen;

	bool activationPauseMenu;
	SDL_Texture* pauseMenu;

	bool activationSettings;
	SDL_Texture* settingsMenu;

	bool activationCredits;
	SDL_Texture* creditsScreen;

	bool initialRespawn;

	bool stateSaved;
	// L16: TODO 2: Declare a GUI Control Button 
	GuiControlButton* guiBt1;
	GuiControlButton* guiBt2;
	GuiControlButton* guiBt3;
	GuiControlButton* guiBt4;
	GuiControlButton* guiBt5;
	GuiControlButton* guiBt6;
	GuiControlButton* guiBt7;
	GuiControlButton* guiBt8;
	GuiControlButton* guiBt9;
	GuiControlCheckBox* guiCh;
	GuiControlSlider* guiSl1;
	GuiControlSlider* guiSl2;

	gameState lastState;

	int width, height;
	SDL_Rect dstRect = { 0, 0, 0, 0 };

	bool changedButtons1;
	bool changedButtons2;
	bool changedButtons3;

	bool selectedOption;

	bool exit;

	void HelpMenu();
	void HandleCamera();
	void LevelTimer();
	void Score();

};