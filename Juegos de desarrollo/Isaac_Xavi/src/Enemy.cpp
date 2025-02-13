#include "Enemy.h"
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
#include "Timer.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	map = Engine::GetInstance().map.get();
}

Enemy::~Enemy() {
	delete pathfinding;
}

bool Enemy::Awake() {
	return true;
}

bool Enemy::Start() 
{
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());

	if (!hasStarted)
	{
		//Add a physics to an item - initialize the physics body
		pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, 8, bodyType::DYNAMIC);
	}
	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());

	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	flying.LoadAnimations(parameters.child("animations").child("flying"));
	idleGrounded.LoadAnimations(parameters.child("animations").child("idleGrounded"));
	flyingGrounded.LoadAnimations(parameters.child("animations").child("flyingGrounded"));
	hutaoAttack.LoadAnimations(parameters.child("animations").child("hutaoAttack"));
	hutaoIdle.LoadAnimations(parameters.child("animations").child("hutaoIdle"));

	if (isBoss)
	{
		currentAnimation = &hutaoIdle;
		bossTimer.Start();
	}
	else if (!isGrounded)
	{
		currentAnimation = &idle;
	}
	else if (isGrounded)
	{
		currentAnimation = &idleGrounded;
	}

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;
	pbody->listener = this;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	chaseFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Enemy/bird_chase.wav");
	chaseGroundedFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Enemy/Old_Man_chase.wav");
	hutaoAttackFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Enemy/hutao/babe_scream.wav");
	hutaoHurtFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Enemy/hutao/babe_pickup.wav");
	hutaoUnaliveFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/Enemy/hutao/babe_mou.wav");

	Mix_Volume(chaseFxId, 200);

	// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();
	hasStarted = true;
	return true;
}

