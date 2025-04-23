#pragma once

#include "Module.h"
#include "Player.h"
#include <vector>
#include "Audio.h"
#include "SDL2/SDL_mixer.h"
#include <map>
#include <list>


#define POS_TO_START_MOVING_CAMX 100
#define POS_TO_STOP_MOVING_CAMX 2180
#define CAM_EXTRA_DISPLACEMENT_X 60

#define POS_TO_START_MOVING_CAMY 248
#define POS_TO_STOP_MOVING_CAMY -190
#define CAM_EXTRA_DISPLACEMENT_Y -400

struct SDL_Texture;

class BatEnemy;
class GroundEnemy;
class Enemy;
class CheckPoint;
class EntityManager;
class GuiControlButton;
class GuiControlSlider;
class GuiControlCheckBox;


enum Levels {

	UNKNOWN,
	LVL1,
	LVL2,
};

class Scene : public Module
{
public:

	Scene(bool startEnabled);

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

	void LoadEnemy(Enemy* enemy, pugi::xml_node instanceNode);


	void LoadItem(CheckPoint* checkPoint, pugi::xml_node instanceNode);

	void LoadState();

	void LoadTimeLivesCandies();

	void SaveState();

	void SetLoadState(bool load);

	bool OnGuiMouseClickEvent(GuiControl* control);

	void SetGuiParameters(GuiControl* bt, std::string btName, pugi::xml_node parameters);

	void ChangeLevel();
	
	int GetLevel();
	
	bool GetStartBossFight();
	
	void SetStartBossFight(bool b);
	void SetBossFightKilled(bool b);

	void ChangeDirectionCameraX();
	
	void SetLevel(Levels level);

	void DrawPlayerHitsUI();

	void DrawPickaxesUI();

	void DrawCurrencyUI();
	
	std::string GetCurrentLevelString();
	
	std::string GetLevelString(Levels level);



public:
	Levels level;
	bool paused = false;
	bool help = false;
	Player* player;
	float lvl1Volume;

	float currentTime;
	bool stoppedTimer;

	SDL_Texture* helpMenu;

	GuiControlButton* resumeBt, * settingsBt, * backToTitleBt, * exitBt, *backBt;

	GuiControlCheckBox* fullScreenCheckBox;
	SDL_Texture* pausePanel;
	
	SDL_Texture* heartsTexture, * piquetaNormal, * piquetaGastada;
	
	Vector2D pausePos;

	int finalCandyNum;

	std::string timerText;
	std::string secondText;

	std::string pickaxeText;
	std::string timeTilPickaxeText;

	std::string livesText;
	std::string currencyText;

	bool cameraDirectionChangeActivation = false;

private:
	
	//L03: TODO 3b: Declare a Player attribute
	float _dt;

	std::vector<Enemy*> enemies;
	std::vector<CheckPoint*> checkPoints;
	pugi::xml_node musicNode;

	std::map<std::string, GuiControlButton*> pauseButtons;
	
	bool loadScene = false;
	bool changeLevel = false;
	bool startBossFight = false;
	bool bossMusPlaying = false;
	bool bossKilled = false;
	bool quit = false;
	
	int transitionDisplace;
	

	Vector2D helpPos;
};
