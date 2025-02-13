#include "EnemyFloor.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"
#include "EntityManager.h"
#include "Player.h"
#include "tracy/Tracy.hpp"

EnemyFloor::EnemyFloor() : Entity(EntityType::ENEMY), pathfinding(nullptr), pbody(nullptr), isFalling(false) {}

EnemyFloor::~EnemyFloor() {
    if (pathfinding != nullptr) {
        delete pathfinding;
    }
}

bool EnemyFloor::Awake() {
    return true;
}

// configurar al enemyfloor
bool EnemyFloor::Start() {
    texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
    position.setX(parameters.attribute("x").as_int());
    position.setY(parameters.attribute("y").as_int());
    texW = parameters.attribute("w").as_int();
    texH = parameters.attribute("h").as_int();

    idle.LoadAnimations(parameters.child("animations").child("idle"));
    move.LoadAnimations(parameters.child("animations").child("move"));
    die.LoadAnimations(parameters.child("animations").child("die"));
    fall.LoadAnimations(parameters.child("animations").child("fall"));

    currentAnimation = &idle;
    pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 4, (int)position.getY() + texH / 4, texH / 4, bodyType::DYNAMIC);
    if (pbody == nullptr) {
        LOG("Error al crear el cuerpo físico del enemigo");
        return false;
    }
    pbody->ctype = ColliderType::ENEMY;
    pbody->listener = this;
    if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);
    deathSfx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/enemy_death.wav");
    pbody->body->SetLinearVelocity(b2Vec2(0, 0));
    pathfinding = new Pathfinding();
    ResetPath();
    pbody->body->SetGravityScale(2);
    isalive = true;
    return true;
}

void EnemyFloor::MoveTowardsTargetTile(float dt) {
    if (pathfinding == nullptr || pathfinding->pathTiles.size() < 2) return;
    
	//si el player está a más de 20 tiles de distancia, el enemigo se queda quieto
    if (pathfinding->pathTiles.size() > 20) {
        pbody->body->SetLinearVelocity(b2Vec2(0, 0));
        currentAnimation = &idle;
        return;
    }
    else {
        currentAnimation = &move;
    }

    pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);

    // coger el penúltimo tile
    auto it = pathfinding->pathTiles.end();
    Vector2D targetTile = *(--(--it));
    Vector2D targetWorldPos = Engine::GetInstance().map.get()->MapToWorld(targetTile.getX(), targetTile.getY());
    Vector2D enemyPos = GetPosition();
    Vector2D direction = targetWorldPos + Vector2D(16, 16) - enemyPos;
    float distance = direction.Length();
    Vector2D velocity(0, 0);

    if (distance > 0.5f) {
        direction.Normalize();
        velocity = direction * 100.0f;
        velocity.setY(0);
    }
    b2Vec2 velocityVec = b2Vec2(PIXEL_TO_METERS(velocity.getX()), 0);
    pbody->body->SetLinearVelocity(velocityVec);
}

void EnemyFloor::SetAlive() {
    isalive = true;
    parameters.attribute("alive").set_value(true);
}

void EnemyFloor::SetDead() {
    isalive = false;
    parameters.attribute("alive").set_value(false);
}

bool EnemyFloor::Update(float dt) {
    if (active) {
        ZoneScoped;
        ResetPath();
        if (!isDying) {
            while (pathfinding->pathTiles.empty()) {
                pathfinding->PropagateAStar(SQUARED);
            }
            if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
                draw = !draw;
            }
            if (draw) {
                pathfinding->DrawPath();
            }
            if (isOnFloor && !isFalling) {
                MoveTowardsTargetTile(dt);
            }
            float velocityX = pbody->body->GetLinearVelocity().x;
            float velocityY = pbody->body->GetLinearVelocity().y;
            if (velocityX < -0.1f) {
                flipSprite = true;
                hflip = SDL_FLIP_HORIZONTAL;
            }
            else if (velocityX > 0.1f) {
                flipSprite = false;
                hflip = SDL_FLIP_NONE;
            }
            if (velocityY > 0.1f) {
                isFalling = true;
            }
            else {
                isFalling = false;
            }

            if (velocityX == 0.0f) {
                currentAnimation = &idle;
            }
        }

        if (isDying) {
            b2Vec2 currentVelocity = pbody->body->GetLinearVelocity();
            pbody->body->SetLinearVelocity(b2Vec2(0, currentVelocity.y));
        }

        b2Transform pbodyPos = pbody->body->GetTransform();
        position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
        position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);
        Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame(), hflip);
        currentAnimation->Update();
        return true;
    }
}

