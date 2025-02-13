#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"

struct SDL_Texture;

class Checkpoint : public Entity
{
public:

	Checkpoint();
	virtual ~Checkpoint();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void OnCollision(PhysBody* physA, PhysBody* physB);


public:

	bool isPicked = false;
	pugi::xml_node parameters;

	Animation* currentAnimation = nullptr;
	Animation off;
	Animation on;

	SDL_Texture* texture = NULL;
	int texW, texH;

	PhysBody* pbody;
};