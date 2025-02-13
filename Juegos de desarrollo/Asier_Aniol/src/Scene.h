#pragma once

#include "Module.h"
#include "Player.h"
#include "Enemy.h"
#include "EnemyFloor.h"
#include "Boss.h"
#include "Item.h"
#include "Checkpoint.h"
#include "Flag.h"
#include "Bullet.h"
#include <vector>

struct SDL_Texture;

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

	void ClearEntities();

	// Draw player health hud
	void UpdatePlayerHUD();

	// Draw boss health hud
	void UpdateBossHUD();

	// Return the player position
	Vector2D GetPlayerPosition();

	//L15 TODO 1: Implement the Load function
	void LoadState();
	//L15 TODO 2: Implement the Save function
	void SaveState();

	void ActivateEnemies();

public:
	// Get tilePosDebug value
	std::string GetTilePosDebug() {
		return tilePosDebug;

	}
	pugi::xml_node bulletNode;

	Player* player;

private:
	SDL_Texture* img;
	bool areControlsVisible = false;
	SDL_Texture* controls = nullptr;
	SDL_Texture* gameOver = nullptr;
	SDL_Texture* win = nullptr;
	SDL_Texture* credits = nullptr;
	SDL_Texture* title = nullptr;
	int bgMusic;

	bool cameraNeedsUpdate = false;

	bool hasPlacedWall = false;

	SDL_Texture* pHealth3;
	SDL_Texture* pHealth2;
	SDL_Texture* pHealth1;
	SDL_Texture* pHealth0;

	SDL_Texture* bHealth4;
	SDL_Texture* bHealth3;
	SDL_Texture* bHealth2;
	SDL_Texture* bHealth1;
	SDL_Texture* bHealth0;

	bool hasShownCredits = false;
	bool hasStarted = false;

	int currentLevel = 1;
	int maxLevel = 2;
	bool arrivedToBoss = false;
	bool hasChangedLevel = false;
	bool isPlayingMusic = false;

	int currentCheckpoint = 0;

	bool hasCreatedEnemies = false;
	bool hasDeletedEnemies = false;

	//L03: TODO 3b: Declare a Player attribute
	
	Checkpoint* checkP;
	Flag* flag;
	Boss* boss;
	SDL_Texture* mouseTileTex = nullptr;
	std::string tilePosDebug = "[0,0]";
	bool once = false;
	std::vector<Enemy*> enemyList;
	std::vector<EnemyFloor*> enemyFList;
	std::vector<Item*> itemList;
	std::vector<Bullet*> bulletList;
	std::vector<Boss*> bossList;
	std::vector<Checkpoint*> checkPList;
	std::vector<Flag*> flagList;
	bool hasReachedCheckpoint = false;
	bool hasReachedFlagpole = false;
};