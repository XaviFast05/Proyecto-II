#pragma once
#include "ParticleManager.h"
#include "Vector2D.h"
#include "Entity.h"
#include "Timer.h"


class DashParticle : public Particle
{
public:

	DashParticle();
	~DashParticle(){};

	bool Start() override;

	bool Update(float dt) override;

private:

};