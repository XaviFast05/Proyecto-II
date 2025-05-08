#pragma once
#include "EntityManager.h"
#include "Entity.h"
#include "vector"
#include "Engine.h"
#include "Bullet.h"
#include "Textures.h"
#include "Scene.h"
#include "Timer.h"

class PickaxeManager
{
public:

	PickaxeManager();
	virtual ~PickaxeManager() {};

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void ThrowPickaxe(Vector2D Direction, Vector2D Position);

	Entity* CreatePickaxe();

	int GetNumPickaxes();

	int GetNumRed();

public:

	int startNumPickaxe;

	int pickaxeCount;

	Timer pickaxeRecollectTimer;
	float pickaxeRecollectCount;
	bool recollectingPickaxes = false;
	int maxPickaxes;
}; 
