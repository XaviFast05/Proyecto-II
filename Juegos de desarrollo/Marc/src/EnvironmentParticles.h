#pragma once
#include "ParticleManager.h"
#include "Vector2D.h"
#include "Entity.h"
#include "Timer.h"


class EnvironmentParticles : public Particle
{
public:

	EnvironmentParticles();
	~EnvironmentParticles() {};

	bool Start() override;

	bool Update(float dt) override;

	int type;

	int areaW;
	int areaH;

private:
	bool active1, active2, active3, active4, active5;
	float lifeTime1, lifeTime2, lifeTime3, lifeTime4, lifeTime5;
	float castTime1, castTime2, castTime3, castTime4, castTime5;
	bool changeDisplay;


	int posX;
	int posY;

	float rotation;

};
