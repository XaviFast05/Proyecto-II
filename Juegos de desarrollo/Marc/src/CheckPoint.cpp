#include "CheckPoint.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"

CheckPoint::CheckPoint() : Entity(EntityType::CHECKPOINT)
{
	name = "item";
}

CheckPoint::~CheckPoint() {}

bool CheckPoint::Awake() {
	return true;
}


bool CheckPoint::Start() {

	
	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.child("properties").attribute("texture").as_string());
	
	/* L08 TODO 4: Add a physics to an item - initialize the physics body*/

	position.setX(instanceParameters.attribute("x").as_float());
	position.setY(instanceParameters.attribute("y").as_float());
	
	texW = parameters.child("properties").attribute("w").as_float();
	texH = parameters.child("properties").attribute("h").as_float();

	
	unlit.LoadAnimations(parameters.child("animations").child("unlit"));
	lit.LoadAnimations(parameters.child("animations").child("lit"));


	pbody = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX() + texW / 2, (int)position.getY() + texH / 2, texW, texH, bodyType::STATIC);
	pbody->listener = this;
	currentAnim = &unlit;

	alight = false;

	// L08 TODO 7: Assign collider type
	pbody->ctype =ColliderType::CHECKPOINT;

	return true;
}

bool CheckPoint::Update(float dt)
{
	currentFrame = currentAnim->GetCurrentFrame();

	if (!Engine::GetInstance().render.get()->InCameraView(pbody->GetPosition().getX() - texW, pbody->GetPosition().getY() - texH, texW, texH))
	{
		return true;
	}

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentFrame);

	return true;
}

bool CheckPoint::CleanUp()
{
	return true;
}

void CheckPoint::SetPlayer(Player* _player) {
	player = _player;
}

void CheckPoint::SaveData(pugi::xml_node itemNode)
{
	itemNode.attribute("alight").set_value(true);
}


void CheckPoint::LoadData(pugi::xml_node itemNode)
{
	alight = itemNode.attribute("alight").as_bool();
}

void CheckPoint::OnCollision(PhysBody* physA, PhysBody* physB) {
	
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		currentAnim = &lit;
		break;
	case ColliderType::UNKNOWN:
		
		break;

	default:
		break;
	}
}

void CheckPoint::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
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



