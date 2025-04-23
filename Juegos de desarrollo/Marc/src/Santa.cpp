#include "Santa.h"
#include "Engine.h"
#include "Textures.h"
#include "Physics.h"
#include "Scene.h"
#include "Player.h"
#include "LOG.h"
#include "Audio.h"
#include "tracy/Tracy.hpp"

Santa::Santa()
{

}

Santa::~Santa() {
	
}

bool Santa::Start() {
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_float());
	position.setY(parameters.attribute("y").as_float());
	texW = parameters.attribute("w").as_float();
	texH = parameters.attribute("h").as_float();
	drawOffsetX = 0;
	drawOffsetY = 0;

	idle.LoadAnimations(parameters.child("animations").child("idle"));
	attack.LoadAnimations(parameters.child("animations").child("attack"));
	hurt.LoadAnimations(parameters.child("animations").child("hurt"));
	death.LoadAnimations(parameters.child("animations").child("death"));
	jump.LoadAnimations(parameters.child("animations").child("jump"));
	fall.LoadAnimations(parameters.child("animations").child("fall"));
	walk.LoadAnimations(parameters.child("animations").child("walk"));

	currentAnimation = &idle;

	//this is meant to be in Enemy.cpp as both enemies use it, but it doesn't work for some reason
	pugi::xml_document audioFile;
	pugi::xml_parse_result result = audioFile.load_file("config.xml");
	audioNode = audioFile.child("config").child("audio").child("fx");

	//SFX LOAD
	noSound = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("noSound").attribute("path").as_string());
	//INIT PHYSICS
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), 16, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetGravityScale(1);
	pbody->body->SetFixedRotation(true);
	pbody->body->SetTransform({ PIXEL_TO_METERS(instanceParameters.child("point").attribute("x").as_float()), PIXEL_TO_METERS(instanceParameters.child("point").attribute("y").as_float())}, 0);
	pbody->listener = this;


	//INIT VARIABLES
	speed = parameters.child("properties").attribute("speed").as_float();
	deathTime = parameters.child("properties").attribute("deathTime").as_float();
	jumpRange = parameters.child("properties").attribute("jumpRange").as_float();
	jumpCooldown = parameters.child("properties").attribute("jumpCooldown").as_float();
	attackRange = parameters.child("properties").attribute("attackRange").as_float();
	attackTime = parameters.child("properties").attribute("attackTime").as_float();
	lives = parameters.child("properties").attribute("lives").as_int();
	hurtTime = parameters.child("properties").attribute("hurtTime").as_int();
	pushForce = parameters.child("properties").attribute("pushForce").as_int();
	state = PATROL;

	jumpCooldownTimer.Start();

	dead = false;

	pugi::xml_document baseFile;
	pugi::xml_parse_result results = baseFile.load_file("config.xml");

	//AUDIO
	punchSFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("punchSound").attribute("path").as_string());
	jumpSFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("santaJumpSound").attribute("path").as_string());
	hurtSFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("santaHurtSound").attribute("path").as_string());

	return true;
}

