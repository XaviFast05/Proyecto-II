#include "Checkpoint.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"
#include "EntityManager.h"

Checkpoint::Checkpoint() : Entity(EntityType::CHECKPOINT)
{
	name = "checkpoint";
}

Checkpoint::~Checkpoint() {}

bool Checkpoint::Awake() {
	return true;
}

bool Checkpoint::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	hasSounded = false;

	// Cargar animaciones
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	beam.LoadAnimations(parameters.child("animations").child("beam"));
	currentAnimation = &idle;

	checkpointSFX = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/checkpoint.ogg");

	// Agregar física al objeto - inicializar el cuerpo físico
	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texH / 4, (int)position.getY() + texH / 4, 90, 30, bodyType::STATIC);

	// Asignar tipo de colisionador

	pbody->ctype = ColliderType::CHECKPOINT;
	pbody->listener = this;


	return true;
}

bool Checkpoint::Update(float dt)
{

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH + 20);

	// Dibujar al enemigo en la pantalla y actualizar su animación
	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();


	return true;
}

bool Checkpoint::CleanUp()
{
	return true;
}

void Checkpoint::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collided with player - SAVE");
		currentAnimation = &beam;
		if (!hasSounded) {
			hasSounded = true;
			Engine::GetInstance().audio.get()->PlayFx(checkpointSFX);
			if (currentAnimation->HasFinished()) {
				beam.Reset();
			}
		}
		break;
	}
}

void Checkpoint::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		LOG("Collided with player");
		break;
	}
}
