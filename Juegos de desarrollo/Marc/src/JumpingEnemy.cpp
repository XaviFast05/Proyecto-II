#include "JumpingEnemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Physics.h"
#include "Scene.h"
#include "Player.h"
#include "LOG.h"
#include "tracy/Tracy.hpp"

JumpingEnemy::JumpingEnemy()
{

}

JumpingEnemy::~JumpingEnemy() {

}


bool JumpingEnemy::Start() {

	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_float());
	position.setY(parameters.attribute("y").as_float());
	texW = parameters.attribute("w").as_float();
	texH = parameters.attribute("h").as_float();
	drawOffsetX = 0;
	drawOffsetY = 0;

	lives = 40;

	idle.LoadAnimations(parameters.child("animations").child("idle"));
	walk.LoadAnimations(parameters.child("animations").child("walk"));
	jump.LoadAnimations(parameters.child("animations").child("jump")); 
	attack.LoadAnimations(parameters.child("animations").child("attack"));
	death.LoadAnimations(parameters.child("animations").child("death"));

	currentAnimation = &idle;
	attacked = false;

	//INIT ROUTE
	for (int i = 0; i < route.size(); i++)
	{
		route[i] = Engine::GetInstance().map.get()->WorldToWorldCenteredOnTile(route[i].getX(), route[i].getY());
	}
	routeDestinationIndex = 0;
	destinationPoint = route[routeDestinationIndex];


	//INIT PHYSICS
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), 60, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetGravityScale(1.2f);
	pbody->body->SetFixedRotation(true);
	pbody->listener = this;
	pbody->body->SetTransform({ PIXEL_TO_METERS(destinationPoint.getX()), PIXEL_TO_METERS(destinationPoint.getY()) }, 0);

	//INIT PATH
	pathfinding = new Pathfinding();
	ResetPath();


	//INIT VARIABLES
	state = PATROL;
	speed = parameters.child("properties").attribute("speed").as_float();
	chaseArea = parameters.child("properties").attribute("chaseArea").as_float();
	attackArea = parameters.child("properties").attribute("attackArea").as_float();
	jumpForce = parameters.child("properties").attribute("jumpForce").as_float();
	attackTime = parameters.child("properties").attribute("attackTime").as_float();
	deathTime = parameters.child("properties").attribute("deathTime").as_float();

	pushForce = parameters.child("properties").attribute("pushForce").as_float();
	pushFriction = parameters.child("properties").attribute("pushFriction").as_float();
	lootAmount = parameters.child("properties").attribute("lootAmount").as_float();
	droppedLoot = parameters.child("properties").attribute("droppedLoot").as_bool();
	dir = LEFT;

	//LOAD SFX
	pugi::xml_document audioFile;
	pugi::xml_parse_result result = audioFile.load_file("config.xml");
	audioNode = audioFile.child("config").child("audio").child("fx");

	swordSlashSFX = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("swordSFX").attribute("path").as_string());
	skeletonDeathSFX = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("skeletonDeathSFX").attribute("path").as_string());

	return true;
}

bool JumpingEnemy::Update(float dt) {
	ZoneScoped;
	if (dead) return true;

	if (state == DEAD) {

		if (deathTimer.ReadSec() > deathTime) {

			pbody->body->SetEnabled(false);
			dead = true;
		}

	}

	if (shouldShootProjectiles) {
		projectileManager->ThrowJumpProjectiles(pbody->GetPhysBodyWorldPosition());
		shouldShootProjectiles = false;
	}

	// Actualiza animación
	switch (state) {
	case JUMP:   currentAnimation = &jump;  break;
	case ATTACK: currentAnimation = &attack;break;
	case PATROL: currentAnimation = &walk;  break;
	case DEAD:   currentAnimation = &death; break;
	default:
		if (pbody->body->GetLinearVelocity().LengthSquared() == 0)
			currentAnimation = &idle;
		else
			currentAnimation = &walk;
	}
	currentAnimation->Update();

	// Actualiza posición
	b2Transform xf = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(xf.p.x) - texW / 2 + drawOffsetX);
	position.setY(METERS_TO_PIXELS(xf.p.y) - texH / 1.5 + drawOffsetY);

	if (state == DEAD && !droppedLoot) {
		DropLoot();
		pbody->ctype = ColliderType::DEADENEMY;
		droppedLoot = true;
	}


	// Culling + dibujo
	float cx = position.getX();
	float cy = position.getY();
	bool insideCam = Engine::GetInstance().render->InCameraView(cx, cy, texW, texH);
	if (insideCam && pbody->body->IsEnabled()) {
		if (Engine::GetInstance().GetDebug()) {
			Engine::GetInstance().render->DrawCircle(position.getX() + texW / 2,
				position.getY() + texH / 2,
				chaseArea * 2, 255, 255, 255);
		}
		if (dir == LEFT) {
			Engine::GetInstance().render->DrawTexture(texture,
				(int)position.getX(),
				(int)position.getY() + 10,
				&currentAnimation->GetCurrentFrame());
		}
		else {
			Engine::GetInstance().render->DrawTextureFlipped(texture,
				(int)position.getX(),
				(int)position.getY() + 10,
				&currentAnimation->GetCurrentFrame());
		}
	}

	return true;
}


void JumpingEnemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	bool push = false;

	switch (physB->ctype) {
	case ColliderType::PLATFORM:
		if (isBossJumping) { // Detectar plataforma tras haber saltado para detener el salto y reiniciar el proceso
			isBossJumping = false;
			pbody->body->SetLinearVelocity({ 0.0f, 0.0f });
			bossDirection *= -1; // derecha o izquierda

			shouldShootProjectiles = true;

			state = PATROL;
			ResetPath();
			destinationPoint = route[routeDestinationIndex]; //esto cambia de derecha a izquierda el destino del salto
		}
		break;
	case ColliderType::WEAPON:
		break;
	case ColliderType::PICKAXE:
		if (state != DEAD) 	DMGEnemyPickaxe();

		damageAccumulated++; //Logica de recibir daño para saltar
		if (damageAccumulated >= 3 && !isBossJumping) {
			TriggerBossJump();
		}
		break;
	case ColliderType::MELEE_AREA:
		if (state != DEAD) {
			if (canPush) push = true;
			DMGEnemyMelee();

			damageAccumulated++; //Logica de recibir daño para saltar
			if (damageAccumulated >= 3 && !isBossJumping) {
				TriggerBossJump();
			}
		}
		break;

	case ColliderType::SPYKE:
		break;
	case ColliderType::ENEMY:
		break;
	case ColliderType::ABYSS:
		break;
	case ColliderType::UNKNOWN:
		break;
	default:
		break;
	}
	if (push) {
		//PUSHING THE ENEMY WHEN HURT
		b2Vec2 pushVec((physA->body->GetPosition().x - player->pbody->body->GetPosition().x),
			(physA->body->GetPosition().y - player->pbody->body->GetPosition().y));
		pushVec.Normalize();
		pushVec *= pushForce;

		pbody->body->SetLinearVelocity(b2Vec2_zero);
		pbody->body->ApplyLinearImpulseToCenter(pushVec, true);
		pbody->body->SetLinearDamping(pushFriction);
	}
}

//Salto del Boss usando la fórmula de caída libre. No me puedo creer que esté usando la física para hacer esto
void JumpingEnemy::TriggerBossJump()
{
	if (state != DEAD) {
		jumpTargetX = (bossDirection < 0) ? bossJumpTargetXLeft : bossJumpTargetXRight;
		float currentX = pbody->GetPhysBodyWorldPosition().getX();
		float dx = jumpTargetX - currentX;

		float vy_mps = PIXEL_TO_METERS(bossJumpSpeedV);
		float g = 9.8f * pbody->body->GetGravityScale();
		float t_flight = 2.0f * (vy_mps / g);
		float vx_px_s = dx / t_flight;
		float vx_mps = PIXEL_TO_METERS(vx_px_s);

		pbody->body->SetLinearDamping(0.0f);
		pbody->body->SetLinearVelocity({ vx_mps, -vy_mps });

		// 5) Marca que está saltando y cambia animación
		isBossJumping = true;
		damageAccumulated = 0;
		state = JUMP;

		shouldShootProjectiles = true;

	}
}
