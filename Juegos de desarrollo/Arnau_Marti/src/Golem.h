#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"

struct SDL_Texture;


class Golem : public Entity
{
public:

	Golem();
	virtual ~Golem();

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

	State state;

	bool debug;

	bool isLookingRight;

	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	int lives;
	bool dead;

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
	PhysBody* pbody;
	Pathfinding* pathfinding;
};
