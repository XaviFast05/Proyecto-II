#include "DashParticle.h"
#include "SoulRock.h"
#include "Timer.h"
#include "Engine.h"
#include "Textures.h"
#include "log.h"
#include "Scene.h"

DashParticle::DashParticle() : Entity(EntityType::DASH_PARTICLE)
{
	name = "dashParticle";
}

bool DashParticle::Start() {

	pugi::xml_node sceneNode = Engine::GetInstance().scene.get()->configParameters;
	texture = Engine::GetInstance().textures.get()->Load(sceneNode.child("entities").child("particles").child("dashParticle").attribute("texture").as_string());
	texW = sceneNode.child("entities").child("particles").child("dashParticle").attribute("w").as_int();
	texH = sceneNode.child("entities").child("particles").child("dashParticle").attribute("h").as_int();

	rad = sceneNode.child("entities").child("particles").child("dashParticle").attribute("rad").as_int();
	idle.LoadAnimations(sceneNode.child("entities").child("particles").child("dashParticle").child("animations").child("idle"));
	pbody = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), texW/3, texH, bodyType::DYNAMIC);

	pbody->body->SetGravityScale(0);
	pbody->ctype = ColliderType::PARTICLE;
	pbody->body->SetLinearVelocity({ 0,0 });
	pbody->body->SetEnabled(true);



	lifeTime = 0.3f;


	currentAnim = &idle;
	active = true;
	isCasted = false;
	isAlive = false;


	return true;
}

bool DashParticle::Update(float dt)
{
	bool ret = true;

	if (isCasted)
	{
		if (!isAlive)
		{
			isAlive = true;
			aliveTimer.Start();
			pbody->body->SetEnabled(true);
			active = true;
			renderable = true;
			Enable();
		}
		else if (isAlive && aliveTimer.ReadSec() < lifeTime)
		{
			currentAnim->Update();

			if (Engine::GetInstance().scene.get()->player->dir == RIGHT) {
				Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) - texW / 2, METERS_TO_PIXELS(pbody->body->GetPosition().y - texH / 2), false, ENTITIES, &currentAnim->GetCurrentFrame());
				Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x ) - texW / 2 - 7, METERS_TO_PIXELS(pbody->body->GetPosition().y - texH / 2), false, ENTITIES, &currentAnim->GetCurrentFrame());
				Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) - texW / 2 - 15, METERS_TO_PIXELS(pbody->body->GetPosition().y - texH / 2), false, ENTITIES, &currentAnim->GetCurrentFrame());
				Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) - texW / 2 - 22, METERS_TO_PIXELS(pbody->body->GetPosition().y - texH / 2), false, ENTITIES, &currentAnim->GetCurrentFrame());

			}
			else if (Engine::GetInstance().scene.get()->player->dir == LEFT) {
				Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) - texW / 2 , METERS_TO_PIXELS(pbody->body->GetPosition().y - texH / 2), true, ENTITIES, &currentAnim->GetCurrentFrame());
				Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) - texW / 2 + 7, METERS_TO_PIXELS(pbody->body->GetPosition().y - texH / 2), true, ENTITIES, &currentAnim->GetCurrentFrame());
				Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) - texW / 2 + 15, METERS_TO_PIXELS(pbody->body->GetPosition().y - texH / 2), true, ENTITIES, &currentAnim->GetCurrentFrame());
				Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) - texW / 2 + 22, METERS_TO_PIXELS(pbody->body->GetPosition().y - texH / 2), true, ENTITIES, &currentAnim->GetCurrentFrame());
				
			}

		}
		else if (isAlive && aliveTimer.ReadSec() >= lifeTime)
		{
			pbody->body->SetEnabled(false);
			isCasted = false;
			isAlive = false;
			active = false;
			renderable = false;

		}
		b2Vec2 tf = Engine::GetInstance().scene.get()->player->pbody->body->GetPosition();
		pbody->body->SetTransform(b2Vec2(tf.x, tf.y), 0);
	}

	return ret;
}


void DashParticle::SetDirection(Vector2D dir)
{
	direction = dir;
	direction = direction.normalized();
}

void DashParticle::SetPosition(Vector2D pos)
{
	pbody->body->SetTransform({ PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()) }, 0);
}

void DashParticle::Restart(Vector2D pos, Vector2D dir)
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

bool DashParticle::CleanUp()
{
	return true;
}