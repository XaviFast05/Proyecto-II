#include "lifeCharger.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"

LifeCharger::LifeCharger() : Entity(EntityType::LIFE_CHARGER)
{

}

LifeCharger::~LifeCharger() {}

bool LifeCharger::Awake() {
	return true;
}

bool LifeCharger::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;

	// L08 TODO 4: Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::SENSOR);

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::LIFE_CHARGE;

	pbody->listener = this;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	return true;
}

bool LifeCharger::Update(float dt)
{

	if (Engine::GetInstance().scene.get()->state == LEVEL1 || Engine::GetInstance().scene.get()->state == LEVEL2) {
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
		currentAnimation->Update();

	}

	return true;
}

bool LifeCharger::CleanUp()
{
	Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
	Engine::GetInstance().textures.get()->UnLoad(texture);
	
	return true;
}

void LifeCharger::OnCollision(PhysBody* physA, PhysBody* physB) {

	if (physB->ctype == ColliderType::PLAYER) {
		CleanUp();
	}

}