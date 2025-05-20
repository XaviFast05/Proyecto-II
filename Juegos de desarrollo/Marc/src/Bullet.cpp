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

Bullet::Bullet(BulletType bulletType, BulletDirection bullet_direction)
    : Entity(EntityType::SHOT),
    bullet_type(bulletType),
    bullet_direction(bullet_direction),
    direction(1, 0),
    pbody(nullptr),
    texW(0),
    texH(0),
    texture(nullptr),
    texturePath("")
{
    name = "bullet";
}

Bullet::~Bullet() {}

bool Bullet::Awake() {
    return true;
}

bool Bullet::Start() {
    
    if (bullet_type == BulletType::PICKAXE) {
        texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/bala.png");
    }
	/*else if (bullet_type == BulletType::FINALBOSS) {
		texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/bala.png");
	}*/
	else if (bullet_type == BulletType::BOSSJUMP) {
		texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/balaJumper.png");
	}
    

    SetParameters(Engine::GetInstance().scene.get()->configParameters);
    texW = parameters.attribute("w").as_int();
    texH = parameters.attribute("h").as_int();

    if (!pbody) {
        pbody = Engine::GetInstance().physics.get()->CreateRectangle(
            static_cast<int>(position.getX()), static_cast<int>(position.getY()),
            48, 12, bodyType::DYNAMIC
        );
    }

    if (pbody == nullptr) {
        LOG("Error: PhysBody creation failed!");
        return false;
    }

    pbody->listener = this;
    pbody->body->SetGravityScale(0);
    pbody->body->SetFixedRotation(true);
    pbody->body->SetType(b2_dynamicBody);
    pbody->body->SetEnabled(true);

    // Establecer el tipo de colisión según el BulletType
    switch (bullet_type) {
    case BulletType::BOSSJUMP:
        pbody->ctype = ColliderType::JUMP;
        break;
    case BulletType::CHILD:
        pbody->ctype = ColliderType::JUMP;
        break;
    case BulletType::PICKAXE:
    default:
        pbody->ctype = ColliderType::PICKAXE;
        break;
    }

    active = true;
    stuckOnWall = false;
    destroyPickaxe = false;

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

    if (!stuckOnWall) {
        b2Vec2 velocity = pbody->body->GetLinearVelocity();
        velocity.x = direction.getX() * (bullet_direction == BulletDirection::HORIZONTAL ? 12.5f : 0);
        velocity.y = direction.getY() * (bullet_direction == BulletDirection::VERTICAL ? -12.5f : 0);
        pbody->body->SetLinearVelocity(velocity);

        b2Transform pbodyPos = pbody->body->GetTransform();
        position.setX(static_cast<float>(METERS_TO_PIXELS(pbodyPos.p.x)) - 32.0f);
        position.setY(static_cast<float>(METERS_TO_PIXELS(pbodyPos.p.y)) - 32.0f);
    }
    else {
        pbody->body->SetType(b2_staticBody);
    }

    if (bullet_direction == BulletDirection::HORIZONTAL) {
        if (direction.getX() < 0) {
            Engine::GetInstance().render.get()->DrawTextureFlipped(texture, static_cast<int>(position.getX()), static_cast<int>(position.getY()));
        }
        else {
            Engine::GetInstance().render.get()->DrawTexture(texture, static_cast<int>(position.getX()), static_cast<int>(position.getY()));
        }
    }
    else {
        Engine::GetInstance().render.get()->DrawTexture(texture, static_cast<int>(position.getX()), static_cast<int>(position.getY()), 0, 1.0f, 270);
    }

    return true;
}

bool Bullet::CleanUp() {
    // Si quieres eliminar pbody manualmente:
    // if (pbody != nullptr) {
    //     Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
    //     pbody = nullptr;
    // }
    return true;
}

void Bullet::SetPosition(Vector2D pos) {
    if (pbody == nullptr) {
        LOG("Error: Cannot set position, pbody is NULL.");
        return;
    }

    b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY() - 0.5f));
    pbody->body->SetTransform(bodyPos, 0);

    position = pos;
}

Vector2D Bullet::GetPosition() {
    b2Vec2 bodyPos = pbody->body->GetTransform().p;
    return Vector2D(static_cast<float>(METERS_TO_PIXELS(bodyPos.x)), static_cast<float>(METERS_TO_PIXELS(bodyPos.y)));
}

void Bullet::ChangeDirection(BulletDirection t) {
    bullet_direction = t;
    pbody->body->SetFixedRotation(false);
    pbody->body->SetTransform(pbody->body->GetTransform().p, bullet_direction == BulletDirection::HORIZONTAL ? 0 : M_PI / 2);
    pbody->body->SetFixedRotation(true);
}

void Bullet::OnCollision(PhysBody* physA, PhysBody* physB) {
    switch (bullet_type)
    {
    case BulletType::CHILD:
        switch (physB->ctype) {
        case ColliderType::PICKAXE:
        case ColliderType::PLATFORM:
        case ColliderType::CLIMBINGWALL:
        case ColliderType::PLAYER:
        case ColliderType::ENEMY:
            LOG("Collided - DESTROY");
            destroyPickaxe = true;
            break;
        }
        break;
    case BulletType::PICKAXE:
        switch (physB->ctype) {
        case ColliderType::PICKAXE:
        case ColliderType::PLATFORM:
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
        break;
    case BulletType::BOSSJUMP:
        switch (physB->ctype) {
        case ColliderType::PICKAXE:
        case ColliderType::PLATFORM:
        case ColliderType::CLIMBINGWALL:
        case ColliderType::PLAYER:
        case ColliderType::ENEMY:
            LOG("Collided - DESTROY");
            destroyPickaxe = true;
            break;
        }
        break;
    default:
        break;
    }
    
    
}
