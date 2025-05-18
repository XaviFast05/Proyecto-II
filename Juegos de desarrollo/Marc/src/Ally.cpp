#include "Ally.h"
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
#include "Pathfinding.h"
#include "Audio.h"

Ally::Ally() : Entity(EntityType::ALLY)
{

}

Ally::~Ally() {

}

bool Ally::Awake() {
	return true;
}

bool Ally::Start() {

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
	pbody = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texW, texH, bodyType::DYNAMIC);

	//Assign collider type
	pbody->ctype = ColliderType::ALLY;
	pbody->listener = this;

	// Set the gravity of the body
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);

	//// Initialize pathfinding
	//pathfinding = new Pathfinding();
	//ResetPath();

	//SFX Node Load
	return true;
}

bool Ally::Update(float dt)
{
	return true;
}

bool Ally::CleanUp()
{
	pathfinding->CleanUp();
	delete pathfinding;
	return true;
}

void Ally::SetPosition(Vector2D pos) {
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Ally::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Ally::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}

void Ally::AddAnimation(Animation& anim, int startPosY, int frameSize, int numFrames)
{
	for (int i = 0; i < numFrames; i++)
	{
		anim.PushBack({ i * frameSize,startPosY,frameSize,frameSize });
	}
}

bool Ally::CheckIfTwoPointsNear(Vector2D point1, Vector2D point2, float nearDistance)
{
	b2Vec2 vectorBetweenPoints = { point1.getX() - point2.getX(), point1.getY() - point2.getY() };
	return vectorBetweenPoints.Length() < nearDistance;
}

void Ally::SetPlayer(Player* _player)
{
	player = _player;
}

void Ally::SetPath(pugi::xml_node pathNode)
{
	route.clear();

	if (pathNode)
	{
		for (pugi::xml_node pointNode : pathNode.children("point")) {

			float x = pointNode.attribute("x").as_float();
			float y = pointNode.attribute("y").as_float();

			route.emplace_back(x, y);
		}

		for (int i = 0; i < route.size(); i++)
		{
			route[i] = Engine::GetInstance().map.get()->WorldToWorldCenteredOnTile(route[i].getX(), route[i].getY());
		}

		routeDestinationIndex = 0;
		destinationPoint = route[routeDestinationIndex];
	}
}


void Ally::SaveData(pugi::xml_node AllyNode)
{
	AllyNode.attribute("x").set_value(pbody->GetPhysBodyWorldPosition().getX());
	AllyNode.attribute("y").set_value(pbody->GetPhysBodyWorldPosition().getY());
}


void Ally::LoadData(pugi::xml_node AllyNode)
{
	pbody->SetPhysPositionWithWorld(AllyNode.attribute("x").as_float(), AllyNode.attribute("y").as_float());
	state = PATROL;
	pbody->body->SetEnabled(true);
	
}

void Ally::Restart()
{
	pbody->SetPhysPositionWithWorld(route[0].getX(), route[0].getY());
	state = PATROL;
	pbody->body->SetEnabled(true);

	ResetPath();
}

void Ally::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype) {
	case ColliderType::PLAYER:
		break;
	case ColliderType::UNKNOWN:
		break;
	default:
		break;
	}
}

void Ally::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		break;
	case ColliderType::CHECKPOINT:
		break;
	case ColliderType::LADDER:
		break;
	case ColliderType::UNKNOWN:
		break;
	default:
		break;
	}
}