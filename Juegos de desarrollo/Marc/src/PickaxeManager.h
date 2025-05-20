#pragma once
#include "EntityManager.h"
#include "Entity.h"
#include "vector"
#include "Engine.h"
#include "Bullet.h"
#include "Textures.h"
#include "Player.h"
#include "Scene.h"
#include "Timer.h"

class ProjectileManager
{
public:

	ProjectileManager();
	virtual ~ProjectileManager() {};

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void ThrowPickaxe(Vector2D Direction, Vector2D Position);

	Entity* CreatePickaxe();

	void ThrowJumpProjectiles(Vector2D position);

	Entity* CreateJumpProjectile();


	int GetNumPickaxes();

	int GetNumRed();
public:

	int startNumPickaxe;
	int startNumJumpProjectiles;

	int pickaxeCount;


	Timer pickaxeRecollectTimer;
	float pickaxeRecollectCount;
	bool recollectingPickaxes = false;
	int maxPickaxes;
}; 
