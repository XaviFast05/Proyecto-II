#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"


class MetallC15 : public Entity
{
public:

	MetallC15();
	virtual ~MetallC15();

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
	bool dead;
	pugi::xml_node parameters;

	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation idleL;
	Animation walk;
	Animation walkL;
	Animation hide;
	Animation appear;

	PhysBody* pbody;
	Pathfinding* pathfinding;

	bool lookRight;

	bool appeared;

	bool activatePathfinding;

	float timer;

	int currentPathIndex;
};
