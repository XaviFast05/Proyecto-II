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
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB) override;


private:

	int drawOffsetX = 0;
	int drawOffsetY = 0;
	
	Animation walk;
	Animation idle;

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