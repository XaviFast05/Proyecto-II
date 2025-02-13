#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"

struct SDL_Texture;


class Boss : public Entity
{
public:

	Boss();
	virtual ~Boss();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	void ResetPath();

	Vector2D distanceVec;

	float distance;

	void Chase();

	void Patrol();

	void Attack();

	void takeDamage();

	State state;

	bool debug;

	bool isLookingRight;

	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	int lives;
	bool dead;
	bool isAttacking;

	float attackDuration = 0.32;
	float attackTimer = 0.0f;

public:

private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idleRight;
	Animation idleLeft;
	Animation attackRight;
	Animation attackLeft;
	Animation walkRight;
	Animation walkLeft;
	Animation dieRight;
	Animation dieLeft;
	Animation hurtRight;
	Animation hurtLeft;
	PhysBody* pbody;
	Pathfinding* pathfinding;
};
