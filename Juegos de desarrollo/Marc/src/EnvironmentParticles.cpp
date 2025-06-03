#include "EnvironmentParticles.h"
#include "SoulRock.h"
#include "Timer.h"
#include "Engine.h"
#include "Textures.h"
#include "log.h"
#include "Scene.h"

EnvironmentParticles::EnvironmentParticles()
{
	name = "environmentParticles";
}

bool EnvironmentParticles::Start() {

	pugi::xml_node sceneNode = Engine::GetInstance().scene.get()->configParameters;
	texture = Engine::GetInstance().textures.get()->Load(sceneNode.child("entities").child("particles").child("environmentParticles").attribute("texture").as_string());
	position.setX(Engine::GetInstance().scene.get()->camX);
	position.setY(Engine::GetInstance().scene.get()->camY);
	texW = sceneNode.child("entities").child("particles").child("environmentParticles").attribute("w").as_int();
	texH = sceneNode.child("entities").child("particles").child("environmentParticles").attribute("h").as_int();
	areaW = Engine::GetInstance().scene.get()->camW;
	areaH = Engine::GetInstance().scene.get()->camH;


	idle.LoadAnimations(sceneNode.child("entities").child("particles").child("environmentParticles").child("animations").child("idle"));
	idle2.LoadAnimations(sceneNode.child("entities").child("particles").child("environmentParticles").child("animations").child("idle2"));
	idle3.LoadAnimations(sceneNode.child("entities").child("particles").child("environmentParticles").child("animations").child("idle3"));
	pbody = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), areaW , areaH, bodyType::STATIC);

	pbody->body->SetGravityScale(0);
	pbody->ctype = ColliderType::PARTICLE;
	pbody->body->SetLinearVelocity({ 0,0 });
	pbody->body->SetEnabled(true);

	particleTimer.Start();
	lifeTime = 0.0f;
	currentAnim = &idle;
	active = true;
	isCasted = false;
	isAlive = false;

	return true;
}

bool EnvironmentParticles::Update(float dt)
{
	bool ret = true;

	//Cam details
	int camX = Engine::GetInstance().scene.get()->camX;
	int camY = Engine::GetInstance().scene.get()->camY;
	int camW = Engine::GetInstance().scene.get()->camW;
	int camH = Engine::GetInstance().scene.get()->camH;


	//Update each particle
	for (auto& p : particles)
	{
		if (p.finished) continue;

		//Compares particle delay time creation with global paticle timer
		if (particleTimer.ReadSec() >= p.startDelay)
		{
			// Particle creation
			if (!p.started)
			{
				p.started = true;
				p.anim.Reset();
				p.life = 0.0f;
			}

			p.life += 0.005f;
			p.anim.Update();

			//Draw particle at his fixed position
			Engine::GetInstance().render.get()->DrawTextureBuffer(texture, (int)p.position.x, (int)p.position.y, false, BETWEEN_MAP, &p.anim.GetCurrentFrame());

			//Particle lifetime finishes
			if (p.life > p.maxLife)
			{
				p.finished = true;
			}

		}
	}

	//Erase particles that have finished
	particles.erase(std::remove_if(particles.begin(), particles.end(),[](auto const& q) { return q.finished; }),particles.end());

    //Particles left creation
	int alive = (int)particles.size();
	int left = maxParticles - alive;
	for (int i = 0; i < left; ++i)
	{
		ParticleInstance newParticle;
		newParticle.position.x = float((rand() % camW) + camX);
		newParticle.position.y = float((rand() % camH) + camY);
		int animChoice = rand() % 3;
		switch (animChoice)
		{
			case 0: newParticle.anim = idle; 
				break;
			case 1: newParticle.anim = idle2; 
				break;
			case 2: newParticle.anim = idle3; 
				break;
		}
		newParticle.anim.Reset();
		newParticle.startDelay = (rand() % 9) * 0.1f;   
		switch (animChoice)
		{
		case 0: newParticle.maxLife = 0.2f;
			break;
		case 1: newParticle.maxLife = 0.3f;
			break;
		case 2: newParticle.maxLife = 0.4f;
			break;
		}
		newParticle.started = false;
		newParticle.finished = false;
		particles.push_back(newParticle);
	}

	return ret;
}
