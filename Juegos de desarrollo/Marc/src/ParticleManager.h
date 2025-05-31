#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Timer.h"

struct SDL_Texture;


class Particle : public Entity
{
public:

	Particle();
	virtual ~Particle() {};

	bool Awake();
	virtual bool Start();
	virtual bool Update(float dt);

	void SetDirection(Vector2D dir);
	void SetPosition(Vector2D pos);
	void Restart(Vector2D pos, Vector2D dir);

	bool CleanUp();

public:

	PhysBody* pbody;
	SDL_Texture* texture;

	Vector2D position;
	Vector2D direction;
	int speed;

	Animation* currentAnim = nullptr;

	Animation idle;
	int texW, texH;
	int rad;

	Timer aliveTimer;
	Timer castTimer;

	bool isAlive;
	bool isCasted;

	float lifeTime;
	float castTime;
};