#pragma once
#include "ParticleManager.h"
#include "Vector2D.h"
#include "Entity.h"
#include "Timer.h"
#include <vector>

const std::vector<SDL_FPoint> directions = {
	{ 0.f, -1.f},  // N
	{ 1.f, -1.f},  // NE
	{ 1.f,  0.f},  // E
	{ 1.f,  1.f},  // SE
	{ 0.f,  1.f},  // S
	{-1.f,  1.f},  // SW
	{-1.f,  0.f},  // W
	{-1.f, -1.f}   // NW
};

struct ParticleInstance
{
	SDL_FPoint position;
	Animation anim;
	SDL_Texture* texture = nullptr;
	float startDelay = 0.0f;
	bool started = false;
	bool finished = false;
	float life = 0.0f;      
	float maxLife = 2.0f;
	Timer movementTimer;
	float movementInterval = 3.3f;
};

class EnvironmentParticles : public Particle
{
public:

	EnvironmentParticles();
	~EnvironmentParticles() {};

	bool Start() override;

	bool Update(float dt) override;

private:

	int areaW;
	int areaH;

	const int maxParticles = 21;
	int numParticles = 21;
	
	Timer particleTimer;

	std::vector<ParticleInstance> particles;
	Animation idle2;
	Animation idle3;
	Animation leaf;


};
