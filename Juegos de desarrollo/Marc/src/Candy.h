#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Player.h"
#include <string.h>

enum CandyFunction {
	POINTS = 0,
	HEALING = 1,
	TIME = 2
};


struct SDL_Texture;

class Candy : public Entity
{
public:

	Candy();
	virtual ~Candy();

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

public:

	std::string name;
	bool picked;
	std::string type;
	CandyFunction function;
	

private:
	//L08 TODO 4: Add a physics to an item
	PhysBody* pbody;

	Player* player;

	const char* texturePath;
	int texW, texH;
	int yAnim;

	//SDL_Texture* swirlTex;
	//SDL_Texture* lollyTex;
	//SDL_Texture* cornTex;
	//SDL_Texture* clockTex;
	//SDL_Texture* heartTex;
	SDL_Texture* candyTex;

	//Animations
	Animation floating;
	Animation* currentAnim;

	//SFX
	int gotCandy;
};

