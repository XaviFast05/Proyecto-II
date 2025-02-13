#include "Checkpoint.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"

Checkpoint::Checkpoint() : Entity(EntityType::CHECKPOINT)
{
	name = "Checkpoint";
}

Checkpoint::~Checkpoint() {}

bool Checkpoint::Awake() {
	return true;
}

bool Checkpoint::Start() {

	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());

	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//position.setX(parameters.attribute("x").as_int());
	//position.setY(parameters.attribute("y").as_int());

	off.LoadAnimations(parameters.child("animations").child("campfireoff"));
	currentAnimation = &off;

	on.LoadAnimations(parameters.child("animations").child("campfireon"));
	currentAnimation = &on;

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::STATIC);
	pbody->listener = this;

	pbody->ctype = ColliderType::CHECKPOINT;


	return true;
}

bool Checkpoint::Update(float dt)
{
	if (isPicked) {
		currentAnimation = &on;
	}
	else {
		currentAnimation = &off;
	}

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();

	return true;
}

bool Checkpoint::CleanUp()
{
	LOG("Cleanup item");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Checkpoint::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collision PLAYER");
		isPicked = true;
		//pbody->body->SetEnabled(true);
		break;
	default:
		break;
	}

}