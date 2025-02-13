#include "MetallC15.h"
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
#include "tracy/Tracy.hpp"

MetallC15::MetallC15() : Entity(EntityType::METALLC15)
{

}

MetallC15::~MetallC15() {
	delete pathfinding;
}

bool MetallC15::Awake() {
	return true;
}

bool MetallC15::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();
	dead = parameters.attribute("dead").as_bool();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	idleL.LoadAnimations(parameters.child("animations").child("idleL"));
	hide.LoadAnimations(parameters.child("animations").child("hide"));
	appear.LoadAnimations(parameters.child("animations").child("appear"));
	walk.LoadAnimations(parameters.child("animations").child("walk"));
	walkL.LoadAnimations(parameters.child("animations").child("walkL"));

	//hitFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/HitFx.wav");

	currentAnimation = &hide;

	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	// Assign listener
	pbody->listener = this;

	// First looks to the left
	lookRight = false;

	// Enemy hiding
	appeared = false;

	// Pathfinding desactivated
	activatePathfinding = false;

	// Initialize currentPathIndex
	currentPathIndex = 0;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();

	// Initialize timer
	timer = 0;

	dead = false;


	return true;
}

bool MetallC15::Update(float dt)
{
	ZoneScoped;
	
	if (Engine::GetInstance().scene.get()->state == LEVEL1 || Engine::GetInstance().scene.get()->state == LEVEL2) {
	
		b2Vec2 velocity;

		velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);
		pathfinding->PropagateAStar(SQUARED);

		Vector2D pos = GetPosition();
		Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());


		// Persecution state
		if (pathfinding->foundDestination) {

			// Appears for the first time
			if (!appeared) {
				currentAnimation = &appear;
				appeared = true;
				timer = SDL_GetTicks();
			}

			if (SDL_GetTicks() - timer >= 700) {
				ResetPath();
				timer = SDL_GetTicks();
				currentPathIndex = 1;
			}

			// Persecution logic
			if (pathfinding->pathTiles.size() > 1) {
				auto it = pathfinding->pathTiles.end();

				int index = 0;
				while (it != pathfinding->pathTiles.begin() && index < currentPathIndex) {
					--it;
					++index;
				}
				if (it != pathfinding->pathTiles.end()) {
					Vector2D nextTile = *it;

					// Compares axis x of tiles to move in the right direction towards the player
					if (tilePos.getX() > nextTile.getX()) {
						lookRight = false;
						velocity.x = -0.15 * 16;
					}
					else if (tilePos.getX() < nextTile.getX()) {
						lookRight = true;
						velocity.x = 0.15 * 16;
					}
					else {
						velocity.x = 0;
					}

					// If is in the same tile adds next index position 
					if (abs(tilePos.getX() - nextTile.getX()) <= 0 && abs(tilePos.getY() - nextTile.getY()) <= 0) {
						currentPathIndex++;
					}

					if (lookRight) {
						currentAnimation = &walk;
					}
					else if (!lookRight) {
						currentAnimation = &walkL;
					}
					else {

						velocity = b2Vec2(0, 0);

						if (lookRight) {
							currentAnimation = &idle;
						}
						else if (!lookRight) {
							currentAnimation = &idleL;
						}
					}
				}
			}
		}
		// Idle state
		else {
			// Still hiding
			if (!appeared) {
				currentAnimation = &hide;
			}
			// Already appeared without destination
			else if (appeared) {

				if (lookRight) {
					currentAnimation = &idle;
				}
				else if (!lookRight) {
					currentAnimation = &idleL;
				}
			}

			if (SDL_GetTicks() - timer >= 700) {
				ResetPath();
				timer = SDL_GetTicks();
			}

		}

		// Apply the velocity to the enemy
		pbody->body->SetLinearVelocity(velocity);

		// L08 TODO 4: Add a physics to an item - update the position of the object from the physics.  
		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

		// Draws texture
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
		currentAnimation->Update();



		// Draw pathfinding on debug
		if (Engine::GetInstance().physics.get()->debug) {
			pathfinding->DrawPath();
		}


	}


	return true;
}

bool MetallC15::CleanUp()
{
	
	Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
	Engine::GetInstance().textures.get()->UnLoad(texture);


	return true;
}

void MetallC15::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D MetallC15::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void MetallC15::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}

void MetallC15::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::BULLET:
		CleanUp();
		break;

	case ColliderType::PLAYER:
		if (Engine::GetInstance().scene.get()->player->attack) {
			CleanUp();
		}

		break;
	}
}

