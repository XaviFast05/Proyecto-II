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
#include "Player.h"
#include "Audio.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{

}

Enemy::~Enemy() {
	
}

bool Enemy::Awake() {
	return true;
}

bool Enemy::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	currentAnimation = &idle;

	//Add a physics to an item - initialize the physics body
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

	//Assign collider type
	pbody->ctype = ColliderType::ENEMY;
	pbody->listener = this;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	dead = false;

	// Initialize pathfinding
	pathfinding = new Pathfinding();
	ResetPath();

	//SFX Node Load


	return true;
}

bool Enemy::Update(float dt)
{
	return true;
}

bool Enemy::CleanUp()
{
	pathfinding->CleanUp();
	delete pathfinding;
	return true;
}

void Enemy::SetPosition(Vector2D pos) {
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

void Enemy::AddAnimation(Animation& anim, int startPosY, int frameSize, int numFrames)
{
	for (int i = 0; i < numFrames; i++)
	{
		anim.PushBack({ i*frameSize,startPosY,frameSize,frameSize });
	}
}

bool Enemy::CheckIfTwoPointsNear(Vector2D point1, Vector2D point2, float nearDistance)
{
	b2Vec2 vectorBetweenPoints = { point1.getX() - point2.getX(), point1.getY() - point2.getY() };
	return vectorBetweenPoints.Length()<nearDistance;
}

void Enemy::SetPlayer(Player* _player)
{
	player = _player;
}

void Enemy::SetPath(pugi::xml_node pathNode)
{
	route.clear();

	if (pathNode)
	{
		for (pugi::xml_node pointNode : pathNode.children("point")) {

			float x = pointNode.attribute("x").as_float();
			float y = pointNode.attribute("y").as_float();

			route.emplace_back( x, y );
		}

		for (int i = 0; i < route.size(); i++)
		{
			route[i] = Engine::GetInstance().map.get()->WorldToWorldCenteredOnTile(route[i].getX(), route[i].getY());
		}

		routeDestinationIndex = 0;
		destinationPoint = route[routeDestinationIndex];
	}
}


void Enemy::SaveData(pugi::xml_node enemyNode)
{
	enemyNode.attribute("dead").set_value(dead);
	enemyNode.attribute("x").set_value(pbody->GetPhysBodyWorldPosition().getX());
	enemyNode.attribute("y").set_value(pbody->GetPhysBodyWorldPosition().getY());
	enemyNode.attribute("lives").set_value(lives);
}


void Enemy::LoadData(pugi::xml_node enemyNode)
{
	pbody->SetPhysPositionWithWorld( enemyNode.attribute("x").as_float(), enemyNode.attribute("y").as_float() );
	dead = enemyNode.attribute("dead").as_bool();
	lives = enemyNode.attribute("lives").as_int();
	if (dead) {
		state = DEAD;
		pbody->body->SetEnabled(false);
	}
	else
	{
		state = CHASING;
		pbody->body->SetEnabled(true);
	}
}

void Enemy::Restart()
{
	pbody->SetPhysPositionWithWorld(route[0].getX(), route[0].getY());
	state = PATROL;
	dead = false;
	pbody->body->SetEnabled(true);
	
	ResetPath();
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {

	
	switch (physB->ctype)
	{
	case ColliderType::WEAPON:
		if(state != DEAD)
			DMGEnemy();
		break;
	case ColliderType::SHOT:

		break;
	case ColliderType::PUMPKIN:
		break;
	case ColliderType::SPYKE:
		break;

	case ColliderType::ENEMY:
		break;
	case ColliderType::ABYSS:
	{
		break;
	}
	case ColliderType::UNKNOWN:
		break;

	default:
		break;
	}
}

void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		break;
	case ColliderType::PUMPKIN:
		break;
	case ColliderType::LADDER:
		break;
	case ColliderType::UNKNOWN:
		break;
	default:
		break;
	}
}

void Enemy::DMGEnemy() {
	lives--;
	if (lives <= 0)
	{
		deathTimer.Start();
		death.Reset();
		state = DEAD;
	}
	
}