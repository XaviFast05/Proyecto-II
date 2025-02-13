#include "Item.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"

Item::Item() : Entity(EntityType::ITEM)
{
	name = "item";
}

Item::~Item() {}

bool Item::Awake() {
	return true;
}

bool Item::Start() {

	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());

	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());

	key.LoadAnimations(parameters.child("animations").child("key"));
	currentAnimation = &key;

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::STATIC);
	pbody->listener = this;

	pbody->ctype = ColliderType::ITEM;


	return true;
}

bool Item::Update(float dt)
{
	if (!isPicked) {
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
		currentAnimation->Update();
	}
	else {
		pbody->body->SetEnabled(false);
	}
	
	return true;
}

bool Item::CleanUp()
{
	LOG("Cleanup item");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		isPicked = true;
		break;
	default:
		break;
	}

}