#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
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

	bool hasSounded;
	bool hasWon;
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

public:

	bool isPicked = false;
	

private:

	SDL_Texture* texture;
	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation beam;
	pugi::xml_node parameters;
	int texW, texH;
	
	int checkpointSFX;
	//L08 TODO 4: Add a physics to an item
	PhysBody* pbody;
};
