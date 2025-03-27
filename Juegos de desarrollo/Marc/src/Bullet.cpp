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
    : Entity(EntityType::SHOT),
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
        texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/bala.png");
    }
   
    texW = parameters.attribute("w").as_int();
    texH = parameters.attribute("h").as_int();


    pbody = Engine::GetInstance().physics.get()->CreateRectangle(static_cast<int>(position.getX()), static_cast<int>(position.getY()), 16, 12, bodyType::DYNAMIC);
    if (pbody == nullptr) {
        LOG("Error: PhysBody creation failed!");
        return false;
    }
    pbody->listener = this;
    pbody->body->SetGravityScale(0);
    pbody->body->SetFixedRotation(true);
    // Establecer tipo de colisión
    pbody->ctype = ColliderType::SHOT;
    
    active = true;

    return true;
}

bool Bullet::Update(float dt) {
    if (pbody == nullptr) {
        LOG("Error: PhysBody creation failed!");
        return false;
    }

    b2Vec2 velocity = pbody->body->GetLinearVelocity();
    velocity.x = direction.getX() * 1.5f;  // Velocidad constante en la dirección horizontal
    velocity.y = 0.0f;  // Sin movimiento vertical
    pbody->body->SetLinearVelocity(velocity);
    b2Transform pbodyPos = pbody->body->GetTransform();
    position.setX(static_cast<float>(METERS_TO_PIXELS(pbodyPos.p.x)) - 12.0f);
    position.setY(static_cast<float>(METERS_TO_PIXELS(pbodyPos.p.y)) - 8.0f);

    if (direction.getX() < 0) {
        Engine::GetInstance().render.get()->DrawTexture(texture, static_cast<int>(position.getX()), static_cast<int>(position.getY()), nullptr, SDL_FLIP_HORIZONTAL);
    }
    else {
        Engine::GetInstance().render.get()->DrawTexture(texture, static_cast<int>(position.getX()), static_cast<int>(position.getY()));
    }

    return true;
}

bool Bullet::CleanUp() {
    //if (pbody != nullptr) {
    //    Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
    //    pbody = nullptr;  // Para asegurarte de que no haya referencias colgando
    //}
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
        LOG("Collided - DESTROY");
        break;
    }
}

//void Bullet::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
//    switch (physB->ctype) {
//    case ColliderType::PLATFORM:
//    case ColliderType::HAZARD:
//    case ColliderType::CHECKPOINT:
//    case ColliderType::ITEM:
//    case ColliderType::ENEMYBULLET:
//    case ColliderType::BULLET:
//        LOG("Collided with hazard - DESTROY");
//        break;
//    }
//}
