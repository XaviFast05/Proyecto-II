#include "Boss.h"
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
#include "tracy/Tracy.hpp"

Boss::Boss() : Entity(EntityType::BOSS), pathfinding(nullptr), pbody(nullptr) {}

Boss::~Boss() {
    delete pathfinding;
}

bool Boss::Awake() {
    return true;
}

bool Boss::Start() {
    texture = Engine::GetInstance().textures->Load(parameters.attribute("texture").as_string());
    position.setX(parameters.attribute("x").as_int());
    position.setY(parameters.attribute("y").as_int());
    texW = parameters.attribute("w").as_int();
    texH = parameters.attribute("h").as_int();

    idle.LoadAnimations(parameters.child("animations").child("idle"));
    move.LoadAnimations(parameters.child("animations").child("move"));
    shootD.LoadAnimations(parameters.child("animations").child("shootdown"));
    die.LoadAnimations(parameters.child("animations").child("die"));
    currentAnimation = &idle;

    pbody = Engine::GetInstance().physics->CreateCircle(position.getX() + texH / 4, position.getY() + texH / 4, texH / 4, bodyType::DYNAMIC);
    if (!pbody) {
        LOG("Error al crear el cuerpo físico del enemigo");
        return false;
    }
    pbody->ctype = ColliderType::ENEMY;
    pbody->listener = this;
    if (!parameters.attribute("gravity").as_bool()) {
        pbody->body->SetGravityScale(0);
    }

    shootFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/shoot.wav");
    deathSfx = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/enemy_death.wav");

    pathfinding = new Pathfinding();
    ResetPath();

    pbody->body->SetLinearVelocity(b2Vec2(0, 0));
    bossTimer.Start();
    return true;
}

void Boss::MoveTowardsTargetTile(float dt) {
    if (!pathfinding || pathfinding->pathTiles.size() < 2) {
        return;
    }

    // Si el jugador está a más de 20 tiles de distancia, el jefe se queda quieto
    if (pathfinding->pathTiles.size() > 35) {
        StayIdle(dt);
        return;
    }

    timeSinceLastAction = (health <= 25) ? bossAngryTimer.ReadSec() : bossTimer.ReadSec();

    struct Action {
        float start;
        float end;
        std::function<void(float)> action;
    };

    // Define las acciones dependiendo del estado del jefe
    Action actions[] = {
        {0, 2, [this](float dt) { MoveToPoint(dt, {8630, 200}); }},
        {2, 3, [this](float dt) { StayIdle(dt); }},
        {3, 6, [this](float dt) { ShootMovingToPoint(dt, {9350, 200}); }},
        {6, 9, [this](float dt) { ShootMovingToPoint(dt, {8630, 200}); }},
        {9, 12, [this](float dt) { ShootMovingToPoint(dt, {9350, 200}); }},
        {12, 13, [this](float dt) { StayIdle(dt); }},
        {13, 15, [this](float dt) { MoveToPoint(dt, {8980, 525}); }},
        {15, 18, [this](float dt) { StayIdle(dt); }},
    };

    // Busca y ejecuta la acción correspondiente al tiempo transcurrido
    for (const auto& action : actions) {
        if (timeSinceLastAction >= action.start && timeSinceLastAction < action.end) {
            action.action(dt);
            return;
        }
    }

    // Reinicia el temporizador si se completan todas las acciones
    if (health <= 25) {
        bossAngryTimer.Start();
    }
    else {
        bossTimer.Start();
    }
}


void Boss::NormalBehavior(float dt) {
    movementSpeed = normalSpeed;
    timeSinceLastAction = bossTimer.ReadSec();

    if (timeSinceLastAction < 2) {
        StayIdle(dt);
    }
    else if (timeSinceLastAction < 6) {
        MoveToPoint(dt, { 9350, 200 });
    }
    else if (timeSinceLastAction < 7) {
        StayIdle(dt);
    }
    else if (timeSinceLastAction < 9) {
        ShootMovingToPoint(dt, { 8630, 200 });
    }
    else if (timeSinceLastAction < 10) {
        StayIdle(dt);
    }
    else if (timeSinceLastAction < 13) {
        ShootMovingToPoint(dt, { 9350, 200 });
    }
    else if (timeSinceLastAction < 14) {
        StayIdle(dt);
    }
    else if (timeSinceLastAction < 17) {
        MoveToPoint(dt, { 8980, 530 });
    }
    else {
        bossTimer.Start();
    }
}

void Boss::MoveToPoint(float dt, const Vector2D& target) {
    if (currentAnimation != &shootD) {
        currentAnimation = &move;
    }
    
    Vector2D direction = target - GetPosition();
    direction.Normalize();
    b2Vec2 velocity(PIXEL_TO_METERS(direction.getX() * movementSpeed), PIXEL_TO_METERS(direction.getY() * movementSpeed));
    pbody->body->SetLinearVelocity(velocity);
}

void Boss::StayIdle(float dt) {
    currentAnimation = &idle;
    pbody->body->SetLinearVelocity(b2Vec2(0, 0));
}

