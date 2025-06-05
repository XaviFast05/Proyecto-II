#include "Merchant.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Physics.h"
#include "Scene.h"
#include "Player.h"
#include "LOG.h"
#include "tracy/Tracy.hpp"
#include "MerchantMenu.h"

Merchant::Merchant()
{
	name = "merchant";
}

Merchant::~Merchant() {

}


bool Merchant::Start() {

	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(instanceParameters.attribute("x").as_float());
	position.setY(instanceParameters.attribute("y").as_float());
	texW = parameters.attribute("w").as_float();
	texH = parameters.attribute("h").as_float();
	dialog = instanceParameters.attribute("dialog").as_string();
	drawOffsetX = 0;
	drawOffsetY = 0;

	idle.LoadAnimations(parameters.child("animations").child("idle"));
	walk.LoadAnimations(parameters.child("animations").child("walk"));

	currentAnimation = &idle;

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

bool Merchant::Update(float dt) {

	ZoneScoped;

	if (!Engine::GetInstance().scene.get()->paused) {

		Vector2D playerPos = player->pbody->GetPhysBodyWorldPosition();
		Vector2D playerPosCenteredOnTile = Engine::GetInstance().map.get()->WorldToWorldCenteredOnTile(playerPos.getX(), playerPos.getY());

		//LOGIC CONTROLER
		if (state == PATROL) {
			
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
		else if (state == DETECTION) {


			if (playerPos.getX() > position.getX()) dir = RIGHT;
			else dir = LEFT;

			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Z) == KEY_DOWN || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_B) == KEY_DOWN) {
				state = INTERACTION;
				player->pbody->body->SetEnabled(false);
				Engine::GetInstance().merchantMenu.get()->merchantPanelOpen = true;
				Engine::GetInstance().merchantMenu.get()->hasOpened = true;
				Engine::GetInstance().merchantMenu.get()->leaveMenu = false;

			}

		}
		else if (INTERACTION) {

			if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Z) == KEY_DOWN || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_B) == KEY_DOWN) || !Engine::GetInstance().merchantMenu.get()->merchantPanelOpen) {
				state = DETECTION;
				player->pbody->body->SetEnabled(true) ;
				Engine::GetInstance().merchantMenu.get()->merchantPanelOpen = false;
			}


		}

	}


	switch (state) {

	case INTERACTION:
		currentAnimation = &idle;
		break;
	case DETECTION:
		currentAnimation = &idle;
		break;
	case PATROL:
		currentAnimation = &walk;
		break;
	default:
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
			Engine::GetInstance().render.get()->DrawTextureBuffer(texture, (int)position.getX(), (int)position.getY() + texH / 2 + 10, false, ENTITIES, &currentAnimation->GetCurrentFrame());
		}
		else if (dir == RIGHT) {
			Engine::GetInstance().render.get()->DrawTextureBuffer(texture, (int)position.getX(), (int)position.getY() + texH / 2 + 10, true, ENTITIES, &currentAnimation->GetCurrentFrame());
		}
	}

	

	return true;
}

void Merchant::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype) {
	case ColliderType::PLAYER:
		
		if (dialog != "")
		{
			player->StartDialog(dialog);
			dialog = "";
		}

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

void Merchant::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {

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