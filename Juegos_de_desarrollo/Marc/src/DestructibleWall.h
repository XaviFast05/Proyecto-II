#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Player.h"
#include <string.h>



struct SDL_Texture;


class DestructibleWall : public Entity
{
public:

	enum DestructibleWallType
	{
		UNKNOWN,
		LAYER1,
		ONIRIC
	};

	DestructibleWall();
	virtual ~DestructibleWall();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node _parameters) {
		this->parameters = _parameters;
	}

	void SaveData(pugi::xml_node itemNode);
	void LoadData(pugi::xml_node itemNode);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

private:
	//L08 TODO 4: Add a physics to an item
	PhysBody* pbody;

	SDL_Texture* texture;
	SDL_Rect currentFrame;

	bool destructed;

	Animation idle;
	Animation destroying;
	Animation* currentAnim = nullptr;

	Timer timerToDestroy;
	int timeToDestroy;

	int texW, texH;

	pugi::xml_node parameters;

	DestructibleWallType destructibleWallType;

	bool destroy;
	bool toDestroy;
};



