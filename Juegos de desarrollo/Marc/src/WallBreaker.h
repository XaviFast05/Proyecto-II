#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include <vector>
#include "Timer.h"
#include "CurrencyManager.h"



struct SDL_Texture;

class WallBreaker : public Entity
{
public:

	enum State {
		IDLE,
		BROKEN,
		UNKNOWN
	};

	enum Dir {
		LEFT,
		RIGHT
	};

	WallBreaker();
	virtual ~WallBreaker();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node _parameters) {
		this->parameters = _parameters;
	}

	void OnCollision(PhysBody* physA, PhysBody* physB);

	//void SaveData(pugi::xml_node itemNode);
	//void LoadData(pugi::xml_node itemNode);


	std::string name;

private:


	//int drawOffsetX = 0;
	//int drawOffsetY = 0;

	float attackTime;
	Timer attackTimer;


	//sound FX
	int hitSFX;
	int brokenSFX;

	float dropTime = 0.5f;
	Timer dropTimer;

	bool hit = false;
	int hits = 2;

	bool flipSprite;

	pugi::xml_node parameters;


	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation fractured;
	Animation broken;

	PhysBody* pbody;

	SDL_Texture* texture;
	int texW, texH;
	const char* texturePath;

	pugi::xml_node audioNode;
	State state;
	Dir dir;

	
	int amount;

};


