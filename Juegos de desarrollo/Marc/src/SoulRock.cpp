﻿#include "SoulRock.h"
#include "SoulRockParticle.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Physics.h"
#include "Scene.h"
#include "Player.h"
#include "Log.h"
#include "Input.h"
#include "Render.h"
#include "Map.h"
#include "EntityManager.h"
#include "CurrencyManager.h"
#include "CurrencyOrb.h"
#include "tracy/Tracy.hpp"


SoulRock::SoulRock() : Entity(EntityType::SOUL_ROCK)
{
	name = "item";
}

SoulRock::~SoulRock() {

}

bool SoulRock::Awake() {
	return true;
}

bool SoulRock::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.child("properties").attribute("texture").as_string());

	position.setX(instanceParameters.attribute("x").as_float());
	position.setY(instanceParameters.attribute("y").as_float()); // player->position.getY() - 44 in xml
	
	texW = parameters.child("properties").attribute("w").as_float();
	texH = parameters.child("properties").attribute("h").as_float();

	flipSprite = instanceParameters.attribute("flipped").as_bool();
	//drawOffsetX = 0;
	//drawOffsetY = 0;

	//INIT ANIMS
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	fractured.LoadAnimations(parameters.child("animations").child("fractured"));
	broken.LoadAnimations(parameters.child("animations").child("broken"));

	currentAnimation = &idle;

	//INIT PHYSICS
	pbody = Engine::GetInstance().physics.get()->CreateCircleSensor((int)position.getX(), (int)position.getY(), texW/2, bodyType::STATIC);
	pbody->ctype = ColliderType::SOUL_ROCK;
	pbody->listener = this;

	//INIT VARIABLES
	if (flipSprite) dir = RIGHT;
	else dir = LEFT;
	state = IDLE;
	
	//INIT CURRENCY MANAGEMENT
	currencyManager = new CurrencyManager();
	currencyManager->Start();
	amount = 2;


	//LOAD SFX
	pugi::xml_document audioFile;
	pugi::xml_parse_result result = audioFile.load_file("config.xml");
	audioNode = audioFile.child("config").child("audio").child("fx");

	hitSFX = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("swordSFX").attribute("path").as_string());
	brokenSFX = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("skeletonDeathSFX").attribute("path").as_string());

	diffPosX = 24.0f;
	diffPosY = 24.0f;

	return true;
}

bool SoulRock::Update(float dt) {

	ZoneScoped;

	if (!Engine::GetInstance().render.get()->InCameraView(pbody->GetPosition().getX() - texW, pbody->GetPosition().getY() - texH, texW, texH))
	{
		pbody->body->SetEnabled(false);
		return true;
	}
	else
	{
		pbody->body->SetEnabled(true);
	}

	if (!Engine::GetInstance().scene.get()->paused) {

		ParticleUse();

		//STATES CONTROLER
		BrakeSystem();

		switch (state) {
			break;
		case IDLE:
			currentAnimation = &idle;
			break;
		case FRACTURED:
			currentAnimation = &fractured;
			break;
		case BROKEN:
			currentAnimation = &broken;
			break;
		default:
			break;
		}

		//DRAW
		if (pbody->body->IsEnabled()) {

			currentAnimation->Update();

			b2Transform pbodyPos = pbody->body->GetTransform();
			position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2); // + drawOffsetX
			position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 1.5); // + drawOffsetY



			if (dir == LEFT) {
				Engine::GetInstance().render.get()->DrawTextureBuffer(texture, (int)position.getX(), (int)position.getY(), false, ENTITIES, &currentAnimation->GetCurrentFrame());
			}
			else if (dir == RIGHT) {
				Engine::GetInstance().render.get()->DrawTextureBuffer(texture, (int)position.getX(), (int)position.getY(),true, ENTITIES ,&currentAnimation->GetCurrentFrame());
			}
		}

	}

	return true;
}

bool SoulRock::CleanUp()
{
	return true;
}

