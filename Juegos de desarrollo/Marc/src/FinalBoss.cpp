#include "FinalBoss.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Physics.h"
#include "Scene.h"
#include "Player.h"
#include "LOG.h"
#include "tracy/Tracy.hpp"

FinalBoss::FinalBoss() {}
FinalBoss::~FinalBoss() {}

bool FinalBoss::Start() {
    texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
    position.setX(parameters.attribute("x").as_float());
    position.setY(parameters.attribute("y").as_float());
    texW = parameters.attribute("w").as_float();
    texH = parameters.attribute("h").as_float();
    chaseArea = parameters.child("properties").attribute("chaseArea").as_float();
    deathTime = parameters.child("properties").attribute("deathTime").as_float();

    pushForce = parameters.child("properties").attribute("pushForce").as_float();
    pushFriction = parameters.child("properties").attribute("pushFriction").as_float();
    lootAmount = parameters.child("properties").attribute("lootAmount").as_float();
    droppedLoot = parameters.child("properties").attribute("droppedLoot").as_bool();

    idle.LoadAnimations(parameters.child("animations").child("idle"));
    walk.LoadAnimations(parameters.child("animations").child("walk"));
    run.LoadAnimations(parameters.child("animations").child("run"));
	jump.LoadAnimations(parameters.child("animations").child("jump"));
    tired.LoadAnimations(parameters.child("animations").child("tired"));
    death.LoadAnimations(parameters.child("animations").child("death"));
    currentAnimation = &idle;

    pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), 60, bodyType::DYNAMIC);
    pbody->ctype = ColliderType::ENEMY;
    pbody->body->SetGravityScale(1.2f);
    pbody->body->SetFixedRotation(true);
    pbody->listener = this;

    pathfinding = new Pathfinding();
    ResetPath();

    reverseCycle = false;

    lives = 10;
    speed = parameters.child("properties").attribute("speed").as_float();
    tiredDuration = 5.0f;
    dir = LEFT;
    goingRight = false;
    completedCycles = 0;
    isTired = false;
    isJumping = false;
    hasJumped = false;
    hasShot = false;

    pugi::xml_document audioFile;
    pugi::xml_parse_result result = audioFile.load_file("config.xml");
    audioNode = audioFile.child("config").child("audio").child("fx");

    return true;
}

