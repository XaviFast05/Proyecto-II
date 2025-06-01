#include "ParticleManager.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"
#include "Player.h"
#include "Audio.h"

Particle::Particle() : Entity(EntityType::PARTICLE)
{

}

bool Particle::Awake() {
	return true;
}

bool Particle::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnim = &idle;

	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texW, texH, bodyType::STATIC);

	//Assign collider type
	pbody->ctype = ColliderType::PARTICLE;
	pbody->listener = this;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);


	return true;
}

bool Particle::Update(float dt)
{
	return true;
}

bool Particle::CleanUp()
{

	return true;
}

void Particle::SetDirection(Vector2D dir)
{
	direction = dir;
	direction = direction.normalized();
}

void Particle::SetPosition(Vector2D pos)
{
	pbody->body->SetTransform({ PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()) }, 0);
}

void Particle::Restart(Vector2D pos, Vector2D dir)
{
	pbody->body->SetLinearVelocity({ 0,0 });
	pbody->body->SetTransform({ PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()) }, 0);
	direction = dir;
	direction = direction.normalized();
	isCasted = true;
	isAlive = false;
	castTimer.Start();
}