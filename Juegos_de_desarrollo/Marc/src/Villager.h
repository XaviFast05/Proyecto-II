#pragma once

#include "Ally.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include <vector>
#include "Timer.h"



struct SDL_Texture;

class Villager : public Ally
{
public:

	Villager();
	~Villager();

	bool Start() override;

	bool Update(float dt) override;

	void OnCollision(PhysBody* physA, PhysBody* physB) override;
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB) override;


private:

	int drawOffsetX = 0;
	int drawOffsetY = 0;

	int villagerType = 0;

	Animation walkGrandma;
	Animation idleGrandma;
	Animation walkBeard;
	Animation idleHeadset;
	Animation walkHeadset;
	Animation walkFlower;
	Animation walkHat;
	Animation walkGlasses;
	Animation walkSunGlasses;
	Animation walkLarge;
	Animation walkBigHead;
	Animation walkDog;

	int movementType = 0;
	Timer lookTimer;
	float lookTime;
	bool lookTimerOn;

	bool justTurned;
	float movementArea;
	float detectionArea;
	float initialPosX;
	float dist;

	float jumpForce;

	std::string dialog;

};

