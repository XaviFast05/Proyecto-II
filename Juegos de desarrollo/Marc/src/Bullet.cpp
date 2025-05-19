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
#include "Scene.h"

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
    texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/bala.png");
   
    SetParameters(Engine::GetInstance().scene.get()->configParameters);
    texW = parameters.attribute("w").as_int();
    texH = parameters.attribute("h").as_int();


    if (!pbody) pbody = Engine::GetInstance().physics.get()->CreateRectangle(static_cast<int>(position.getX()), static_cast<int>(position.getY()), hitboxWidth, hitboxHeight, bodyType::DYNAMIC);
    if (pbody == nullptr) {
        LOG("Error: PhysBody creation failed!");
        return false;
    }
    pbody->listener = this;
    pbody->body->SetGravityScale(0);
    pbody->body->SetFixedRotation(true);
    // Establecer tipo de colisión
    pbody->ctype = ColliderType::PICKAXE;
    pbody->body->SetType(b2_dynamicBody);
    pbody->body->SetEnabled(true);
    
    active = true;
    stuckOnWall = false;
    destroyPickaxe = false;

    fixture = pbody->body->GetFixtureList();
    if (fixture) {
        b2Filter filter = fixture->GetFilterData();
        filter.categoryBits = CATEGORY_PICKAXE;
        filter.maskBits = 0xFFFF & ~CATEGORY_PLAYER;
        fixture->SetFilterData(filter);
    }

    player = Engine::GetInstance().scene.get()->player;

    return true;
}

bool Bullet::Update(float dt) {
    if (pbody == nullptr) {
        LOG("Error: PhysBody creation failed!");
        return false;
    }

    if (destroyPickaxe) {
        pbody->body->SetEnabled(false);
        active = false;
        destroyPickaxe = false;
    }

    if (onPlayer && player->onPickaxe && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) && isActive) {
        inactiveTimer.Start();
        fixture = pbody->body->GetFixtureList();
        if (fixture) {
            b2Filter filter = fixture->GetFilterData();
            filter.categoryBits = CATEGORY_PICKAXE;
            filter.maskBits = 0xFFFF & ~CATEGORY_PLAYER;
            fixture->SetFilterData(filter);
        }
        isActive = false;
    }

    if (!stuckOnWall) {
        b2Vec2 velocity = pbody->body->GetLinearVelocity();
        velocity.x = direction.getX() * (type == BulletType::HORIZONTAL ? 12.5f : 0); 
        velocity.y = direction.getY() * (type == BulletType::VERTICAL ? -12.5f : 0);  
        pbody->body->SetLinearVelocity(velocity);
        b2Transform pbodyPos = pbody->body->GetTransform();
        position.setX(static_cast<float>(METERS_TO_PIXELS(pbodyPos.p.x)) - 32.0f);
        position.setY(static_cast<float>(METERS_TO_PIXELS(pbodyPos.p.y)) - 32.0f);
    }
    else {
        pbody->body->SetType(b2_staticBody);
    }

    if (type == BulletType::HORIZONTAL)
    {
        if (direction.getX() < 0) {
            Engine::GetInstance().render.get()->DrawTextureFlipped(texture, static_cast<int>(position.getX()), static_cast<int>(position.getY()));
        }
        else {
            Engine::GetInstance().render.get()->DrawTexture(texture, static_cast<int>(position.getX()), static_cast<int>(position.getY()));
        }
    }
    else
    {
        Engine::GetInstance().render.get()->DrawTexture(texture, static_cast<int>(position.getX()), static_cast<int>(position.getY()),0,1.0f,270);
    }

    if ((pbody->body->GetPosition().y - 0.5) > (player->pbody->body->GetPosition().y) && isActive) {
        b2Filter filter = fixture->GetFilterData();
        filter.categoryBits = CATEGORY_DEFAULT;
        filter.maskBits = 0xFFFF;
        fixture->SetFilterData(filter);
    }
    else {
        b2Filter filter = fixture->GetFilterData();
        filter.categoryBits = CATEGORY_PICKAXE;
        filter.maskBits = 0xFFFF & ~CATEGORY_PLAYER;
        fixture->SetFilterData(filter);
    }

    if (!isActive) {
        if (inactiveTimer.ReadSec() > inactiveTimerMax) {
            isActive = true;
            b2Filter filter = fixture->GetFilterData();
            filter.categoryBits = CATEGORY_PICKAXE;
            filter.maskBits = 0xFFFF & ~CATEGORY_PLAYER;
            fixture->SetFilterData(filter);
        }
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

    b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY() - 0.5f));
    pbody->body->SetTransform(bodyPos, 0);  // Establecer la nueva posición física

    position = pos;  // Actualizar la posición en pantalla
}

Vector2D Bullet::GetPosition() {
    b2Vec2 bodyPos = pbody->body->GetTransform().p;
    Vector2D pos = Vector2D(static_cast<float>(METERS_TO_PIXELS(bodyPos.x)), static_cast<float>(METERS_TO_PIXELS(bodyPos.y)));  // Conversión a float
    return pos;
}

void Bullet::ChangeType(BulletType t) {
    type = t;
    pbody->body->SetFixedRotation(false);
    pbody->body->SetTransform(pbody->body->GetTransform().p, type == BulletType::HORIZONTAL ? 0 : M_PI/2);
    pbody->body->SetFixedRotation(true);
}

void Bullet::OnCollision(PhysBody* physA, PhysBody* physB) {
    switch (physB->ctype) {
    case ColliderType::PLAYER:
        onPlayer = true;
        break;
    case ColliderType::PLATFORM:
        LOG("Collided - DESTROY");
        destroyPickaxe = true;
        break;
    case ColliderType::PICKAXE:
        LOG("Collided - DESTROY");
        destroyPickaxe = true;
        break;
    case ColliderType::CLIMBINGWALL:
        LOG("Piqueta con don");
        stuckOnWall = true;
        bulletfixedpos = pbody->body->GetTransform().p;
        break;
	case ColliderType::ENEMY:
		LOG("Collided - DESTROY");
		destroyPickaxe = true;
		break;
    }
}

void Bullet::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
    switch (physB->ctype) {
    case ColliderType::PLAYER:
        onPlayer = false;
        break;
    }
}