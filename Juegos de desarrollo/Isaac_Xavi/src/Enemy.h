#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Map.h"
#include "Timer.h"

struct SDL_Texture;
class Physics;

class Enemy : public Entity
{
public:

	Enemy();
	virtual ~Enemy();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	bool DeleteBody();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
		Start();
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	void ResetPath();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	bool isDead = false;

	SDL_RendererFlip hflip = SDL_FLIP_NONE;

	bool flipSprite = false;

	void ChangeGrounded(bool grounded);

	void HurtHutao();

	void HutaoAttack();

	void unaliveHutao();

	bool isBoss = false;

	int lifes = 4;

	bool isGrounded = false;

	bool side = false;

	bool paused = false;

	bool DrawingPath = false;

private:
	
	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation flying;
	Animation idleGrounded;
	Animation flyingGrounded;
	Animation hutaoAttack;
	Animation hutaoIdle;
	int chaseFxId;
	int chaseGroundedFxId;
	int hutaoAttackFxId;
	int hutaoHurtFxId;
	int hutaoUnaliveFxId;
	PhysBody* pbody;
	Pathfinding* pathfinding;
	Map* map;
	Physics* physics;

	bool hasStarted = false;
	bool IsSearching = false;
	int x = 0;
	int y = 0;
	int z = 0;

	Timer bossTimer;
};