#include "WallBrakerParticle.h"
#include "SoulRock.h"
#include "Timer.h"
#include "Engine.h"
#include "Textures.h"
#include "log.h"
#include "Scene.h"

WallBrakerParticle::WallBrakerParticle() : Entity(EntityType::WALLBRAKER_PARTICLE)
{
	name = "wallBrakerParticle";
}

bool WallBrakerParticle::Start() {

	pugi::xml_node sceneNode = Engine::GetInstance().scene.get()->configParameters;
	texture = Engine::GetInstance().textures.get()->Load(sceneNode.child("entities").child("particles").child("wallBrakerParticle").attribute("texture").as_string());
	texW = sceneNode.child("entities").child("particles").child("wallBrakerParticle").attribute("w").as_int();
	texH = sceneNode.child("entities").child("particles").child("wallBrakerParticle").attribute("h").as_int();

	rad = sceneNode.child("entities").child("particles").child("wallBrakerParticle").attribute("rad").as_int();
	idle.LoadAnimations(sceneNode.child("entities").child("particles").child("wallBrakerParticle").child("animations").child("idle"));
	pbody = Engine::GetInstance().physics.get()->CreateCircleSensor((int)position.getX(), (int)position.getY(), rad, bodyType::STATIC);

	pbody->body->SetGravityScale(0);
	pbody->ctype = ColliderType::PARTICLE;
	pbody->body->SetLinearVelocity({ 0,0 });
	pbody->body->SetEnabled(false);

	active = true;
	renderable = true;
	Enable();

	//speed = 5;
	lifeTime = 0.2f;
	castTime = 0.0f;
	//posXOffset = 20;

	currentAnim = &idle;

	isCasted = true;
	isAlive = false;
	castTimer.Start();

	return true;
}

bool WallBrakerParticle::Update(float dt)
{
	bool ret = true;
	if (isCasted)
	{
		if (!isAlive && castTimer.ReadSec() >= castTime)
		{
			isAlive = true;
			aliveTimer.Start();
			pbody->body->SetEnabled(true);
		}
		else if (isAlive && aliveTimer.ReadSec() < lifeTime)
		{
			currentAnim->Update();

			Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x - texW / 2) + 10, METERS_TO_PIXELS(pbody->body->GetPosition().y - texH / 1.5) + 10, true, ENTITIES, &currentAnim->GetCurrentFrame());

		}
		else if (isAlive && aliveTimer.ReadSec() >= lifeTime)
		{
			pbody->body->SetEnabled(false);
			isCasted = false;
			active = false;
			renderable = false;
			Disable();
		}
	}

	return ret;
}


void WallBrakerParticle::SetDirection(Vector2D dir)
{
	direction = dir;
	direction = direction.normalized();
}

void WallBrakerParticle::SetPosition(Vector2D pos)
{
	pbody->body->SetTransform({ PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()) }, 0);
}

void WallBrakerParticle::Restart(Vector2D pos, Vector2D dir)
{
	pbody->body->SetLinearVelocity({ 0,0 });
	//float offset = 0;
	//if (dir.getX() > 0) offset = posXOffset;
	//else offset = -posXOffset;
	pbody->body->SetTransform({ PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()) }, 0);
	direction = dir;
	direction = direction.normalized();
	isCasted = true;
	isAlive = false;
	castTimer.Start();
}

bool WallBrakerParticle::CleanUp()
{
	return true;
}