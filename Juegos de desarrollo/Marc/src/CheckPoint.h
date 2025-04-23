#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Player.h"
#include <string.h>



struct SDL_Texture;


class CheckPoint : public Entity
{
public:

	CheckPoint();
	virtual ~CheckPoint();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node _parameters) {
		this->parameters = _parameters;
	}

	void SetPlayer(Player* _player);

	void SaveData(pugi::xml_node itemNode);
	void LoadData(pugi::xml_node itemNode);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

public:

	
	std::string name;

private:
	//L08 TODO 4: Add a physics to an item
	PhysBody* pbody;

	Player* player;

	SDL_Texture* texture;
	SDL_Rect currentFrame;

	bool alight;

	int texW, texH;
	Animation lit;
	Animation unlit;
	Animation* currentAnim = nullptr;

	pugi::xml_node parameters;
};



