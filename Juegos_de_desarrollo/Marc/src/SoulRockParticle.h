#pragma once
#include "ParticleManager.h"
#include "Vector2D.h"
#include "Entity.h"
#include "Timer.h"


class SoulRockParticle : public Particle
{
public:

	SoulRockParticle();
	~SoulRockParticle(){};

	bool Start() override;

	bool Update(float dt) override;

private:


};



