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
	//position.setX(instanceParameters.attribute("x").as_float());
	//position.setY(instanceParameters.attribute("y").as_float());
	texW = sceneNode.child("entities").child("particles").child("environmentParticles").attribute("w").as_int();
	texH = sceneNode.child("entities").child("particles").child("environmentParticles").attribute("h").as_int();
	//areaW = sceneNode.child("entities").child("particles").child("environmentParticles").attribute("areaW").as_int();
	//areaH = sceneNode.child("entities").child("particles").child("environmentParticles").attribute("areaH").as_int();
	rotation = sceneNode.child("entities").child("particles").child("environmentParticles").attribute("rotation").as_float();

	//rad = sceneNode.child("entities").child("particles").child("environmentParticles").attribute("rad").as_int();
	idle.LoadAnimations(sceneNode.child("entities").child("particles").child("environmentParticles").child("animations").child("idle"));
	pbody = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), areaW , areaH, bodyType::STATIC);

	pbody->body->SetGravityScale(0);
	pbody->ctype = ColliderType::PARTICLE;
	pbody->body->SetLinearVelocity({ 0,0 });
	pbody->body->SetEnabled(true);


	lifeTime = 0.3f;


	castTime1 = 1.0f;
	castTime2 = 2.0f;
	castTime3 = 3.0f;
	castTime4 = 4.0f;
	castTime5 = 5.0f;


	lifeTime1 = 6.0f;
	lifeTime2 = 7.0f;
	lifeTime3 = 8.0f;
	lifeTime4 = 9.0f;
	lifeTime5 = 10.0f;

	changeDisplay = false;

	currentAnim = &idle;
	active = true;
	isCasted = false;
	isAlive = false;



	return true;
}

