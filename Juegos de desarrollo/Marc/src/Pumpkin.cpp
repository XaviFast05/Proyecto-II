#include "Pumpkin.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"

Pumpkin::Pumpkin() : Entity(EntityType::PUMPKIN)
{
	name = "item";
}

Pumpkin::~Pumpkin() {}

bool Pumpkin::Awake() {
	return true;
}


bool Pumpkin::Start() {

	//initilize textures
	pumpkinTex = Engine::GetInstance().textures.get()->Load(parameters.child("properties").attribute("texture").as_string());
	
	/* L08 TODO 4: Add a physics to an item - initialize the physics body*/

	position.setX(instanceParameters.attribute("x").as_float());
	position.setY(instanceParameters.attribute("y").as_float());
	
	texW = parameters.child("properties").attribute("w").as_float();
	texH = parameters.child("properties").attribute("h").as_float();

	
	unlit.LoadAnimations(parameters.child("animations").child("unlit"));
	lit.LoadAnimations(parameters.child("animations").child("lit"));


	pbody = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX() + texW / 2, (int)position.getY() + texH / 2, texW, texH, bodyType::STATIC);
	
	currentAnim = &lit;

	alight = false;

	// L08 TODO 7: Assign collider type
	pbody->ctype =ColliderType::PUMPKIN;

	return true;
}

bool Pumpkin::Update(float dt)
{
	if (!Engine::GetInstance().render.get()->InCameraView(pbody->GetPosition().getX() - texW, pbody->GetPosition().getY() - texH, texW, texH))
	{
		return true;
	}

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	if (player->transformed) {
		currentAnim = &lit;
		alight = true;
	}
	else {
		currentAnim = &unlit;
		alight = false;
	}
	
	Engine::GetInstance().render.get()->DrawTexture(pumpkinTex, (int)position.getX(), (int)position.getY(), &currentAnim->GetCurrentFrame());

	return true;
}

bool Pumpkin::CleanUp()
{
	return true;
}

void Pumpkin::SetPlayer(Player* _player) {
	player = _player;
}

void Pumpkin::SaveData(pugi::xml_node itemNode)
{
	itemNode.attribute("alight").set_value(true);
	itemNode.attribute("x").set_value(pbody->GetPhysBodyWorldPosition().getX());
	itemNode.attribute("y").set_value(pbody->GetPhysBodyWorldPosition().getY());
}


void Pumpkin::LoadData(pugi::xml_node itemNode)
{
	pbody->SetPhysPositionWithWorld(itemNode.attribute("x").as_float(), itemNode.attribute("y").as_float());
	alight = itemNode.attribute("alight").as_bool();
}



