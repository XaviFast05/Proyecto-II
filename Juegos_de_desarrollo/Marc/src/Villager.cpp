#include "Villager.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Physics.h"
#include "Scene.h"
#include "Player.h"
#include "LOG.h"
#include "tracy/Tracy.hpp"
#include "MerchantMenu.h"
#include "DialoguesManager.h"

Villager::Villager()
{
	name = "villager";
}

Villager::~Villager() {

}


bool Villager::Start() {

	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(instanceParameters.attribute("x").as_float());
	position.setY(instanceParameters.attribute("y").as_float());
	texW = parameters.attribute("w").as_float();
	texH = parameters.attribute("h").as_float();
	dialog = instanceParameters.attribute("dialog").as_string();
	villagerType = instanceParameters.attribute("villagerType").as_int();
	movementType = instanceParameters.attribute("movementType").as_int();
	drawOffsetX = 0;
	drawOffsetY = 0;

	idleGrandma.LoadAnimations(parameters.child("animations").child("idleGrandma"));
	walkGrandma.LoadAnimations(parameters.child("animations").child("walkGrandma"));
	walkBeard.LoadAnimations(parameters.child("animations").child("walkBeard"));
	idleHeadset.LoadAnimations(parameters.child("animations").child("idleHeadset"));
	walkHeadset.LoadAnimations(parameters.child("animations").child("walkHeadset"));
	walkFlower.LoadAnimations(parameters.child("animations").child("walkFlower"));
	walkHat.LoadAnimations(parameters.child("animations").child("walkHat"));
	walkGlasses.LoadAnimations(parameters.child("animations").child("walkGlasses"));
	walkSunGlasses.LoadAnimations(parameters.child("animations").child("walkSunGlasses"));
	walkLarge.LoadAnimations(parameters.child("animations").child("walkLarge"));
	walkBigHead.LoadAnimations(parameters.child("animations").child("walkBigHead"));
	walkDog.LoadAnimations(parameters.child("animations").child("walkDog"));

	currentAnimation = &idleGrandma;

	//INIT VARIABLES
	state = PATROL;
	speed = parameters.child("properties").attribute("speed").as_float();
	movementArea = instanceParameters.attribute("movementArea").as_float();
	detectionArea = parameters.child("properties").attribute("detectionArea").as_float();
	dir = LEFT;

	//INIT PHYSICS
	pbody = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), detectionArea, 128, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::ALLY;
	pbody->body->SetGravityScale(0.0f);
	pbody->body->SetFixedRotation(true);
	pbody->listener = this;

	//LOAD SFX
	pugi::xml_document audioFile;
	pugi::xml_parse_result result = audioFile.load_file("config.xml");
	audioNode = audioFile.child("config").child("audio").child("fx");

	justTurned = false;
	initialPosX = position.getX();
	lookTimerOn = false;
	lookTime = 2.0f;

	return true;
}

