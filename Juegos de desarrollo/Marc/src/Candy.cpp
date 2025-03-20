#include "Candy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"


Candy::Candy() : Entity(EntityType::CANDY)
{
	name = "candy";
}

Candy::~Candy() {}

bool Candy::Awake() {
	return true;
}


bool Candy::Start() {

	//initilize textures
	candyTex = Engine::GetInstance().textures.get()->Load(parameters.child("properties").attribute("texture").as_string());

	texW = parameters.child("properties").attribute("w").as_float();
	texH = parameters.child("properties").attribute("h").as_float();
	/* L08 TODO 4: Add a physics to an item - initialize the physics body*/

	position.setX(instanceParameters.attribute("x").as_float());
	position.setY(instanceParameters.attribute("y").as_float());
	type = instanceParameters.attribute("type").as_string();

	function = POINTS;
	if (type == "corn") yAnim = 0;
	else if (type == "ube") yAnim = 16;
	else if (type == "swirl") yAnim = 32;
	else if (type == "toffee") yAnim = 48;
	else if (type == "mint") yAnim = 64;
	else if (type == "lolly") yAnim = 80;
	else if (type == "grape") yAnim = 96;
	else if (type == "choc") yAnim = 112;
	else if (type == "clock") {
		yAnim = 128;
		function = TIME;
	}
	else if (type == "heart")
	{
		yAnim = 144;
		function = HEALING;
	}


	floating.LoadAnimations(parameters.child("animations").child("floating"));

	for (int i = 0; i < floating.totalFrames; ++i)
		floating.frames[i].y = yAnim;


	pbody = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX() + texW / 2, (int)position.getY() + texH / 2, texW, texH, bodyType::STATIC);
	pbody->listener = this;

	
	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::CANDY;

	picked = false;
	return true;
}

bool Candy::Update(float dt)
{
	if (!Engine::GetInstance().render.get()->InCameraView(pbody->GetPosition().getX() - texW, pbody->GetPosition().getY() - texH, texW, texH))
	{
		return true;
	}

	if (picked && pbody->body->IsEnabled()) {
		pbody->body->SetEnabled(false);
		picked = true;
	}
	else {
		currentAnim = &floating;
	}

	if (!picked) {
		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

		currentAnim->Update();
		Engine::GetInstance().render.get()->DrawTexture(candyTex, (int)position.getX(), (int)position.getY(), &currentAnim->GetCurrentFrame());
	}
	
	return true;
}

bool Candy::CleanUp()
{
	return true;
}

void Candy::SetPlayer(Player* _player) {
	player = _player;
}

void Candy::SaveData(pugi::xml_node candyNode)
{
	
	candyNode.attribute("x").set_value(pbody->GetPhysBodyWorldPosition().getX());
	candyNode.attribute("y").set_value(pbody->GetPhysBodyWorldPosition().getY());
	candyNode.attribute("type").set_value(type.c_str());
	candyNode.attribute("picked").set_value(picked);
}


void Candy::LoadData(pugi::xml_node candyNode)
{
	pbody->SetPhysPositionWithWorld(candyNode.attribute("x").as_float(), candyNode.attribute("y").as_float());
	picked = candyNode.attribute("picked").as_bool();
	if (picked) 
		pbody->body->SetEnabled(false);
	else 
		pbody->body->SetEnabled(true);
}

void Candy::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		picked = true;
		break;

	case ColliderType::UNKNOWN:
		break;

	default:
		break;
	}
}
