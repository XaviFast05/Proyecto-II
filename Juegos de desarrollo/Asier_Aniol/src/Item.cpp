#include "Item.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"

// Constructor actualizado
Item::Item(ItemType iType) : Entity(EntityType::ITEM), itemType(iType)
{
    name = "item";
}

Item::~Item() {}

bool Item::Awake() {
    return true;
}

bool Item::Start() {
    texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
    position.setX(parameters.attribute("x").as_int());
    position.setY(parameters.attribute("y").as_int());
    texW = parameters.attribute("w").as_int();
    texH = parameters.attribute("h").as_int();

    coin.LoadAnimations(parameters.child("animations").child("coin"));
    powerupJump.LoadAnimations(parameters.child("animations").child("powerupJ"));
    powerupSpeed.LoadAnimations(parameters.child("animations").child("powerupS"));
    health.LoadAnimations(parameters.child("animations").child("health"));

    pickCoinFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/coin.ogg");

    // L08 TODO 4: Add a physics to an item - initialize the physics body
    Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);
    pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texH / 8, (int)position.getY() + texH / 4, 32, 32, bodyType::STATIC);
    pbody->listener = this;

    // L08 TODO 7: Assign collider type
    if (itemType == ItemType::COLLECT) {
        currentAnimation = &coin;
        pbody->ctype = ColliderType::ITEM;
    }
    else if (itemType == ItemType::HEALTH) {
        currentAnimation = &health;
        pbody->ctype = ColliderType::HEALTH;
    }
    else if (itemType == ItemType::POWERUPJUMP) {
        currentAnimation = &powerupJump;
        pbody->ctype = ColliderType::POWERUPJUMP;
    }
    else if (itemType == ItemType::POWERUPSPEED) {
        currentAnimation = &powerupSpeed;
        pbody->ctype = ColliderType::POWERUPSPEED;
    }

    return true;
}

bool Item::Update(float dt)
{
    b2Transform pbodyPos = pbody->body->GetTransform();
    position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 8);
    position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 8);

    Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
    currentAnimation->Update();

    return true;
}

bool Item::CleanUp()
{
    if (pbody != nullptr) {
        Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
        Engine::GetInstance().textures.get()->UnLoad(texture);
        pbody = nullptr;  // Para asegurarte de que no haya referencias colgando
    }
    return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
    case ColliderType::PLAYER:
        LOG("Item collided with player");

		if (!isPicked) {
			isPicked = true;
			switch (itemType)
			{
			case ItemType::COLLECT:
                Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
				Engine::GetInstance().scene.get()->player->coinCount++;
				break;
			case ItemType::HEALTH:
                Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
				Engine::GetInstance().scene.get()->player->lives++;
				break;
			case ItemType::POWERUPJUMP:
                Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
				Engine::GetInstance().scene.get()->player->powerUpJumpActive = true;
				break;
			case ItemType::POWERUPSPEED:
				Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
				Engine::GetInstance().scene.get()->player->powerUpSpeedActive = true;
				break;
			}
		}
        Engine::GetInstance().entityManager.get()->DestroyEntity(this);

        break;
	}
}

void Item::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collided with player");
		break;
	}
}
