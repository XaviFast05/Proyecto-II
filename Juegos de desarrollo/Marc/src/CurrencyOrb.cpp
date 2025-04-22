#include "CurrencyOrb.h"
#include "Engine.h"
#include "Textures.h"
#include "Scene.h"

CurrencyOrb::CurrencyOrb() : Entity(EntityType::CURRENCY_ORB)
{

}

bool CurrencyOrb::Start()
{
    texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/Orb.png");

    SetParameters(Engine::GetInstance().scene.get()->configParameters);
    texW = 16;
    texH = 16;

    if (!pbody) pbody = Engine::GetInstance().physics.get()->CreateCircle(static_cast<int>(position.getX()), static_cast<int>(position.getY()), 10, bodyType::DYNAMIC);
    if (pbody == nullptr) {
        LOG("Error: PhysBody creation failed!");
        return false;
    }
    pbody->listener = this;
    pbody->body->SetGravityScale(1);
    pbody->body->SetFixedRotation(true);
    // Establecer tipo de colisión
    pbody->ctype = ColliderType::ORB;
    pbody->body->SetEnabled(true);

    active = true;
    disable = false;

    player = Engine::GetInstance().scene.get()->player;

    velocity = 3;
    smoothingFactor = 0.01f;
    distToStartMoving = 3;
    
    currentDirection = { player->pbody->body->GetPosition().x - pbody->body->GetPosition().x, player->pbody->body->GetPosition().y - pbody->body->GetPosition().y };
    currentDirection = currentDirection.normalized();

	return true;
}

bool CurrencyOrb::Update(float dt)
{
    direction = { player->pbody->body->GetPosition().x - pbody->body->GetPosition().x, player->pbody->body->GetPosition().y - pbody->body->GetPosition().y };
    float distance = direction.magnitude();
    if (distance < distToStartMoving)
    {
        direction = direction.normalized();

        currentDirection = currentDirection.lerp(direction, smoothingFactor * dt);
        currentDirection = currentDirection.normalized();

        
        float velocityFactor = (distToStartMoving - distance) / distToStartMoving;
        LOG("%f", velocityFactor);
        pbody->body->SetLinearVelocity({ currentDirection.getX() * velocity * velocityFactor, currentDirection.getY() * velocity * velocityFactor });
    }
    else
    {
        currentDirection = { player->pbody->body->GetPosition().x - pbody->body->GetPosition().x, player->pbody->body->GetPosition().y - pbody->body->GetPosition().y };
        currentDirection.normalized();
    }
        
    position.setX(pbody->GetPhysBodyWorldPosition().getX() - texW/2);
    position.setY(pbody->GetPhysBodyWorldPosition().getY() - texH/2);
    Engine::GetInstance().render.get()->DrawTexture(texture, static_cast<int>(position.getX()), static_cast<int>(position.getY()));

    if (disable)
    {
        pbody->body->SetEnabled(false);
        Disable();
    }

	return true;
}

bool CurrencyOrb::CleanUp()
{
	return true;
}

void CurrencyOrb::OnCollision(PhysBody* physA, PhysBody* physB) {
    switch (physB->ctype)
    {
    case ColliderType::PLAYER:
        disable = true;
        break;

    case ColliderType::UNKNOWN:
        break;
    }
}

void CurrencyOrb::SetPosition(Vector2D pos) {
    pbody->SetPhysPosition({ pos.getX(), pos.getY() });
}

