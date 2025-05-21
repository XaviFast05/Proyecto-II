#pragma once

#include "Enemy.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "PickaxeManager.h"
#include <vector>



struct SDL_Texture;

class ShootingEnemy : public Enemy
{
public:

	ShootingEnemy();
	~ShootingEnemy();


	bool Start() override;

	bool Update(float dt) override;

	void OnCollision(PhysBody* physA, PhysBody* physB) override;

	void DMGEnemyPickaxe();
	
	bool shootRight = true;


private:

	int drawOffsetX = -16;
	int drawOffsetY = -16;
	Vector2D destinationPoint;
	ProjectileManager* projectileManager;
	//SOUND FX
	int batWingsSFX;
	int farBatWingsSFX;
	int batDeathSFX;


};

