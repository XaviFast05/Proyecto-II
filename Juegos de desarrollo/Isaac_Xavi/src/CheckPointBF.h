#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"

struct SDL_Texture;
class Scene;
class Player;

class CheckPointBF : public Entity
{
public:

	CheckPointBF();

	virtual ~CheckPointBF();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetPosition(Vector2D pos);

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

public:

	//Declare CheckPoint parameters
	SDL_Texture* texture = NULL;
	SDL_Texture* fastTravel;
	int texW, texH;

	//Audio fx
	int jumpFxId;

	// L08 TODO 5: Add physics to the Checkpoint - declare a Physics body
	PhysBody* pbody;

	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Player* player;
	Scene* scene;
	Animation notTaken;
	Animation taken;

	bool CheckTaken = false;
	bool Saving = false;
	bool onPlayer = false;
	int CheckPointTakenFxId;
};
