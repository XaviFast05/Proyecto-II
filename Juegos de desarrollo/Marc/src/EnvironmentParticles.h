#pragma once
#include "ParticleManager.h"
#include "Vector2D.h"
#include "Entity.h"
#include "Timer.h"
#include <vector>

struct ParticleInstance
{
	SDL_FPoint position;
	Animation anim;
	float startDelay = 0.0f;
	bool started = false;
	bool finished = false;
	float life = 0.0f;      
	float maxLife = 2.0f;
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


};
