#include "Merchant.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Physics.h"
#include "Scene.h"
#include "Player.h"
#include "LOG.h"
#include "tracy/Tracy.hpp"

Merchant::Merchant()
{

}

Merchant::~Merchant() {

}


bool Merchant::Start() {

	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_float());
	position.setY(parameters.attribute("y").as_float());
	texW = parameters.attribute("w").as_float();
	texH = parameters.attribute("h").as_float();
	drawOffsetX = 0;
	drawOffsetY = 0;

	//INIT ANIMS
	AddAnimation(idle, 0, texW, 1);
	idle.speed = 0.2f;


	AddAnimation(walk, 0, texW, 4);
	walk.speed = 0.2f;

	idle.LoadAnimations(parameters.child("animations").child("idle"));
	walk.LoadAnimations(parameters.child("animations").child("walk"));

	currentAnimation = &idle;

	////INIT ROUTE
	//for (int i = 0; i < route.size(); i++)
	//{
	//	route[i] = Engine::GetInstance().map.get()->WorldToWorldCenteredOnTile(route[i].getX(), route[i].getY());
	//}
	//routeDestinationIndex = 0;
	//destinationPoint = route[routeDestinationIndex];



	////INIT PATH
	//pathfinding = new Pathfinding();
	//ResetPath();

	//INIT VARIABLES
	state = PATROL;
	speed = parameters.child("properties").attribute("speed").as_float();
	movementArea = parameters.child("properties").attribute("movementArea").as_float();
	detectionArea = parameters.child("properties").attribute("detectionArea").as_float();
	dir = LEFT;

	//INIT PHYSICS
	pbody = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), detectionArea, 128, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::ALLY;
	pbody->body->SetGravityScale(0.0f);
	pbody->body->SetFixedRotation(true);
	pbody->listener = this;
	//pbody->body->SetTransform({ PIXEL_TO_METERS(destinationPoint.getX()), PIXEL_TO_METERS(destinationPoint.getY()) }, 0);

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
		/*dist = pbody->GetPhysBodyWorldPosition().distanceEuclidean(player->pbody->GetPhysBodyWorldPosition());*/
	
	
	//STATES CHANGERS
	
	//if (dist > patrolArea && state != PATROL)
	//{
	//	state = PATROL;
	//	ResetPath();
	//	destinationPoint = route[routeDestinationIndex];
	//}
	//else if (dist <= chaseArea/* && state != CHASING*/)
	//{

	//	if (state != INTERACTION)
	//	{
	//		state = PATROL;
	//		ResetPath();
	//		destinationPoint = route[routeDestinationIndex];
	//	}
	//}



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
			}

		}
		else if (INTERACTION) {

			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Z) == KEY_DOWN || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_B) == KEY_DOWN) {
				state = DETECTION;

				player->pbody->body->SetEnabled(true) ;
			}
		}
	
		//if (state == PATROL) {

		//	Vector2D physPos = pbody->GetPhysBodyWorldPosition();
		//	if (CheckIfTwoPointsNear(destinationPoint, { physPos.getX(), physPos.getY() }, 7))
		//	{
		//		routeDestinationIndex++;
		//		if (routeDestinationIndex == route.size()) routeDestinationIndex = 0;
		//		destinationPoint = route[routeDestinationIndex];
		//		ResetPath();
		//	}
		//}
		//else if (state == INTERACTION) {

		//	if (destinationPoint != playerPosCenteredOnTile)
		//	{
		//		destinationPoint = playerPosCenteredOnTile;
		//		ResetPath();
		//	}
		//}

		////PATHFINDING CONTROLER
		//if (state == PATROL)
		//{
		//	if (pathfinding->pathTiles.empty())
		//	{
		//		while (pathfinding->pathTiles.empty())
		//		{
		//			pathfinding->PropagateAStar(SQUARED, destinationPoint, Pathfinding::WALK);

		//		}
		//		pathfinding->pathTiles.pop_back();
		//	}
		//	else
		//	{

		//		Vector2D nextTile = pathfinding->pathTiles.back();
		//		Vector2D nextTileWorld = Engine::GetInstance().map.get()->MapToWorldCentered(nextTile.getX(), nextTile.getY());

		//		if (CheckIfTwoPointsNear(nextTileWorld, { (float)METERS_TO_PIXELS(pbody->body->GetPosition().x), (float)METERS_TO_PIXELS(pbody->body->GetPosition().y) }, 3)) {

		//			pathfinding->pathTiles.pop_back();
		//			if (pathfinding->pathTiles.empty()) ResetPath();
		//		}
		//		else {
		//			Vector2D nextTilePhysics = { PIXEL_TO_METERS(nextTileWorld.getX()),PIXEL_TO_METERS(nextTileWorld.getY()) };
		//			b2Vec2 direction = { nextTilePhysics.getX() - pbody->body->GetPosition().x, nextTilePhysics.getY() - pbody->body->GetPosition().y };
		//			direction.Normalize();
		//			pbody->body->SetLinearVelocity({ direction.x * speed, pbody->body->GetLinearVelocity().y });
		//		}
		//	}
		//	Vector2D currentTile = Engine::GetInstance().map.get()->WorldToMap(pbody->GetPhysBodyWorldPosition().getX(), pbody->GetPhysBodyWorldPosition().getY());

		//	if (pathfinding->IsJumpable(currentTile.getX(), currentTile.getY()) && VALUE_NEAR_TO_0(pbody->body->GetLinearVelocity().LengthSquared()))
		//	{
		//		pbody->body->ApplyLinearImpulseToCenter({ 0, -jumpForce }, true);
		//	}
		//}
		//else
		//{
		//	pbody->body->SetLinearVelocity({ 0,0 });

		//}

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

	if (pbody->body->GetLinearVelocity().LengthSquared() == 0 && state != DEAD) {
		currentAnimation = &idle;
	}
	if (pbody->body->GetLinearVelocity().LengthSquared() != 0 && state != DEAD) {
		currentAnimation = &walk;
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
			Engine::GetInstance().render.get()->DrawCircle(initialPosX, position.getY() + texH / 2, movementArea, 255, 255, 255);
			//Engine::GetInstance().render.get()->DrawCircle(destinationPoint.getX(), destinationPoint.getY(), 3, 255, 0, 0);
			/*pathfinding->DrawPath();*/
		}

		currentAnimation->Update();

		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2 + drawOffsetX);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 1.5 + drawOffsetY);



		if (dir == LEFT) {
			Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY() + texH / 2 + 10, &currentAnimation->GetCurrentFrame());
		}
		else if (dir == RIGHT) {
			Engine::GetInstance().render.get()->DrawTextureFlipped(texture, (int)position.getX(), (int)position.getY() + texH / 2 + 10, &currentAnimation->GetCurrentFrame());
		}
	}

	

	return true;
}

void Merchant::OnCollision(PhysBody* physA, PhysBody* physB) {

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