bool Enemy::Update(float dt)
{
	Vector2D enemyCurrentPos = GetPosition();

	if (paused == true)
	{
		b2Vec2 velocityStop = b2Vec2(0, 0);
		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

		pbody->body->SetLinearVelocity(velocityStop);

		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame(), hflip);
		currentAnimation->Update();
		SetPosition(enemyCurrentPos);

	}
	else
	{
		int maxIterations = 0;
		if (isBoss)
		{
			maxIterations = 40; // Max number of iterations to avoid crashes
		}
		else
		{
			maxIterations = 13; // Max number of iterations to avoid crashes
		}
		int iterations = 0;

		if (!isGrounded and !isBoss)
		{
			currentAnimation = &idle;
		}
		else if (isGrounded and !isBoss)
		{
			currentAnimation = &idleGrounded;
		}
		else if (isBoss)
		{
			currentAnimation = &hutaoIdle;
		}

		pathfinding->layerNav = map->GetNavigationLayer();

		//Reset and propagate the pathfanding to follow the player
		Vector2D enemyPos = GetPosition();
		Vector2D playerPos = Engine::GetInstance().scene.get()->GetPlayerPosition();
		Vector2D playerTilePos = Engine::GetInstance().map.get()->WorldToMap(playerPos.getX(), playerPos.getY());

		pathfinding->ResetPath(Engine::GetInstance().map.get()->WorldToMap(enemyPos.getX(), enemyPos.getY()));

		while (pathfinding->pathTiles.empty() && iterations < maxIterations) {
			pathfinding->PropagateAStar(SQUARED);
			iterations++;
		}

		//Get last tile
		if (pathfinding->pathTiles.size() > 3) {
			Vector2D targetTile = pathfinding->pathTiles[pathfinding->pathTiles.size() - 4];
			Vector2D targetWorldPos;
			if (isBoss == false)
			{
				targetWorldPos = Engine::GetInstance().map.get()->MapToWorld(targetTile.getX(), targetTile.getY());
			}
			else
			{

				if (bossTimer.ReadSec() < 2)
				{
					//IDLE
					currentAnimation = &hutaoIdle;
					if (side == false)
					{
						targetWorldPos = Engine::GetInstance().map.get()->MapToWorld(3, 1);
					}
					else
					{
						targetWorldPos = Engine::GetInstance().map.get()->MapToWorld(20, 2);
					}
					z = 0;
				}
				else if (bossTimer.ReadSec() < 4)
				{
					//ATTACK
					if (z == 0)
					{
						HutaoAttack();
						z = 1;
					}

					currentAnimation = &hutaoAttack;
					targetWorldPos = Engine::GetInstance().map.get()->MapToWorld(targetTile.getX(), 14);
				}
				else if (bossTimer.ReadSec() < 5)
				{
					//IDLE
					currentAnimation = &hutaoIdle;
					if (side == false)
					{
						targetWorldPos = Engine::GetInstance().map.get()->MapToWorld(20, 2);
					}
					else
					{
						targetWorldPos = Engine::GetInstance().map.get()->MapToWorld(3, 1);
					}
					z = 0;
				}
				else
				{
					bossTimer.Start();
					side = !side;
				}
			}

			//Calculate vector movement
			Vector2D movement = targetWorldPos + Vector2D(16, 16) - enemyPos;
			float distance = movement.magnitude();

			if (distance > 1.0f) { //If distance is bigger than
				movement.normalized();
				if (!isGrounded)
				{
					b2Vec2 velocity(movement.getX() * 0.05f, movement.getY() * 0.05f); // Aplicar velocidad en X e Y
					pbody->body->SetLinearVelocity(velocity);
				}
				else if (isGrounded)
				{
					b2Vec2 velocity(movement.getX() * 0.05f, 2); // Aplicar velocidad en X
					pbody->body->SetLinearVelocity(velocity);
				}
				IsSearching = true;
			}
		}

		if (enemyPos.getX() > playerPos.getX())
		{
			flipSprite = true;
			if (flipSprite == true && hflip == SDL_FLIP_NONE) {
				hflip = SDL_FLIP_HORIZONTAL;
			}
		}
		else
		{
			flipSprite = false;
			if (flipSprite == false && hflip == SDL_FLIP_HORIZONTAL) {
				hflip = SDL_FLIP_NONE;
			}
		}

		if (!isBoss)
		{
			if (IsSearching == true)
			{
				if (!isGrounded)
				{
					currentAnimation = &flying;
					x++;
				}
				else if (isGrounded)
				{
					currentAnimation = &flyingGrounded;
					y++;
				}

			}
		}


		if (currentAnimation == &flying && x == 1)
		{
			Engine::GetInstance().audio->PlayFx(chaseFxId);
			x++;
		}

		if (currentAnimation == &flyingGrounded && y == 1)
		{
			Engine::GetInstance().audio->PlayFx(chaseGroundedFxId);
			x++;
		}

		// L08 TODO 4: Add a physics to an item - update the position of the object from the physics.  
		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame(), hflip);
		currentAnimation->Update();
	}

	// Draw pathfinding 
	if (DrawingPath) pathfinding->DrawPath();

	return true;
}

bool Enemy::CleanUp()
{
	Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
	return true;
}

bool Enemy::DeleteBody()
{
	Engine::GetInstance().physics.get()->DeleteBody(pbody);
	return true;
}

void Enemy::SetPosition(Vector2D pos) {
	pos.setX(pos.getX());
	pos.setY(pos.getY());
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Enemy::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Enemy::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		break;
	case ColliderType::PLAYERSENSOR:
		break;
	}
}

void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		break;
	}
}

void Enemy::ChangeGrounded(bool grounded)
{
	isGrounded = grounded;
}

void Enemy::HurtHutao()
{
	Engine::GetInstance().audio->PlayFx(hutaoHurtFxId);
}

void Enemy::HutaoAttack()
{
	Engine::GetInstance().audio->PlayFx(hutaoAttackFxId);
}

void Enemy::unaliveHutao()
{
	Engine::GetInstance().audio->PlayFx(hutaoUnaliveFxId);
}