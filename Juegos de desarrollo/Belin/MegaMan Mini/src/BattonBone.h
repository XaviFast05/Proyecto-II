#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"


class BattonBone : public Entity
{
public:

	BattonBone();
	virtual ~BattonBone();

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

	void OnCollision(PhysBody* physA, PhysBody* physB);


public:

private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;
	bool dead;

	Animation* currentAnimation = nullptr;
	Animation fly;
	Animation hide;
	Animation appear;

	PhysBody* pbody;
	Pathfinding* pathfinding;

	bool appeared;

	bool activatePathfinding;

	float timer;

	int currentPathIndex;
};
