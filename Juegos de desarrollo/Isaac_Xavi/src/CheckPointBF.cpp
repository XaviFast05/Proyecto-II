#include "CheckPointBF.h"
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

CheckPointBF::CheckPointBF() : Entity(EntityType::CHECKPOINTBF)
{
	name = "checkpoinbf";
}

CheckPointBF::~CheckPointBF() {

}

bool CheckPointBF::Awake() {
	return true;
}

bool CheckPointBF::Start()
{
	// Initialize CheckPointBF parameters
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	notTaken.LoadAnimations(parameters.child("animations").child("notTaken"));
	taken.LoadAnimations(parameters.child("animations").child("taken"));
	currentAnimation = &notTaken;

	// Add sensor to the CheckPointBF - initialize physics body
	pbody = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), 20, 20, bodyType::STATIC);

	// Assign collider type
	pbody->ctype = ColliderType::CHECKPOINTBF;

	// Assign CheckPointBF class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// initialize audio effect
	CheckPointTakenFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Bonfire/fire.wav");

	return true;
}

bool CheckPointBF::Update(float dt)
{
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH + 15);
	
	if (CheckTaken == false) currentAnimation = &notTaken;
	if (CheckTaken == true)
	{
		currentAnimation = &taken;
	}

	Engine::GetInstance().render.get()->DrawTexture(texture, (float)position.getX(), (float)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();

	return true;
}

bool CheckPointBF::CleanUp()
{
	LOG("Cleanup CheckPointBF");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void CheckPointBF::SetPosition(Vector2D pos) {
	pos.setX(pos.getX());
	pos.setY(pos.getY());
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

// Define OnCollision function for the CheckPointBF. 
void CheckPointBF::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYERSENSOR:
		LOG("CheckPoint taken - SaveState");
		currentAnimation = &taken;
		if (!CheckTaken)
		{
			Engine::GetInstance().audio.get()->PlayFx(CheckPointTakenFxId);
			Saving = true;
			CheckTaken = true;
		}
		onPlayer = true;
		break;
	}
}

void CheckPointBF::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYERSENSOR:
		onPlayer = false;
		break;
	}
}