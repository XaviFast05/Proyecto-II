#include "DestructibleWall.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "tracy/Tracy.hpp"   

DestructibleWall::DestructibleWall() : Entity(EntityType::CHECKPOINT)
{
	name = "item";
}

DestructibleWall::~DestructibleWall() {}

bool DestructibleWall::Awake() {
	return true;
}


bool DestructibleWall::Start() {

	//initilize textures
	destructibleWallType = (DestructibleWallType)instanceParameters.attribute("destructibleWallType").as_int();
	//initilize textures
	switch (destructibleWallType)
	{
	case LAYER1:
		texture = Engine::GetInstance().textures.get()->Load(parameters.child("properties").attribute("texture1").as_string());
		break;
	case ONIRIC:
		texture = Engine::GetInstance().textures.get()->Load(parameters.child("properties").attribute("texture2").as_string());
		break;
	default:
		break;
	}

	position.setX(instanceParameters.attribute("x").as_float());
	position.setY(instanceParameters.attribute("y").as_float());

	texW = parameters.child("properties").attribute("w").as_float();
	texH = parameters.child("properties").attribute("h").as_float();

	timeToDestroy = parameters.child("properties").attribute("timeToDestroy").as_int();

	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texW / 2, (int)position.getY() + texH / 2, texW, texH, bodyType::STATIC);
	pbody->listener = this;

	idle.LoadAnimations(parameters.child("animations").child("idle"));
	destroying.LoadAnimations(parameters.child("animations").child("destroying"));
	currentAnim = &idle;

	destroy = false;
	toDestroy = false;

	pbody->ctype = ColliderType::DESTRUCTIBLE_WALL;

	return true;
}

bool DestructibleWall::Update(float dt)
{
	ZoneScoped;
	if (!Engine::GetInstance().render.get()->InCameraView(pbody->GetPosition().getX() - texW, pbody->GetPosition().getY() - texH, texW, texH))
	{
		return true;
	}

	if ((timerToDestroy.ReadSec() > timeToDestroy && toDestroy)|| destroy)
	{
		destroy = true;
		pbody->body->SetEnabled(false);
		active = false;
	}
	else
	{
		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

		currentFrame = currentAnim->GetCurrentFrame();
		Engine::GetInstance().render.get()->DrawTextureBuffer(texture, (int)position.getX(), (int)position.getY(), false, MAP, &currentFrame);
		currentAnim->Update();
	}

	return true;
}

bool DestructibleWall::CleanUp()
{
	return true;
}

void DestructibleWall::SaveData(pugi::xml_node itemNode)
{
	itemNode.attribute("broke").set_value(destroy);
}


void DestructibleWall::LoadData(pugi::xml_node itemNode)
{
	destroy = itemNode.attribute("broke").as_bool();

}

void DestructibleWall::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::MELEE_AREA_CHARGED:
		timerToDestroy.Start();
		toDestroy = true;
		destroying.Reset();
		currentAnim = &destroying;
		break;
	case ColliderType::UNKNOWN:

		break;

	default:
		break;
	}
}

void DestructibleWall::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:

		break;
	case ColliderType::UNKNOWN:

		break;
	default:
		break;
	}
}



