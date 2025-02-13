#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"
#include "Timer.h"

struct SDL_Texture;

class Slash : public Entity
{
public:

	Slash();

	virtual ~Slash();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}


	Vector2D GetPosition();
	void SetPosition(Vector2D pos);

public:

	//Declare player parameters
	float speed = 3.0f;
	SDL_Texture* texture = NULL;
	int texW, texH;

	PhysBody* pbody;
	bool isAttacking = false;
	float attackDuration = 3;
	float attackTimer;

	b2Vec2 velocity;

	float playerX, playerY;

	Animation* currentAnimation = nullptr;
	Animation slashRight;
	Animation slashLeft;


	bool isLookingRight;

	b2Vec2 playerPos;

	void OnCollision(PhysBody* physA, PhysBody* physB);



private:
	pugi::xml_node parameters;

};