void Boss::ShootMovingToPoint(float dt, const Vector2D& target) {
    // Establece la animación de disparo y mueve hacia el objetivo
    currentAnimation = &shootD;
    MoveToPoint(dt, target);

    // Intentar disparar (solo si no está disparando actualmente)
    Shoot();

    // Restablecer el estado cuando la animación de disparo termina
    if (currentAnimation == &shootD && currentAnimation->HasFinished()) {
        isShooting = false;
        shootD.Reset();
    }
}

bool Boss::Update(float dt) {
    if (active) {
        ZoneScoped;
        ResetPath();
        if (!isDying) {
            while (pathfinding->pathTiles.empty()) {
                pathfinding->PropagateAStar(SQUARED);
            }
            if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
                draw = !draw;
            }
            if (draw) {
                pathfinding->DrawPath();
            }

            // Ejecuta el comportamiento de movimiento y disparo
            MoveTowardsTargetTile(dt);

            // Actualiza el sprite solo si cambia la dirección
            float velocityX = pbody->body->GetLinearVelocity().x;
            if ((velocityX < -0.1f && !flipSprite) || (velocityX > 0.1f && flipSprite)) {
                flipSprite = !flipSprite;
                hflip = flipSprite ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            }
        }

        if (health <= 25) {
            movementSpeed = angrySpeed;
        }
        else
        {
            movementSpeed = normalSpeed;
        }

        if (isDying) {
            b2Vec2 currentVelocity = pbody->body->GetLinearVelocity();
            pbody->body->SetLinearVelocity(b2Vec2(0, currentVelocity.y));
        }

        UpdatePositionAndRender();
        return true;
    }
}


void Boss::UpdatePositionAndRender() {
    b2Transform pbodyPos = pbody->body->GetTransform();
    position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
    position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);
    Engine::GetInstance().render->DrawTexture(texture, position.getX(), position.getY(), &currentAnimation->GetCurrentFrame(), hflip);
    currentAnimation->Update();
}

bool Boss::CleanUp() {
    if (pbody != nullptr) {
        Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
    }
    return true;
}

void Boss::SetPosition(Vector2D pos) {
    pos.setX(pos.getX() + texW / 4);
    pos.setY(pos.getY() + texH / 4);
    b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
    if (pbody != nullptr) {
        pbody->body->SetTransform(bodyPos, 0);
    }
}

Vector2D Boss::GetPosition() {
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

void Boss::Shoot() {
    // Solo dispara si no está disparando actualmente
    if (!isShooting) {
        currentAnimation = &shootD;

        // Crear la bala
        Vector2D bulletPosition = GetPosition();
        bulletPosition.setY(bulletPosition.getY() + 28);
        Bullet* bullet = new Bullet(BulletType::BOSSV);
        bullet->SetDirection(Vector2D(0, -1));
        bullet->SetParameters(Engine::GetInstance().scene.get()->configParameters);
        bullet->texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/enemies/bossBullet.png");
        Engine::GetInstance().entityManager.get()->AddEntity(bullet);
        bullet->Start();
        bullet->SetPosition(bulletPosition);

        // Reproducir efecto de sonido
        Engine::GetInstance().audio.get()->PlayFx(shootFxId);

        // Marcar que el jefe está disparando
        isShooting = true;
    }
}


void Boss::ResetPath() {
    Vector2D pos = GetPosition();
    Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
    if (pathfinding != nullptr) {
        pathfinding->ResetPath(tilePos);
    }
}

void Boss::OnCollision(PhysBody* physA, PhysBody* physB) {
    switch (physB->ctype) {
    case ColliderType::BULLET:
        if (!isDying) {
			if (health > 0) {
				health--;

			}
			if (health <= 0) {
				LOG("Collided with bullet - START DYING");
				Engine::GetInstance().audio.get()->PlayFx(deathSfx);
				isDying = true;
				currentAnimation = &die;
				pbody->body->SetGravityScale(1);
			}
        }
		if (isDying && currentAnimation->HasFinished()) {
			LOG("FINISHED - DELETE BOSS");
			Engine::GetInstance().audio.get()->PlayFx(deathSfx);
			Engine::GetInstance().entityManager.get()->DestroyEntity(this);
			isDying = true;
			die.Reset();
		}
		Engine::GetInstance().entityManager.get()->DestroyEntity(physB->listener);
        break;
    case ColliderType::VOID:
        LOG("Collided with hazard - DESTROY");
        Engine::GetInstance().audio.get()->PlayFx(deathSfx);
        Engine::GetInstance().entityManager.get()->DestroyEntity(this);
        break;
    }
}


void Boss::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype) {
	case ColliderType::BULLET:
		LOG("Collision hazard");
		break;
	}
}

// Aqui A VECES falla, es lo que no entendemos
int Boss::GetLevel()
{
    if (!isDying) {
        level = parameters.attribute("level").as_int();
    }
    else {
        level = -1;
    }
    return level;
}

void Boss::SetActive(bool var)
{
	active = var;
}

void Boss::SetAlive() {
	isalive = true;
	parameters.attribute("alive").set_value(true);
}

void Boss::SetDead() {
	isalive = false;
	parameters.attribute("alive").set_value(false);
}
