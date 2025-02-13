#include "Fire.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"

Fire::Fire() : Entity(EntityType::FIRE)
{
	name = "fire";
}

Fire::~Fire() {}

bool Fire::Awake() {
	return true;
}

bool Fire::Start() {

	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	fire.LoadAnimations(parameters.child("animations").child("fire"));
	currentAnimation = &fire;

	return true;
}

bool Fire::Update(float dt)
{
	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();

	return true;
}

bool Fire::CleanUp()
{
	LOG("Cleanup fire");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}