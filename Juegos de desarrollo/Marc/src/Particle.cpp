#include "Particle.h"
#include "Timer.h"
#include "Engine.h"
#include "Textures.h"
#include "log.h"
#include "Scene.h"



Particle::Particle() : Entity(EntityType::SHOT)
{

}

bool Particle::Start() {

	pugi::xml_node sceneNode = Engine::GetInstance().scene.get()->configParameters;
	texture = Engine::GetInstance().textures.get()->Load(sceneNode.child("entities").child("shot").attribute("texture").as_string());
	texW = sceneNode.child("entities").child("shot").attribute("w").as_int();
	texH = sceneNode.child("entities").child("shot").attribute("h").as_int();

	shotRad = sceneNode.child("entities").child("shot").attribute("rad").as_int();
	anim.LoadAnimations(sceneNode.child("entities").child("shot").child("animations").child("travel"));
	pbody = Engine::GetInstance().physics.get()->CreateCircleSensor((int)position.getX(), (int)position.getY(), shotRad, bodyType::DYNAMIC);
	pbody->body->SetGravityScale(0);
	pbody->ctype = ColliderType::SHOT;
	pbody->body->SetLinearVelocity({ 0,0 });
	pbody->body->SetEnabled(false);

	speed = 5;
	lifeTime = 1;
	castTime = 0.5f;
	posXOffset = 20;

	isCasted = false;
	isAlive = false;

	return true;
}

bool Particle::Update(float dt)
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
			anim.Update();

			// Update the position in the screen
			pbody->body->SetLinearVelocity({ direction.getX() * speed,direction.getY() });
			position.setX(pbody->GetPhysBodyWorldPosition().getX() - texW / 2);
			position.setY(pbody->GetPhysBodyWorldPosition().getY() - texH / 2);

			if (direction.getX() > 0)Engine::GetInstance().render.get()->DrawTexture(texture, position.getX(), position.getY(), &anim.GetCurrentFrame());
			else Engine::GetInstance().render.get()->DrawTextureFlipped(texture, position.getX(), position.getY(), &anim.GetCurrentFrame());
			
		}
		else if (isAlive && aliveTimer.ReadSec() >= lifeTime)
		{
			pbody->body->SetEnabled(false);
			isCasted = false;
		}
	}
	
	return ret;
}


void Particle::SetDirection(Vector2D dir)
{
	direction = dir;
	direction = direction.normalized();
}

void Particle::SetPosition(Vector2D pos)
{
	pbody->body->SetTransform({ PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()) }, 0);
}

void Particle::Restart(Vector2D pos, Vector2D dir)
{
	pbody->body->SetLinearVelocity({ 0,0 });
	float offset = 0;
	if (dir.getX() > 0) offset = posXOffset;
	else offset = -posXOffset;
	pbody->body->SetTransform({ PIXEL_TO_METERS(pos.getX()) + PIXEL_TO_METERS(offset), PIXEL_TO_METERS(pos.getY())}, 0);
	direction = dir;
	direction = direction.normalized();
	isCasted = true;
	isAlive = false;
	castTimer.Start();
}

bool Particle::CleanUp()
{
	return true;
}