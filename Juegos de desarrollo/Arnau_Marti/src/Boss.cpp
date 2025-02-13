#include "Boss.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"
#include "tracy/Tracy.hpp"

Boss::Boss() : Entity(EntityType::BOSS)
{

}

Boss::~Boss() {
	delete pathfinding;
}

bool Boss::Awake() {
	return true;
}

bool Boss::Start() {

	isLookingRight = true;
	isAttacking = false;
	lives = 20;
	//state = PATROL;
	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idleRight.LoadAnimations(parameters.child("animations").child("idleRight"));
	idleLeft.LoadAnimations(parameters.child("animations").child("idleLeft"));
	walkRight.LoadAnimations(parameters.child("animations").child("runRight"));
	walkLeft.LoadAnimations(parameters.child("animations").child("runLeft"));
	attackRight.LoadAnimations(parameters.child("animations").child("attackRight"));
	attackLeft.LoadAnimations(parameters.child("animations").child("attackLeft"));
	dieRight.LoadAnimations(parameters.child("animations").child("dieRight"));
	dieLeft.LoadAnimations(parameters.child("animations").child("dieLeft"));
	hurtRight.LoadAnimations(parameters.child("animations").child("hurtRight"));
	hurtLeft.LoadAnimations(parameters.child("animations").child("hurtLeft"));
	currentAnimation = &idleRight;

	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texH/2, bodyType::DYNAMIC);
	pbody->listener = this;
	//Assign collider type
	pbody->ctype = ColliderType::BOSS;

	pbody->body->SetEnabled(true);

	// Set the gravity of the body

	//if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);
	pbody->body->SetGravityScale(1);

	// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();

	return true;
}

bool Boss::Update(float dt)
{
	ZoneScoped;
	if (!dead) {
		Vector2D playerPos = Engine::GetInstance().scene.get()->GetPlayerPosition();

		Vector2D enemyPos = GetPosition();

		distanceVec = enemyPos - playerPos;
		distance = distanceVec.magnitude();

		if (PIXEL_TO_METERS(distance) <= 5) {
			if (PIXEL_TO_METERS(distance) <= 1) {
				Attack();
			}
			else {
				
				Chase();
			}
		}
		else {
			Patrol();
		}
		//LOG("Enemy position: %f, %f", position.getX(), position.getY());

		// saber en que posision esta el enemigo y pintar la textura
		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

		if (!dead) {
			Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY()-16, &currentAnimation->GetCurrentFrame());
			currentAnimation->Update();
		}

		// Draw pathfinding
		debug = Engine::GetInstance().physics.get()->returnDebug();
		if (debug) {
			pathfinding->DrawPath();
		}

		if (pbody->body->GetLinearVelocity().x < 0) {
			isLookingRight = false;
		}
		else {
			isLookingRight = true;
		}
	}
	else {
		pbody->body->SetEnabled(false);
	}


	return true;
}

bool Boss::CleanUp()
{
	return true;
}

void Boss::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Boss::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Boss::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}

void Boss::Attack() {

	//Engine::GetInstance().audio.get()->PlayFx(attack);
	isAttacking = true;

	pbody->body->SetLinearVelocity(b2Vec2(0, 0));
	if (isLookingRight && isAttacking) {
		currentAnimation = &attackRight;
	}
	else {
		currentAnimation = &attackLeft;
	}

}

void Boss::Chase() {

	isAttacking = false;
	state = CHASE;
	if (isLookingRight) {
		currentAnimation = &walkRight;
	}
	else {
		currentAnimation = &walkLeft;
	}
	ResetPath();
	// propgara hasta encontrar el player
	while (pathfinding->pathTiles.empty()) {
		pathfinding->PropagateAStar(SQUARED);
	}

	if (pathfinding != nullptr && !pathfinding->pathTiles.empty()) {

		int posIndex = 0;
		Vector2D nextPosEnemy;


		for (const auto& enemyPos : pathfinding->pathTiles) {
			posIndex++;
			if (posIndex == pathfinding->pathTiles.size() - 1) {
				nextPosEnemy = enemyPos;
				break;
			}
		}



		Vector2D nextPosEnemyPixels = Engine::GetInstance().map.get()->MapToWorldCenter(nextPosEnemy.getX(), nextPosEnemy.getY());

		Vector2D enemyWorldPos = { (float)METERS_TO_PIXELS(pbody->body->GetPosition().x) , (float)METERS_TO_PIXELS(pbody->body->GetPosition().y) };

		Vector2D direction = { nextPosEnemyPixels.getX() - enemyWorldPos.getX(), nextPosEnemyPixels.getY() - enemyWorldPos.getY() };

		float distance = direction.magnitude();

		pbody->body->SetLinearVelocity({ direction.normalized().getX(),pbody->body->GetLinearVelocity().y});
	}

}

void Boss::Patrol() {
	isAttacking = false;
	if (isLookingRight) {
		currentAnimation = &idleRight;
	}
	else {
		currentAnimation = &idleLeft;
	}
	state = PATROL;
	pbody->body->SetLinearVelocity({ 0,0 });
}

void Boss::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::SLASH:
		LOG("Collision SLASH");
		takeDamage();
		break;
	case ColliderType::PLAYER:
		if (isAttacking) {
			Engine::GetInstance().scene.get()->takeDamagePlayer();
		}
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	case ColliderType::GROUND:
		LOG("Collision GROUND");
		break;
	case ColliderType::SPIKES:
		LOG("Collision SPIKES");
		break;
	case ColliderType::ENEMY:
		LOG("Collision ENEMY");
		break;
	default:
		break;
	}
}

void Boss::takeDamage()
{
	lives--;
	if (lives <= 0) {
		dead = true;
		pbody->body->SetLinearVelocity(b2Vec2(0, 0));
		
		if (isLookingRight) currentAnimation = &dieRight;
		else
		{
			currentAnimation = &dieLeft;
		}
	}

	if (isLookingRight)	currentAnimation = &hurtRight;

	else
	{
		currentAnimation = &hurtLeft;
	}
}

void Boss::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	case ColliderType::SPIKES:
		LOG("End Collision SPIKES");
		break;
	default:
		break;
	}
}