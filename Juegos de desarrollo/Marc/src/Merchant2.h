#pragma once

#include "Ally.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include <vector>
#include "Timer.h"



struct SDL_Texture;

class Merchant : public Ally
{
public:

	Merchant();
	~Merchant();


	bool Start() override;

	bool Update(float dt) override;

	void OnCollision(PhysBody* physA, PhysBody* physB) override;



private:

	int drawOffsetX = 0;
	int drawOffsetY = 0;
	Vector2D destinationPoint;
	Animation walk;
	float attackTime;
	Timer attackTimer;

	bool attacked;

	float dist;

	float jumpForce;

	//sound FX
	int swordSlashSFX;
	int skeletonDeathSFX;
};

