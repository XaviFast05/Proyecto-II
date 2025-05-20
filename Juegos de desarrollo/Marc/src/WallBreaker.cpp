#include "WallBreaker.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Physics.h"
#include "Scene.h"
#include "Player.h"
#include "Log.h"
#include "Input.h"
#include "Render.h"
#include "Map.h"
#include "EntityManager.h"
#include "CurrencyManager.h"
#include "CurrencyOrb.h"
#include "tracy/Tracy.hpp"


WallBreaker::WallBreaker() : Entity(EntityType::BREAKWALL)
{
	name = "item";
}

WallBreaker::~WallBreaker() {

}

bool WallBreaker::Awake() {
	return true;
}

bool WallBreaker::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load(parameters.child("properties").attribute("texture").as_string());

	position.setX(instanceParameters.attribute("x").as_float());
	position.setY(instanceParameters.attribute("y").as_float()); // player->position.getY() - 44 in xml

	texW = parameters.child("properties").attribute("w").as_float();
	texH = parameters.child("properties").attribute("h").as_float();

	flipSprite = instanceParameters.attribute("flipped").as_bool();
	//drawOffsetX = 0;
	//drawOffsetY = 0;

	//INIT ANIMS
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	fractured.LoadAnimations(parameters.child("animations").child("fractured"));
	broken.LoadAnimations(parameters.child("animations").child("broken"));

	currentAnimation = &idle;

	//INIT PHYSICS
	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX(), (int)position.getY(), texW / 2, texH / 2, bodyType::STATIC);
	pbody->ctype = ColliderType::BREAKWALL;
	pbody->listener = this;

	//INIT VARIABLES
	if (flipSprite) dir = RIGHT;
	else dir = LEFT;
	state = IDLE;

	//INIT CURRENCY MANAGEMENT
	//LOAD SFX
	pugi::xml_document audioFile;
	pugi::xml_parse_result result = audioFile.load_file("config.xml");
	audioNode = audioFile.child("config").child("audio").child("fx");

	hitSFX = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("swordSFX").attribute("path").as_string());
	brokenSFX = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("skeletonDeathSFX").attribute("path").as_string());

	return true;
}

bool WallBreaker::Update(float dt) {

	ZoneScoped;

	if (!Engine::GetInstance().render.get()->InCameraView(pbody->GetPosition().getX() - texW, pbody->GetPosition().getY() - texH, texW, texH))
	{
		pbody->body->SetEnabled(false);
		return true;
	}
	else
	{
		pbody->body->SetEnabled(true);
	}

	if (!Engine::GetInstance().scene.get()->paused) {

		//STATES CONTROLER
		

		switch (state) {
			break;
		case IDLE:
			currentAnimation = &idle;
			break;
		case BROKEN:
			currentAnimation = &broken;
			break;
		default:
			break;
		}

		//DRAW
		if (pbody->body->IsEnabled()) {

			currentAnimation->Update();

			b2Transform pbodyPos = pbody->body->GetTransform();
			position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2); // + drawOffsetX
			position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 1.5); // + drawOffsetY



			if (dir == LEFT) {
				Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
			}
			else if (dir == RIGHT) {
				Engine::GetInstance().render.get()->DrawTextureFlipped(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
			}
		}

	}

	return true;
}

bool WallBreaker::CleanUp()
{
	return true;
}

void WallBreaker::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype) {
	case ColliderType::WEAPON:
		break;
	case ColliderType::SHOT:
		break;
	case ColliderType::MELEE_AREA:
		hit = true;
		break;
	case ColliderType::SPYKE:
		break;
	case ColliderType::ENEMY:
		break;
	case ColliderType::ABYSS:
		break;
	case ColliderType::UNKNOWN:
		break;
	case ColliderType::MELEE_AREA_CHARGED:
		hit = true;
	default:
		break;
	}



