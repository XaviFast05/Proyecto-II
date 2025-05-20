#include "RunningEnemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Physics.h"
#include "Scene.h"
#include "Player.h"
#include "LOG.h"
#include "tracy/Tracy.hpp"

RunningEnemy::RunningEnemy() {}
RunningEnemy::~RunningEnemy() {}

bool RunningEnemy::Start() {
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

    lives = 10;
    speed = parameters.child("properties").attribute("speed").as_float();
    tiredDuration = 5.0f;
    dir = LEFT;
    goingRight = true;
    completedCycles = 0;
    isTired = false;

    pugi::xml_document audioFile;
    pugi::xml_parse_result result = audioFile.load_file("config.xml");
    audioNode = audioFile.child("config").child("audio").child("fx");

    return true;
}

bool RunningEnemy::Update(float dt) {
    ZoneScoped;

    // --- 1) Si ha muerto, mostrar animación de muerte y soltar loot ---
    if (state == DEAD) {
        // Arrancar animación de muerte
        currentAnimation = &death;
        currentAnimation->Update();

        // Soltar loot una sola vez
        if (!droppedLoot) {
            DropLoot();
            pbody->ctype = ColliderType::DEADENEMY;
            droppedLoot = true;
        }

        // Tras transcurrir deathTime, desactivar el cuerpo
        if (deathTimer.ReadSec() > deathTime) {
            pbody->body->SetEnabled(false);
            dead = true;
        }

        // Actualizar posición y renderizar la última fase de la animación
        b2Transform xf = pbody->body->GetTransform();
        position.setX(METERS_TO_PIXELS(xf.p.x) - texW / 2 + drawOffsetX);
        position.setY(METERS_TO_PIXELS(xf.p.y) - texH / 1.5 + drawOffsetY);

        if (pbody->body->IsEnabled() &&
            Engine::GetInstance().render->InCameraView(position.getX(), position.getY(), texW, texH)) {
            if (dir == LEFT) {
                Engine::GetInstance().render->DrawTextureBuffer(texture,
                    (int)position.getX(), (int)position.getY() + 10, false, ENTITIES,
                    &currentAnimation->GetCurrentFrame());
            }
            else {
                Engine::GetInstance().render->DrawTextureBuffer(texture,
                    (int)position.getX(), (int)position.getY() + 10, true, ENTITIES,
                    &currentAnimation->GetCurrentFrame());
            }
        }
        return true;
    }

    // --- 2) Medir distancia al jugador para cambiar PATROL <-> CHASING ---
    Vector2D myPos = pbody->GetPhysBodyWorldPosition();
    Vector2D playerPos = player->pbody->GetPhysBodyWorldPosition();
    float   distToPlayer = myPos.distanceEuclidean(playerPos);

    if (distToPlayer <= chaseArea && state != TIRED) {
        state = CHASING;
    }
    else if (distToPlayer > chaseArea && state == CHASING) {
        state = PATROL;
    }

    // --- 3) Lógica de estado TIRED ---
    if (state == TIRED) {
        currentAnimation = &tired;
        currentAnimation->Update();

        if (tiredTimer.ReadSec() >= tiredDuration) {
            state = PATROL;
            completedCycles = 0;
            goingRight = true;
        }
        // Parar movimiento horizontal
        pbody->body->SetLinearVelocity({ 0, pbody->body->GetLinearVelocity().y });
    }
    // --- 4) Lógica de estado PATROL (idle) ---
    else if (state == PATROL) {
        currentAnimation = &idle;
        currentAnimation->Update();
        pbody->body->SetLinearVelocity({ 0, pbody->body->GetLinearVelocity().y });
    }
    // --- 5) Lógica de estado CHASING (patrulla lateral) ---
    else if (state == CHASING) {
        currentAnimation = &run;
        float currentX = myPos.getX();
        float targetX = goingRight ? rightTargetX : leftTargetX;
        float dx = targetX - currentX;
        float dirX = (dx > 0) ? 1.0f : -1.0f;

        dir = (dirX < 0) ? LEFT : RIGHT;

        b2Vec2 vel = pbody->body->GetLinearVelocity();
        vel.x = dirX * speed;
        pbody->body->SetLinearVelocity(vel);

        // Cuando llegue a un extremo, cambiar dirección y contar ciclo
        if ((goingRight && currentX >= rightTargetX) ||
            (!goingRight && currentX <= leftTargetX)) {
            goingRight = !goingRight;
            completedCycles++;
            if (completedCycles >= 4) {
                state = TIRED;
                tiredTimer.Start();
                vel.x = 0;
                pbody->body->SetLinearVelocity(vel);
            }
        }
        currentAnimation->Update();
    }

    // --- 6) Actualizar posición lógica ---
    b2Transform xf = pbody->body->GetTransform();
    position.setX(METERS_TO_PIXELS(xf.p.x) - texW / 2 + drawOffsetX);
    position.setY(METERS_TO_PIXELS(xf.p.y) - texH / 1.5 + drawOffsetY);

    // --- 7) Renderizado final ---
    if (pbody->body->IsEnabled() &&
        Engine::GetInstance().render->InCameraView(position.getX(), position.getY(), texW, texH)) {
        if (dir == LEFT) {
            Engine::GetInstance().render->DrawTextureBuffer(texture,
                (int)position.getX(), (int)position.getY() + 10, false, ENTITIES,
                &currentAnimation->GetCurrentFrame());
        }
        else {
            Engine::GetInstance().render->DrawTextureBuffer(texture,
                (int)position.getX(), (int)position.getY() + 10, true, ENTITIES,
                &currentAnimation->GetCurrentFrame());
        }
    }

    return true;
}



void RunningEnemy::OnCollision(PhysBody* physA, PhysBody* physB) {
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
