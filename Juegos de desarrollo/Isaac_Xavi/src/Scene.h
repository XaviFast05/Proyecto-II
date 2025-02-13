#pragma once

#include "Module.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
#include "CheckPointBF.h"
#include <vector>

#include "GuiControlButton.h"
#include "GuiControlSlider.h"
#include "GuiControlCheck.h"

struct SDL_Texture;
class Timer;
class CheckPointBF;

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

	void changeLevel(int level, bool upordown);

	// Return the player position
	Vector2D GetPlayerPosition();

	//L15 TODO 1: Implement the Load function
	void LoadState();
	//L15 TODO 2: Implement the Save function
	void SaveState();
	//Reset to original position
	void SpawnPoint();
	//Spawn in level 2
	void SpawnPointLvl2();

	// Handles multiple Gui Event methods
	bool OnGuiMouseClickEvent(GuiControl* control);

	// Handles when the Gui buttons are shown
	void ButtonManager();

	void FadeInOut(SDL_Renderer* renderer, int duration, bool fadeIn);

	void DrawUI();

public:
	// Get tilePosDebug value
	std::string GetTilePosDebug() {
		return tilePosDebug;

	}

private:
	SDL_Texture* bg;
	SDL_Texture* menuBg;
	SDL_Texture* configBg;
	SDL_Texture* title;
	SDL_Texture* endingImg;
	SDL_Texture* lifeIcon;
	SDL_Texture* coinIcon;
	SDL_Texture* endingBg;
	SDL_Texture* credits;
	SDL_Texture* fastTravel;
	SDL_Texture* die;
	SDL_Texture* introBg;
	bool ifCredits = false;
	bool CTVisible = false;
	std::string tilePosDebug = "[0,0]";
	SDL_Texture* CTtexture = nullptr;
	bool configMenu = false;
	int menu_introMS;
	int invincibilityMS;
	int final_bossMS;
	int lvl1_5MS;
	int lvl6_11MS;
	int coinFxId;
	int oneUpId;
	int pauseFxId;
	int volumeMusic;
	int volumeFx;
	int x = 0;
	int y = 0;
	bool menuMusic = false;
	bool lvl6_11Music = false;
	bool finalBossMusic = false;
	bool dieMusic = false;
	

	// Declare a Player attribute
	Player* player;
	Enemy* enemy;
	Item* item;
	CheckPointBF* checkpoint;
	CheckPointBF* checkpoint2;
	CheckPointBF* checkpoint3;
	Timer* timer;
	Physics* physics;
	std::vector<Enemy*> enemyList;
	std::vector<Item*> itemList;
	std::vector<int> takenItems;

	bool grounded = false;

	// L16: TODO 2: Declare a GUI Control Button 
	GuiControlButton* guiBt;
	GuiControlButton* guiContinue;
	GuiControlButton* guiConfig;
	GuiControlButton* guiCredits;
	GuiControlButton* guiExit;
	GuiControlSlider* guiMusicSlider;
	GuiControlSlider* guiFxSlider;
	GuiControlButton* guiBack;
	GuiControlCheck* guiCheckScreen;
	GuiControlButton* guiResume;
	GuiControlButton* guiBackToTitle;

	
	Timer playerInvencibility;
	bool playerInvincible;

	bool exitGame = false;	

	bool canContinue = false;

	bool fading;
	bool fadeIn;
	bool fadeOut;
	float alpha;
	float fadeSpeed;

	bool ending = false;
	bool dead = false;

	Timer playTime;
	int sceneSeconds = 0;
	int initialSeconds = 0;
	bool counting = false;
	bool canSave = true;


	int secondsOnScreen;
	int minutesOnScreen;
	int sceneFinalSeconds;

	bool fixFade = false;
	SDL_Color white = { 255, 255, 255, 255 };

	bool isIntro = true;
	Timer introTimer;

	GuiControl* show;
};