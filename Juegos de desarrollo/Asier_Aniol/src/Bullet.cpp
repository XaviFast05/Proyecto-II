// Bullet.cpp
#include "Bullet.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"
#include "EntityManager.h"
#include "Player.h"

Bullet::Bullet(BulletType type)
    : Entity(EntityType::BULLET),
    direction(1, 0),
    pbody(nullptr),
    texW(0),
    texH(0),
    texture(nullptr),
    texturePath(""),
    type(type)
{
    name = "bullet";
}


Bullet::~Bullet() {}

bool Bullet::Awake() {
    return true;
}

bool Bullet::Start() {
    // Inicializar texturas
    if (BulletType::HORIZONTAL == type) {
        texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/player/bullet2.png");
    }
	else if (BulletType::BOSSH == type) {
		texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/enemies/bossBullet.png");
	}
    else if (BulletType::VERTICAL == type) {
        texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/player/bullet3.png");
    }
   
    position.setX(static_cast<float>(parameters.attribute("x").as_int()));  // Conversión a float
    position.setY(static_cast<float>(parameters.attribute("y").as_int()));  // Conversión a float
    texW = parameters.attribute("w").as_int();
    texH = parameters.attribute("h").as_int();

    // Cargar animaciones
    travel.LoadAnimations(parameters.child("animations").child("travel"));
    currentAnimation = &travel;

    pbody = Engine::GetInstance().physics.get()->CreateRectangle(static_cast<int>(position.getX()), static_cast<int>(position.getY()), 22, 10, bodyType::DYNAMIC);
    if (pbody == nullptr) {
        LOG("Error: PhysBody creation failed!");
        return false;
    }
    pbody->listener = this;
    pbody->body->SetGravityScale(0);

    // Establecer tipo de colisión
    if (BulletType::BOSSH == type || BulletType::BOSSV == type) {
        pbody->ctype = ColliderType::ENEMYBULLET;
    }
    else
    {
        pbody->ctype = ColliderType::BULLET;
    }
    

    return true;
}

bool Bullet::Update(float dt) {
    if (pbody == nullptr) {
        LOG("Error: PhysBody creation failed!");
        return false;
    }

    b2Vec2 velocity = pbody->body->GetLinearVelocity();
    if (type == BulletType::HORIZONTAL || type == BulletType::BOSSH) {
        velocity.x = direction.getX() * 7.5f;  // Velocidad constante en la dirección horizontal
        velocity.y = 0.0f;  // Sin movimiento vertical
    }
    else if (type == BulletType::VERTICAL) {
        velocity.x = 0.0f;  // Sin movimiento horizontal
        velocity.y = direction.getY() * 9.0f;  // Velocidad constante en la dirección vertical
    }
    else if (type == BulletType::BOSSV) {
        velocity.x = 0.0f;  // Sin movimiento horizontal
        velocity.y = direction.getY() * -9.0f;  // Velocidad constante en la dirección vertical
    }
    pbody->body->SetLinearVelocity(velocity);

    b2Transform pbodyPos = pbody->body->GetTransform();
    position.setX(static_cast<float>(METERS_TO_PIXELS(pbodyPos.p.x)) - 12.0f);
    position.setY(static_cast<float>(METERS_TO_PIXELS(pbodyPos.p.y)) - 12.0f);

    if (direction.getX() < 0) {
        Engine::GetInstance().render.get()->DrawTexture(texture, static_cast<int>(position.getX()), static_cast<int>(position.getY()), nullptr, SDL_FLIP_HORIZONTAL);
    }
    else {
        Engine::GetInstance().render.get()->DrawTexture(texture, static_cast<int>(position.getX()), static_cast<int>(position.getY()));
    }
    currentAnimation->Update();

    return true;
}

bool Bullet::CleanUp() {
    if (pbody != nullptr) {
        Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
        pbody = nullptr;  // Para asegurarte de que no haya referencias colgando
    }
    return true;
}

void Bullet::SetPosition(Vector2D pos) {
    if (pbody == nullptr) {
        LOG("Error: Cannot set position, pbody is NULL.");
        return; // Salir si el pbody es NULL
    }

    b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
    pbody->body->SetTransform(bodyPos, 0);  // Establecer la nueva posición física

    position = pos;  // Actualizar la posición en pantalla
}

Vector2D Bullet::GetPosition() {
    b2Vec2 bodyPos = pbody->body->GetTransform().p;
    Vector2D pos = Vector2D(static_cast<float>(METERS_TO_PIXELS(bodyPos.x)), static_cast<float>(METERS_TO_PIXELS(bodyPos.y)));  // Conversión a float
    return pos;
}

void Bullet::OnCollision(PhysBody* physA, PhysBody* physB) {
    switch (physB->ctype) {
    case ColliderType::PLATFORM:
	case ColliderType::WALL:
    case ColliderType::HAZARD:
    case ColliderType::CHECKPOINT:
    case ColliderType::ITEM:
    case ColliderType::HEALTH:
    case ColliderType::POWERUPSPEED:
    case ColliderType::POWERUPJUMP:
        LOG("Collided - DESTROY");
        isAlive = false;
        Engine::GetInstance().entityManager.get()->DestroyEntity(this);
        break;
    case ColliderType::ENEMYBULLET:
        if (type == BulletType::HORIZONTAL || type == BulletType::VERTICAL) {
            isAlive = false;
            Engine::GetInstance().entityManager.get()->DestroyEntity(this);
        }
        break;
    case ColliderType::BULLET:
        if (type == BulletType::BOSSH || type == BulletType::BOSSV) {
            isAlive = false;
            Engine::GetInstance().entityManager.get()->DestroyEntity(this);
        }
        break;
    }
}

void Bullet::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
    switch (physB->ctype) {
    case ColliderType::PLATFORM:
    case ColliderType::HAZARD:
    case ColliderType::CHECKPOINT:
    case ColliderType::ITEM:
    case ColliderType::ENEMYBULLET:
    case ColliderType::BULLET:
        LOG("Collided with hazard - DESTROY");
        break;
    }
}