bool FinalBoss::Update(float dt) {
    ZoneScoped;
    // Asumiendo que leftTargetX y rightTargetX están definidos y son correctos
    if (position.getX() <= leftTargetX) {
        goingRight = true;  // Si está en la izquierda, debe ir hacia la derecha
    }
    else if (position.getX() >= rightTargetX) {
        goingRight = false; // Si está en la derecha, debe ir hacia la izquierda
    }
    // --- 1) Si ha muerto, mostrar animación de muerte y soltar loot ---
    if (state == DEAD) {
        currentAnimation = &death;
        currentAnimation->Update();

        if (!droppedLoot) {
            DropLoot();
            pbody->ctype = ColliderType::DEADENEMY;
            droppedLoot = true;
        }

        if (deathTimer.ReadSec() > deathTime) {
            pbody->body->SetEnabled(false);
            dead = true;
        }

        b2Transform xf = pbody->body->GetTransform();
        position.setX(METERS_TO_PIXELS(xf.p.x) - texW / 2 + drawOffsetX);
        position.setY(METERS_TO_PIXELS(xf.p.y) - texH / 1.5 + drawOffsetY);

        if (pbody->body->IsEnabled() &&
            Engine::GetInstance().render->InCameraView(position.getX(), position.getY(), texW, texH)) {
            if (dir == LEFT) {
                Engine::GetInstance().render->DrawTexture(texture,
                    (int)position.getX(), (int)position.getY() + 10,
                    &currentAnimation->GetCurrentFrame());
            }
            else {
                Engine::GetInstance().render->DrawTextureFlipped(texture,
                    (int)position.getX(), (int)position.getY() + 10,
                    &currentAnimation->GetCurrentFrame());
            }
        }
        return true;
    }

    Vector2D myPos = pbody->GetPhysBodyWorldPosition();
    Vector2D playerPos = player->pbody->GetPhysBodyWorldPosition();
    float distToPlayer = myPos.distanceEuclidean(playerPos);

    if (distToPlayer <= chaseArea && state != TIRED) {
        state = CHASING;
    }
    else if (distToPlayer > chaseArea && state == CHASING) {
        state = PATROL;
    }

    if (state == TIRED) {
        currentAnimation = &tired;
        currentAnimation->Update();

        if (tiredTimer.ReadSec() >= tiredDuration) {
            state = PATROL;
            completedCycles = 0;
            goingRight = true;
            hasJumped = false;
            hasShot = false;
        }
        pbody->body->SetLinearVelocity({ 0, pbody->body->GetLinearVelocity().y });
    }
    else if (state == PATROL) {
        currentAnimation = &idle;
        currentAnimation->Update();
        pbody->body->SetLinearVelocity({ 0, pbody->body->GetLinearVelocity().y });
    }
    else if (state == CHASING) {
        float currentX = myPos.getX();
        float targetX = goingRight ? rightTargetX : leftTargetX;
        float dx = targetX - currentX;
        float dirX = (dx > 0) ? 1.0f : -1.0f;

        dir = (dirX < 0) ? LEFT : RIGHT;

        // Ejecutar el ciclo normal hasta completar 4 recorridos
        if (completedCycles < 2 && !isJumping && !hasJumped) {
            currentAnimation = &run;
            b2Vec2 vel = pbody->body->GetLinearVelocity();
            vel.x = dirX * speed;
            pbody->body->SetLinearVelocity(vel);

            // Cambiar de dirección al llegar al destino
            if ((goingRight && currentX >= rightTargetX) ||
                (!goingRight && currentX <= leftTargetX)) {
                goingRight = !goingRight;
                completedCycles++;
            }
        }
        // Empezar salto una vez hechos 4 recorridos
        else if (completedCycles >= 2) {
            if (!isJumping && !hasJumped) {
                currentAnimation = &jump;
                b2Vec2 vel = pbody->body->GetLinearVelocity();
                vel.y = -jumpForce;
                vel.x = (goingRight ? 1 : -1) * speed;
                pbody->body->SetLinearVelocity(vel);
                isJumping = true;
                hasJumped = true;
                hasShot = false;
            }
            else if (isJumping && pbody->body->GetLinearVelocity().y == 0) {
                // Aterriza
                isJumping = false;

                if (!hasShot) {
                    projectileManager->ThrowJumpProjectiles(pbody->GetPhysBodyWorldPosition());
                    hasShot = true;
                }

                // Cambiar de dirección y preparar segundo salto o cansancio
                if (!goingRight) {
                    goingRight = true; // ahora va a la derecha
                    hasJumped = false; // listo para segundo salto
                }
                else {
                    // Completó ambos saltos: entrar en estado TIRED
                    state = TIRED;
                    tiredTimer.Start();
                    completedCycles = 0;
                    hasJumped = false;
                    hasShot = false;
                }
            }
        }

        currentAnimation->Update();
    }

    b2Transform xf = pbody->body->GetTransform();
    position.setX(METERS_TO_PIXELS(xf.p.x) - texW / 2 + drawOffsetX);
    position.setY(METERS_TO_PIXELS(xf.p.y) - texH / 1.5 + drawOffsetY);

    if (pbody->body->IsEnabled() &&
        Engine::GetInstance().render->InCameraView(position.getX(), position.getY(), texW, texH)) {
        if (dir == LEFT) {
            Engine::GetInstance().render->DrawTexture(texture,
                (int)position.getX(), (int)position.getY() + 10,
                &currentAnimation->GetCurrentFrame());
        }
        else {
            Engine::GetInstance().render->DrawTextureFlipped(texture,
                (int)position.getX(), (int)position.getY() + 10,
                &currentAnimation->GetCurrentFrame());
        }
    }

    return true;
}

void FinalBoss::OnCollision(PhysBody* physA, PhysBody* physB) {
    bool push = false;

    switch (physB->ctype) {
    case ColliderType::WEAPON:
        break;
    case ColliderType::PICKAXE:
        if (state != DEAD) 	DMGEnemyPickaxe();
        break;
    case ColliderType::MELEE_AREA:
        if (state != DEAD) {
            if (canPush) push = true;
            DMGEnemyMelee();
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
