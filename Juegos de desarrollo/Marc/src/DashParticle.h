#pragma once

#include "Vector2D.h"
#include "Entity.h"
#include "Physics.h"
#include "Timer.h"


class DashParticle : public Entity
{
public:

	DashParticle();
	virtual ~DashParticle() {};

	bool Start();


	bool Update(float dt);

	//void OnCollision(PhysBody* physA, PhysBody* physB);

	void SetDirection(Vector2D dir);
	void SetPosition(Vector2D pos);
	void Restart(Vector2D pos, Vector2D dir);

	bool CleanUp();

public:

	PhysBody* pbody;
	SDL_Texture* texture;

	Vector2D position;
	int posXOffset;
	Vector2D direction;
	int speed;
	Animation anim;
	int texW, texH;
	int shotRad;

	Timer aliveTimer;
	Timer castTimer;

	bool isAlive;
	bool isCasted;

	//Should be uint
	float lifeTime;
	float castTime;
};