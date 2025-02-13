#pragma once

#include "Module.h"
#include "Player.h"
#include "Bat.h"
#include "Golem.h"
#include "Slash.h"
#include "Boss.h"
#include "Item.h"
#include "Potion.h"
#include "Checkpoint.h"
#include "GuiControlButton.h"
#include <list>

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

	//Music and FX

	int EnterHallownest;
	int Radiance;

	Vector2D GetPlayerPosition();

	void LoadState();
	void SaveState();

	bool isAttacking();
	bool isLookingRight();
	void takeDamagePlayer();
	void playerHeal();
	void takePoints();

	bool OnGuiMouseClickEvent(GuiControl* control);
	void CreateCheckpoint(Vector2D pos);


private:
	SDL_Texture* img;
	SDL_Texture* mouseTileTex = nullptr;
	std::string tilePosDebug = "[0,0]";
	bool once = false;

	Player* player;
	Bat* bat1;
	Bat* bat2;
	Golem* golem1;
	Golem* golem2;
	Slash* slash;
	Boss* boss;
	Item* key;
	Potion* potion;
	Checkpoint* checkpoint;
	//std::list<Vector2D*> checkpointList;
	//GuiControlButton* guiBt;

};