bool EnemyFloor::CleanUp() {
    if (pbody != nullptr) {
        Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
    }
    return true;
}

// Aqui A VECES falla, es lo que no entendemos
int EnemyFloor::GetLevel()
{
    if (!isDying) {
        level = parameters.attribute("level").as_int();
    }
    else
    {
        return -1;
    }
    return level;
}

void EnemyFloor::SetActive(bool var)
{
    active = var;
}

void EnemyFloor::CreateEnemyAtPosition(Vector2D position) {
    Enemy* newEnemy = new Enemy();
    newEnemy->SetPosition(position);
    Engine::GetInstance().entityManager.get()->AddEntity(newEnemy);
}

void EnemyFloor::SetPosition(Vector2D pos) {
    pos.setX(pos.getX() + texW / 2);
    pos.setY(pos.getY() + texH / 2);
    b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
    if (pbody != nullptr) {
        pbody->body->SetTransform(bodyPos, 0);
    }
}

Vector2D EnemyFloor::GetPosition() {
    if (!isalive) {
        return Vector2D(0, 0);
    }
    if (pbody != nullptr) {
        b2Vec2 bodyPos = pbody->body->GetTransform().p;
        Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
        return pos;
    }
    return Vector2D(0, 0);
}

void EnemyFloor::ResetPath() {
    Vector2D pos = GetPosition();
    Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
    if (pathfinding != nullptr) {
        pathfinding->ResetPath(tilePos);
    }
}

void EnemyFloor::OnCollision(PhysBody* physA, PhysBody* physB) {
    switch (physB->ctype) {
    case ColliderType::BULLET:
        if (!isDying) {
            LOG("Collided with bullet - START DYING");
            Engine::GetInstance().audio.get()->PlayFx(deathSfx);
            isDying = true;
            currentAnimation = &die;
        }
        if (isDying && currentAnimation->HasFinished()) {
            LOG("FINISHED - DELETE ENEMY");
            Engine::GetInstance().audio.get()->PlayFx(deathSfx);
            Engine::GetInstance().entityManager.get()->DestroyEntity(this);
            isDying = true;
            die.Reset();
        }
        Engine::GetInstance().entityManager.get()->DestroyEntity(physB->listener);
        break;
    case ColliderType::ENEMYBULLET:
        Engine::GetInstance().entityManager.get()->DestroyEntity(physB->listener);
        break;
    case ColliderType::VOID:
        LOG("Collided with hazard - DESTROY");
		isDying = true;
        Engine::GetInstance().audio.get()->PlayFx(deathSfx);
        Engine::GetInstance().entityManager.get()->DestroyEntity(this);
        break;
    case ColliderType::PLATFORM:
        isOnFloor = true;
        isFalling = false;
        break;
    case ColliderType::HAZARD:
        isOnFloor = true;
        isFalling = false;
        break;
    }
}

void EnemyFloor::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
    switch (physB->ctype) {
    case ColliderType::BULLET:
        LOG("Collision hazard");
        break;
    case ColliderType::PLATFORM:
        if (!isDying) {
            isOnFloor = false;
            currentAnimation = &fall;
            isFalling = true;
        }
        break;
    case ColliderType::HAZARD:
        if (!isDying) {
            isOnFloor = false;
            currentAnimation = &fall;
            isFalling = true;
        }
        break;
    }
}
