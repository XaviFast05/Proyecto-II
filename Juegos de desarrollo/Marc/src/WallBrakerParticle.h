#pragma once
#include "ParticleManager.h"
#include "Vector2D.h"
#include "Entity.h"
#include "Timer.h"


class WallBrakerParticle : public Particle
{
public:

	WallBrakerParticle();
	~WallBrakerParticle(){};

	bool Start() override;
	bool Update(float dt) override;


private:

};