bool Santa::Update(float dt) {
	
	ZoneScoped;

	
	if (!dead) {

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
			//STATES CHANGERS
			if (!Engine::GetInstance().scene.get()->GetStartBossFight())
			{
				state = PATROL;
			}
			else 
			{
				float deltaX = player->pbody->GetPhysBodyWorldPosition().getX() - pbody->GetPhysBodyWorldPosition().getX();
				float deltaY = player->pbody->GetPhysBodyWorldPosition().getY() - pbody->GetPhysBodyWorldPosition().getY();
				float distanceToPlayer = sqrt(deltaX * deltaX + deltaY * deltaY);
				
				if (state != ATTACK && state != JUMP && state != FALL && state != DEAD && state !=HURT)
				{
					if (distanceToPlayer < attackRange)
					{
						Engine::GetInstance().audio.get()->PlayFx(punchSFX);
						state = ATTACK;
						pbody->body->SetLinearVelocity({ 0,0 });
						attackTimer.Start();
					}
					else if (distanceToPlayer < jumpRange && distanceToPlayer > jumpRange - 10 && jumpCooldownTimer.ReadSec()>5) { // `attackRange` define el rango de ataque
						Engine::GetInstance().audio.get()->PlayFx(jumpSFX);
						state = JUMP;
						jumpCooldownTimer.Start();
						jumped = false;
					}
					else {
						state = CHASING;
					}
				}
			}

			//STATES CONTROLER
			if (state == CHASING) {

				Vector2D direction = {player->pbody->GetPhysBodyWorldPosition().getX()>pbody->GetPhysBodyWorldPosition().getX() ? 1.0f : -1.0f ,0.0f};
				pbody->body->SetLinearVelocity({ direction.getX() * speed, pbody->body->GetLinearVelocity().y});
			}
			else if (state == JUMP) {
				// Lógica para saltar hacia el jugador
				Vector2D direction = {
					player->pbody->GetPhysBodyWorldPosition().getX() > pbody->GetPhysBodyWorldPosition().getX() ? 1.0f : -1.0f,
					-1.0f // Salto hacia arriba
				};

				// Verificar si aterrizó para volver al estado CHASING
				if (VALUE_NEAR_TO_0(pbody->body->GetLinearVelocity().y) && inFloor) {
					if (jumped)
					{
						state = CHASING; // Cambiar a otro estado al aterrizar
					}
					else
					{
						float jumpImpulseX = 0.2f; // Fuerza horizontal
						float jumpImpulseY = 2; // Fuerza vertical
						// Impulso para saltar hacia el jugador
						Vector2D impulse = { direction.getX() * jumpImpulseX, direction.getY() * jumpImpulseY };
						pbody->body->ApplyLinearImpulseToCenter({ impulse.getX(), impulse.getY() }, true);
						jumped = true;
					}
				}
			}
			else if (state == ATTACK) {
				
				if (attackTimer.ReadSec() >= attackTime)
				{
					state = CHASING;
					attack.Reset();
				}
			}
			else if (state == DEAD) {

				if (deathTimer.ReadSec() > deathTime && !dead) {
					pbody->body->SetEnabled(false);
					dead = true;
					Engine::GetInstance().scene.get()->SetBossFightKilled(true);

					LOG("killed boss");
				}
			}
			else if (state == HURT) {

				if (hurtTimer.ReadSec() >= hurtTime) {
					state = CHASING;
					hurt.Reset();
				}
			}
		}
		else {

			pbody->body->SetLinearVelocity({ 0,0 });
		}


		if (pbody->body->GetLinearVelocity().x > 0.2f) {
			dir = RIGHT;
		}
		else if (pbody->body->GetLinearVelocity().x < -0.2f) {
			dir = LEFT;
		}

		switch (state) {
			break;
		case CHASING:
			currentAnimation = &walk;
			break;
		case PATROL:
			currentAnimation = &idle;
			break;
		case ATTACK:
			currentAnimation = &attack;
			break;
		case JUMP:
			if (pbody->body->GetLinearVelocity().y < 0) currentAnimation = &jump;
			else currentAnimation = &fall;
			break;
		case DEAD:
			currentAnimation = &death;
			break;
		case HURT:
			currentAnimation = &hurt;
			break;
		default:
			break;
		}


		//DRAW

		if (pbody->body->IsEnabled()) {
			if (Engine::GetInstance().GetDebug())
			{
				Engine::GetInstance().render.get()->DrawCircle(position.getX() + texW / 2, position.getY() + texH / 2, chaseArea * 2, 255, 255, 255);
				Engine::GetInstance().render.get()->DrawCircle(destinationPoint.getX(), destinationPoint.getY(), 3, 255, 0, 0);
			}

			currentAnimation->Update();

			b2Transform pbodyPos = pbody->body->GetTransform();
			position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2 + drawOffsetX);
			position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2 + drawOffsetY);
			if (dir == LEFT) {
				Engine::GetInstance().render.get()->DrawTextureFlipped(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
			}
			else if (dir == RIGHT) {
				Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
			}
		}

	}



	return true;
}

void Santa::OnCollision(PhysBody* physA, PhysBody* physB) {


	switch (physB->ctype)
	{
	case ColliderType::WEAPON:
		LOG("Enemy was hit by WEAPON");
		if (state != DEAD) {
			DMGEnemy(physA, physB);
		}

		break;
	case ColliderType::PLATFORM:
		LOG("Enemy was hit by WEAPON");
		inFloor = true;

		break;
	case ColliderType::PICKAXE:
		LOG("Enemy was hit by SHOT");
		if (state != DEAD) {
			DMGEnemy(physA, physB);
		}
		break;
	case ColliderType::SPYKE:

		LOG("Collision SPYKE");
		break;

	case ColliderType::ENEMY:
		LOG("Collision ENEMY");
		break;
	case ColliderType::ABYSS:

		LOG("Collision ABYSS");
		break;
		if (state != DEAD) {
			KillEnemy();

		}

	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		if (state == JUMP || state == FALL) {

		}
		break;

	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;

	default:
		break;
	}
}

void Santa::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{

	case ColliderType::PLATFORM:
		LOG("Enemy was hit by WEAPON");
		inFloor = false;

		break;

	default:
		break;
	}
}

void Santa::DMGEnemy(PhysBody* physA, PhysBody* physB)
{
	if (state != DEAD && state != HURT) {

		Engine::GetInstance().audio.get()->PlayFx(hurtSFX);
		lives--;
		if (lives <= 0) {
			state = DEAD;
			deathTimer.Start();
			death.Reset();
		}
		else
		{
			hurtTimer.Start();
			state = HURT;
		}
	}
}


void Santa::KillEnemy() {
	deathTimer.Start();
	death.Reset();
	state = DEAD;
}