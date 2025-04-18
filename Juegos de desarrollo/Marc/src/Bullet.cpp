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

    texW = parameters.attribute("w").as_int();
    texH = parameters.attribute("h").as_int();

    if (type == BulletType::HORIZONTAL) direction = Vector2D(1, 0);
    else if (type == BulletType::VERTICAL) direction = Vector2D(0, -1);

    if (!pbody) pbody = Engine::GetInstance().physics.get()->CreateRectangle(static_cast<int>(position.getX()), static_cast<int>(position.getY()), 48, 12, bodyType::DYNAMIC);
    if (pbody == nullptr) {
        LOG("Error: PhysBody creation failed!");
        return false;
    }
    pbody->listener = this;
    pbody->body->SetGravityScale(0);
    pbody->body->SetFixedRotation(true);
    // Establecer tipo de colisi�n
    pbody->ctype = ColliderType::SHOT;
    active = true;

    return true;
}

bool Bullet::Update(float dt) {
    if (pbody == nullptr) {
        LOG("Error: PhysBody creation failed!");
        return false;
    }

    if (toDisable) {
        pbody->body->SetEnabled(false);
        active = false;
    }

    if (!stuckOnWall) {
        b2Vec2 velocity = pbody->body->GetLinearVelocity();
        if (type == BulletType::HORIZONTAL) {
            velocity.x = direction.getX() * 12.5f;  // Velocidad constante en la direcci�n horizontal
            velocity.y = 0.0f;  // Sin movimiento vertical
        }
        else if (type == BulletType::VERTICAL) {
            velocity.x = 0.0f;  // Sin movimiento horizontal
            velocity.y = direction.getY() * 12.5f;  // Velocidad constante en la direcci�n vertical
        }

        pbody->body->SetLinearVelocity(velocity);
        b2Transform pbodyPos = pbody->body->GetTransform();
        if (type == BulletType::HORIZONTAL) {
            position.setX(static_cast<float>(METERS_TO_PIXELS(pbodyPos.p.x)) - 32.0f);
            position.setY(static_cast<float>(METERS_TO_PIXELS(pbodyPos.p.y)) - 16.0f);
        }
        else if (type == BulletType::VERTICAL) {
            position.setX(static_cast<float>(METERS_TO_PIXELS(pbodyPos.p.x)) - 16.0f);
            position.setY(static_cast<float>(METERS_TO_PIXELS(pbodyPos.p.y)) - 32.0f);
        }
    }
    else {
        pbody->body->SetType(b2_staticBody);
    }

    if (type == BulletType::HORIZONTAL) {
        if (direction.getX() < 0) {
            Engine::GetInstance().render.get()->DrawTextureFlipped(texture, static_cast<int>(position.getX()), static_cast<int>(position.getY()));
        }
        else {
            Engine::GetInstance().render.get()->DrawTexture(texture, static_cast<int>(position.getX()), static_cast<int>(position.getY()));
        }
    }
    else if (type == BulletType::VERTICAL) {
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
    pbody->body->SetTransform(bodyPos, 0);  // Establecer la nueva posici�n f�sica

    position = pos;  // Actualizar la posici�n en pantalla
}

Vector2D Bullet::GetPosition() {
    b2Vec2 bodyPos = pbody->body->GetTransform().p;
    Vector2D pos = Vector2D(static_cast<float>(METERS_TO_PIXELS(bodyPos.x)), static_cast<float>(METERS_TO_PIXELS(bodyPos.y)));  // Conversi�n a float
    return pos;
}

void Bullet::ChangeType(BulletType t) {
    type = t;
    pbody->body->SetFixedRotation(false);
    if (type == BulletType::HORIZONTAL)
        pbody->body->SetTransform(pbody->body->GetTransform().p, 0);
    else
        pbody->body->SetTransform(pbody->body->GetTransform().p, 90);
}

void Bullet::OnCollision(PhysBody* physA, PhysBody* physB) {
    switch (physB->ctype) {
    case ColliderType::SHOT:
    case ColliderType::PLATFORM:
        LOG("Collided - DESTROY");
        toDisable = true;
        break;
    case ColliderType::CLIMBINGWALL:
        LOG("Piqueta con don");
        stuckOnWall = true;
        bulletfixedpos = pbody->body->GetTransform().p;
        break;
    //case ColliderType::MELEE_AREA:
    //    LOG("Collided - DESTROY");
    //    variableMarc = true;
    //    break;
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
