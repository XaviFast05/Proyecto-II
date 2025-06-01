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

	lives = 20;

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

    // Si disparo pendiente (al aterrizar), lo lanzo aquí
    if (shouldShoot) {
        projectileManager->ThrowJumpProjectiles(pbody->GetPhysBodyWorldPosition());
        shouldShoot = false;
    }

    // Si tengo 3 impactos y no estoy saltando, empiezo el salto
    if (damageAccumulated >= 3 && !isBossJumping) {
        TriggerBossJump();
    }

    // Actualiza animación
    currentAnimation = isBossJumping ? &jump : &walk;
    currentAnimation->Update();

    // Actualiza posición del sprite según Box2D
    b2Transform xf = pbody->body->GetTransform();
    position.setX(METERS_TO_PIXELS(xf.p.x) - texW / 2 + drawOffsetX);
    position.setY(METERS_TO_PIXELS(xf.p.y) - texH / 1.5 + drawOffsetY);

    // Dibuja
    if (pbody->body->IsEnabled() &&
        Engine::GetInstance().render->InCameraView(position.getX(), position.getY(), texW, texH))
    {
        if (bossDirection < 0)
            Engine::GetInstance().render->DrawTextureBuffer(texture,
                (int)position.getX(), (int)position.getY() + 10, false, ENTITIES,
                &currentAnimation->GetCurrentFrame());
        else
            Engine::GetInstance().render->DrawTextureBuffer(texture,
                (int)position.getX(), (int)position.getY() + 10, true, ENTITIES,
                &currentAnimation->GetCurrentFrame());
    }

    return true;
}

void JumpingEnemy::TriggerBossJump() {
    // 1) Defino el destino en px
    float targetX_px = (bossDirection < 0)
        ? bossJumpTargetXLeft
        : bossJumpTargetXRight;

    // 2) Posición actual en px
    Vector2D pos = pbody->GetPhysBodyWorldPosition();
    float currentX_px = pos.getX();

    // 3) Velocidad vertical en m/s
    float vy_mps = PIXEL_TO_METERS(bossJumpSpeedV);

    // 4) Tiempo de vuelo (ida y vuelta)
    float g = 9.8f * pbody->body->GetGravityScale();
    float t_flight = 2.5f * (vy_mps / g);

    // 5) Derivo la vx necesaria para cubrir la distancia dx en ese tiempo
    float dx_px = targetX_px - currentX_px;
    float vx_px_s = dx_px / t_flight;
    float vx_mps = PIXEL_TO_METERS(vx_px_s);

    // 6) Aplico la velocidad inicial
    pbody->body->SetLinearDamping(0.0f);
    pbody->body->SetLinearVelocity({ vx_mps, -vy_mps });
    isBossJumping = true;
}

void JumpingEnemy::OnCollision(PhysBody* physA, PhysBody* physB) {
    bool push = false;

    switch (physB->ctype) {
    case ColliderType::PLATFORM:
        if (isBossJumping) {
            // Al chocar con la plataforma tras el salto:
            isBossJumping = false;
            damageAccumulated = 0;
            shouldShoot = true;           // para disparar en el siguiente Update
            bossDirection *= -1;          // invierto para el próximo salto
        }
        break;

    case ColliderType::PICKAXE:
        if (!dead) {
            DMGEnemyPickaxe();
            damageAccumulated++;
        }
        break;

    case ColliderType::MELEE_AREA:
        if (!dead) {
            if (canPush) push = true;
            DMGEnemyMelee();
            damageAccumulated++;
        }
        break;

    default:
        break;
    }

    if (push) {
        b2Vec2 pushVec = physA->body->GetPosition()
            - player->pbody->body->GetPosition();
        pushVec.Normalize();
        pushVec *= pushForce;
        pbody->body->SetLinearVelocity(b2Vec2_zero);
        pbody->body->ApplyLinearImpulseToCenter(pushVec, true);
        pbody->body->SetLinearDamping(pushFriction);
    }
}