void SoulRock::OnCollision(PhysBody* physA, PhysBody* physB) {


	if (physB->ctype != ColliderType::SHOT &&
		physB->ctype != ColliderType::MELEE_AREA) return;


	b2Vec2 hitPosMeters = physB->body->GetTransform().p;
	collidePos.setX(METERS_TO_PIXELS(hitPosMeters.x));
	collidePos.setY(METERS_TO_PIXELS(hitPosMeters.y));
		

	hit = true;     
	pendingParticles = true;     

}

void SoulRock::BrakeSystem() {


	if (hit) {
		hits--;


		if (!droppedLoot) {
			if (state == IDLE) {
				amount = 2;
				DropLoot();
				droppedLoot = true;
				dropTimer.Start();

			}
			else if (state == FRACTURED) {
				amount = 3;
				DropLoot();
				droppedLoot = true;
				dropTimer.Start();
			}
		}

		if (hits == 1) state = FRACTURED;
		else if (hits <= 0) state = BROKEN;

		hit = false;
	}


	if (dropTimer.ReadSec() > dropTime) {
		droppedLoot = false;
	}



}

void SoulRock::DropLoot() {
	
	std::vector <int> sizes;

	switch (amount) {
	case 0:
		amount = 0;
		break;
	case 1:
		amount = rand() % 2 + 1;
		sizes = { 2,1,1,1,1,1,1,1,1,1 };
		break;
	case 2:
		amount = rand() % 2 + 2;
		sizes = { 2,2,2,1,1,1,1,1,1,1 };
		break;
	case 3:
		amount = rand() % 3 + 3;
		sizes = { 3,2,2,2,2,2,1,1,1,1 };
		break;
	case 4:
		amount = rand() % 3 + 5;
		sizes = { 3,3,2,2,2,2,2,1,1,1 };
		break;
	case 5:
		amount = rand() % 3 + 7;
		sizes = { 3,3,3,2,2,2,2,2,2,1 };
		break;
	default:
		break;
	}
	if (amount > 0) {
		for (int i = 0; i < amount; i++) {
			int num = rand() % 9;
			currencyManager->EnableOrb(pbody->body->GetPosition().x, pbody->body->GetPosition().y, sizes[num]);
		}
	}
}

void SoulRock::ParticleUse() {

	if (pendingParticles)
	{
		particle1 = (SoulRockParticle*)(Engine::GetInstance().entityManager->CreatePooledEntities(EntityType::SOULROCK_PARTICLE));
		particle2 = (SoulRockParticle*)(Engine::GetInstance().entityManager->CreatePooledEntities(EntityType::SOULROCK_PARTICLE));

		particle1->SetParameters(Engine::GetInstance().scene->configParameters);
		particle2->SetParameters(Engine::GetInstance().scene->configParameters);

		particle1->Start();
		particle2->Start();

		Vector2D p1(METERS_TO_PIXELS(pbody->body->GetPosition().x) + diffPosX, METERS_TO_PIXELS(pbody->body->GetPosition().y) + diffPosY);
		Vector2D p2(METERS_TO_PIXELS(pbody->body->GetPosition().x) - diffPosX, METERS_TO_PIXELS(pbody->body->GetPosition().y) - diffPosY);
		Vector2D p3(METERS_TO_PIXELS(pbody->body->GetPosition().x) - diffPosX, METERS_TO_PIXELS(pbody->body->GetPosition().y) + diffPosY);
		Vector2D p4(METERS_TO_PIXELS(pbody->body->GetPosition().x) + diffPosX, METERS_TO_PIXELS(pbody->body->GetPosition().y) - diffPosY);
		Vector2D p5(METERS_TO_PIXELS(pbody->body->GetPosition().x), METERS_TO_PIXELS(pbody->body->GetPosition().y));

		if (state == IDLE)
		{

			if (collidePos.getX() < METERS_TO_PIXELS(pbody->body->GetPosition().x))
			{
				particle1->SetPosition(p1); particle2->SetPosition(p2);

			}
			else
			{
				particle1->SetPosition(p3); particle2->SetPosition(p4);
			}
		}
		else if (FRACTURED)
		{
			particle1->SetPosition(p5);
			particle2->SetPosition(p5);
		}

		pendingParticles = false;
	}
}