bool EnvironmentParticles::Update(float dt)
{
	bool ret = true;


		if (!isAlive)
		{
			isAlive = true;
			aliveTimer.Start();
			printf("TONTO");
			pbody->body->SetEnabled(true);
			active = true;
			renderable = true;
			Enable();
		}
		else if (isAlive)
		{
			if (!changeDisplay) {
				if (aliveTimer.ReadSec() > castTime1 && aliveTimer.ReadSec() < lifeTime1) {
					Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) + 2 * (areaW / 5) - areaW / 2, METERS_TO_PIXELS(pbody->body->GetPosition().y) - 3 * (areaH / 5) + areaH / 2, false, ENTITIES, &currentAnim->GetCurrentFrame());

					//printf("POS X: %f", pbody->body->GetPosition().x);
					//printf("POS Y: %f", pbody->body->GetPosition().y);
				}

				if (aliveTimer.ReadSec() > castTime2 && aliveTimer.ReadSec() < lifeTime2) {
					Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) + (areaW / 5) - areaW / 2, METERS_TO_PIXELS(pbody->body->GetPosition().y) - 5 * (areaH / 5) + areaH / 2, false, ENTITIES, &currentAnim->GetCurrentFrame());
				}

				if (aliveTimer.ReadSec() > castTime3 && aliveTimer.ReadSec() < lifeTime3) {
					Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) + 4 * (areaW / 5) - areaW / 2, METERS_TO_PIXELS(pbody->body->GetPosition().y) - 4 * (areaH / 5) + areaH / 2, false, ENTITIES, &currentAnim->GetCurrentFrame());


				}

				if (aliveTimer.ReadSec() > castTime4 && aliveTimer.ReadSec() < lifeTime4) {
					Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) - areaW / 2, METERS_TO_PIXELS(pbody->body->GetPosition().y) - 2 * (areaH / 5) + areaH / 2, false, ENTITIES, &currentAnim->GetCurrentFrame());

				}

				if (aliveTimer.ReadSec() > castTime5 && aliveTimer.ReadSec() < lifeTime5) {
					Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) + 3 * (areaW / 5) - areaW / 2, METERS_TO_PIXELS(pbody->body->GetPosition().y) - 1 * (areaH / 5) + areaH / 2, false, ENTITIES, &currentAnim->GetCurrentFrame());

				}
			}
			else if (changeDisplay) {
				if (aliveTimer.ReadSec() > castTime1 && aliveTimer.ReadSec() < lifeTime1) {
					Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) + (areaW / 5) - areaW / 2, METERS_TO_PIXELS(pbody->body->GetPosition().y) - 4 * (areaH / 5) + areaH / 2, false, ENTITIES, &currentAnim->GetCurrentFrame());
					//printf("POS X: %f", pbody->body->GetPosition().x);
					//printf("POS Y: %f", pbody->body->GetPosition().y);
				}

				if (aliveTimer.ReadSec() > castTime2 && aliveTimer.ReadSec() < lifeTime2) {
					Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) + 2 * (areaW / 5) - areaW / 2, METERS_TO_PIXELS(pbody->body->GetPosition().y) - 3 * (areaH / 5) + areaH / 2, false, ENTITIES, &currentAnim->GetCurrentFrame());
				}

				if (aliveTimer.ReadSec() > castTime3 && aliveTimer.ReadSec() < lifeTime3) {
					Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) + 4 * (areaW / 5) - areaW / 2, METERS_TO_PIXELS(pbody->body->GetPosition().y) - 5 * (areaH / 5) + areaH / 2, false, ENTITIES, &currentAnim->GetCurrentFrame());
				}

				if (aliveTimer.ReadSec() > castTime4 && aliveTimer.ReadSec() < lifeTime4) {
					Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) - areaW / 2, METERS_TO_PIXELS(pbody->body->GetPosition().y) - 2 * (areaH / 5) + areaH / 2, false, ENTITIES, &currentAnim->GetCurrentFrame());
				}

				if (aliveTimer.ReadSec() > castTime5 && aliveTimer.ReadSec() < lifeTime5) {
					Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) + 3 * (areaW / 5) - areaW / 2, METERS_TO_PIXELS(pbody->body->GetPosition().y) - 1 * (areaH / 5) + areaH / 2, false, ENTITIES, &currentAnim->GetCurrentFrame());
				}
			}





			currentAnim->Update();

			//if (Engine::GetInstance().scene.get()->player->dir == RIGHT) {
			//	

	

			//}
			//else if (Engine::GetInstance().scene.get()->player->dir == LEFT) {
			//	Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) - texW / 2, METERS_TO_PIXELS(pbody->body->GetPosition().y - texH / 2), true, ENTITIES, &currentAnim->GetCurrentFrame());
			//	Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) - texW / 2 + 7, METERS_TO_PIXELS(pbody->body->GetPosition().y - texH / 2), true, ENTITIES, &currentAnim->GetCurrentFrame());
			//	Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) - texW / 2 + 15, METERS_TO_PIXELS(pbody->body->GetPosition().y - texH / 2), true, ENTITIES, &currentAnim->GetCurrentFrame());
			//	Engine::GetInstance().render.get()->DrawTextureBuffer(texture, METERS_TO_PIXELS(pbody->body->GetPosition().x) - texW / 2 + 22, METERS_TO_PIXELS(pbody->body->GetPosition().y - texH / 2), true, ENTITIES, &currentAnim->GetCurrentFrame());

			//}
			printf("BOOL: %i", isAlive);
			printf("TIMER: %f\n", aliveTimer.ReadSec());

		}
		if (isAlive && aliveTimer.ReadSec() >= lifeTime5)
		{
			//pbody->body->SetEnabled(false);
			isAlive = false;
			changeDisplay = !changeDisplay;
			//aliveTimer.Start();
			//currentAnim->Reset();
			//active = false;
			//renderable = false;
			printf("LOLOLOOLOOL");

		}


		//b2Vec2 tf = Engine::GetInstance().scene.get()->player->pbody->body->GetPosition();
		//pbody->body->SetTransform(b2Vec2(tf.x, tf.y), 0);
	

	return ret;
}