bool Villager::Update(float dt) {

	ZoneScoped;

	if (!Engine::GetInstance().scene.get()->paused) {

		Vector2D playerPos = player->pbody->GetPhysBodyWorldPosition();
		Vector2D playerPosCenteredOnTile = Engine::GetInstance().map.get()->WorldToWorldCenteredOnTile(playerPos.getX(), playerPos.getY());

		//LOGIC CONTROLER
		if (state == PATROL) {
			if (movementType == 1) {

				dist = movementArea / 2;

				if (lookTimerOn) {
					if (lookTimer.ReadSec() >= lookTime) {
						lookTimerOn = false;
						if (position.getX() >= initialPosX) dir = LEFT;
						else dir = RIGHT;

						justTurned = true;
					}
					else {
						pbody->body->SetLinearVelocity({ b2Vec2_zero });
					}
				}
				else {

					if (dir == RIGHT) pbody->body->SetLinearVelocity({ speed, 0 });
					else pbody->body->SetLinearVelocity({ -speed, 0 });


					if (!justTurned && (position.getX() > initialPosX + dist || position.getX() < initialPosX - dist)) {

						pbody->body->SetLinearVelocity({ b2Vec2_zero });
						lookTimer.Start();
						lookTimerOn = true;
					}

					if (justTurned) {
						float currentDist = abs(position.getX() - initialPosX);
						if (currentDist < dist * 0.9f) {
							justTurned = false;
						}
					}
				}
			}
			else if (movementType == 2) {

				pbody->body->SetLinearVelocity({ b2Vec2_zero });

			}
			
		}
		else if (state == DETECTION) {


			if (playerPos.getX() > position.getX()) dir = RIGHT;
			else dir = LEFT;

			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Z) == KEY_DOWN || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_B) == KEY_DOWN) {
				state = INTERACTION;
				player->pbody->body->SetEnabled(false);
				if (dialog != "")
				{
					player->StartDialog(dialog);
					dialog = "";
				}

			}

		}
		else if (INTERACTION) {

			if (!Engine::GetInstance().scene.get()->player->dialoguesManager->GetOnDialogue()) {
				state = DETECTION;
				player->pbody->body->SetEnabled(true);

			}
		}

	}



	switch (villagerType) {
	case 1:
		if (state == DETECTION || state == INTERACTION) {
			currentAnimation = &idleGrandma;
		}
		else {
			currentAnimation = &walkGrandma;
		}
		break;
	case 2:
		currentAnimation = &walkBeard;
		break;
	case 3:
		if (state == DETECTION || state == INTERACTION) {
			currentAnimation = &idleHeadset;
		}
		else {
			currentAnimation = &walkHeadset;
		}
		break;
	case 4:
		currentAnimation = &walkFlower;
		break;
	case 5:
		currentAnimation = &walkHat;
		break;
	case 6:
		currentAnimation = &walkGlasses;
		break;
	case 7:
		currentAnimation = &walkSunGlasses;
		break;
	case 8:
		currentAnimation = &walkLarge;
		break;
	case 9:
		currentAnimation = &walkBigHead;
		break;
	case 10:
		currentAnimation = &walkDog;
		break;
	}


	//DIRECTION
	if (pbody->body->GetLinearVelocity().x > 0.2f) {
		dir = RIGHT;
	}
	else if (pbody->body->GetLinearVelocity().x < -0.2f) {
		dir = LEFT;
	}

	//DRAW
	if (pbody->body->IsEnabled()) {

		if (Engine::GetInstance().GetDebug())
		{
			Engine::GetInstance().render.get()->DrawCircleBuffer(initialPosX, position.getY() + texH / 2, movementArea, 255, 255, 255, 255, FRONT);
		}

		currentAnimation->Update();

		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2 + drawOffsetX);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 1.5 + drawOffsetY);



		if (dir == LEFT) {
			Engine::GetInstance().render.get()->DrawTextureBuffer(texture, (int)position.getX(), (int)position.getY() + texH / 3 - 10, false, ENTITIES, &currentAnimation->GetCurrentFrame());
		}
		else if (dir == RIGHT) {
			Engine::GetInstance().render.get()->DrawTextureBuffer(texture, (int)position.getX(), (int)position.getY() + texH / 3 - 10, true, ENTITIES, &currentAnimation->GetCurrentFrame());
		}
	}



	return true;
}

void Villager::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype) {
	case ColliderType::PLAYER:


		if (state == PATROL) {

			state = DETECTION;
			pbody->body->SetLinearVelocity({ b2Vec2_zero });
		}
		else if (state == DETECTION) state = PATROL;
		break;

	case ColliderType::UNKNOWN:
		break;
	default:
		break;
	}
}

void Villager::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype) {
	case ColliderType::PLAYER:

		if (state == DETECTION) state = PATROL;
		break;

	case ColliderType::UNKNOWN:
		break;
	default:
		break;
	}
}