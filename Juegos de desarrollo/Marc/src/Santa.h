#pragma once

#include "Enemy.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include <vector>



struct SDL_Texture;

class Santa : public Enemy
{
public:

	Santa();
	~Santa();


	bool Start() override;

	bool Update(float dt) override;

	void OnCollision(PhysBody* physA, PhysBody* physB) override;
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB) override;

	void DMGEnemy(PhysBody* physA, PhysBody* physB);
	void KillEnemy();


private:

	int drawOffsetX = -16;
	int drawOffsetY = -16;
	Vector2D destinationPoint;

	float jumpRange;
	Timer jumpCooldownTimer;
	float jumpCooldown;

	float attackRange;
	Timer attackTimer;
	float attackTime;

	bool inFloor;
	bool jumped;

	int lives;
	float pushForce;
	float hurtTime;
	Timer hurtTimer;

	//SOUND FX
	int punchSFX;
	int hurtSFX;
	int jumpSFX;


};
