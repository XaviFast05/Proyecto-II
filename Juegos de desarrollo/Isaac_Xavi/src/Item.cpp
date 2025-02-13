#include "Item.h"
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

Item::Item() : Entity(EntityType::ITEM)
{
}

Item::~Item() {}

bool Item::Awake() {
	return true;
}

bool Item::Start() 
{
	//initilize textures
	if (!started)
	{
		position.setX(parameters.attribute("x").as_int());
		position.setY(parameters.attribute("y").as_int());
		type = parameters.attribute("type").as_int();

		std::string texturePath = "Assets/Textures/items/item" + std::to_string(type) + ".png";
		texture = Engine::GetInstance().textures.get()->Load(texturePath.c_str());
	
		// L08 TODO 4: Add a physics to an item - initialize the physics body
		Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);
		pbody = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), texW / 2, texH / 2, bodyType::STATIC);

		// L08 TODO 7: Assign collider type
		pbody->ctype = ColliderType::ITEM;
		pbody->listener = this;
	}

	started = true;
	return true;
}

bool Item::Update(float dt)
{
	// L08 TODO 4: Add a physics to an item - update the position of the object from the physics.  
	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX() - texH / 2, (int)position.getY() - texH / 2);
	return true;
}

bool Item::CleanUp()
{
	Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
	return true;
}

bool Item::DeleteBody()
{
	Engine::GetInstance().physics.get()->DeleteBody(pbody);
	return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		break;
	case ColliderType::PLAYERSENSOR:
		break;
	}
}

void Item::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		break;
	}
}