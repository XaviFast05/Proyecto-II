#include "BatEnemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Physics.h"
#include "Scene.h"
#include "Player.h"
#include "LOG.h"
#include "Audio.h"
#include "tracy/Tracy.hpp"

BatEnemy::BatEnemy()
{

}

BatEnemy::~BatEnemy() {

}

bool BatEnemy::Start() {
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_float());
	position.setY(parameters.attribute("y").as_float());
	texW = parameters.attribute("w").as_float();
	texH = parameters.attribute("h").as_float();
	drawOffsetX = 0;
	drawOffsetY = 0;


	idle.LoadAnimations(parameters.child("animations").child("idle"));
	attack.LoadAnimations(parameters.child("animations").child("attack"));
	hurt.LoadAnimations(parameters.child("animations").child("hurt"));
	death.LoadAnimations(parameters.child("animations").child("death"));
	


	currentAnimation = &idle;

	//this is meant to be in Enemy.cpp as both enemies use it, but it doesn't work for some reason
	pugi::xml_document audioFile;
	pugi::xml_parse_result result = audioFile.load_file("config.xml");
	audioNode = audioFile.child("config").child("audio").child("fx");

	//SFX LOAD
	batWingsSFX = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("batWingsSFX").attribute("path").as_string());
	farBatWingsSFX = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("farBatWings").attribute("path").as_string());
	batDeathSFX = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("batDeathSFX").attribute("path").as_string());
	noSound = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("noSound").attribute("path").as_string());
	

	//INIT ROUTE
	for (int i = 0; i < route.size(); i++)
	{
		route[i] = Engine::GetInstance().map.get()->WorldToWorldCenteredOnTile(route[i].getX(), route[i].getY());
	}
	routeDestinationIndex = 0;
	destinationPoint = route[routeDestinationIndex];

	//INIT PHYSICS
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), 16, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetGravityScale(0);
	pbody->body->SetFixedRotation(true);
	pbody->body->SetTransform({ PIXEL_TO_METERS(destinationPoint.getX()), PIXEL_TO_METERS(destinationPoint.getY()) }, 0);
	pbody->listener = this;

	//INIT PATH
	pathfinding = new Pathfinding();
	ResetPath();
	
	//INIT VARIABLES
	speed = parameters.child("properties").attribute("speed").as_float();
	chaseArea = parameters.child("properties").attribute("chaseArea").as_float();
	deathTime = parameters.child("properties").attribute("deathTime").as_float();

	pushForce = parameters.child("properties").attribute("pushForce").as_float();
	pushFriction = parameters.child("properties").attribute("pushFriction").as_float();
	lootAmount = parameters.child("properties").attribute("lootAmount").as_float();
	droppedLoot = parameters.child("properties").attribute("droppedLoot").as_bool();
	state = PATROL;

	playingSound = false;
	return true;
}

bool BatEnemy::Update(float dt) {
	ZoneScoped;

	if (!dead) {

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
			pbody->body->SetGravityScale(0);
			//STATES CHANGERS
			if (state != DEAD) {
				if (pbody->GetPhysBodyWorldPosition().distanceEuclidean(player->pbody->GetPhysBodyWorldPosition()) > chaseArea && state != PATROL)
				{
					state = PATROL;
					ResetPath();
					destinationPoint = route[routeDestinationIndex];

				}
				else if (pbody->GetPhysBodyWorldPosition().distanceEuclidean(player->pbody->GetPhysBodyWorldPosition()) <= chaseArea && state != CHASING)
				{
					state = CHASING;
					ResetPath();
				}
			}

			//STATES CONTROLER

			if (state == PATROL) {

				if (CheckIfTwoPointsNear(destinationPoint, { (float)METERS_TO_PIXELS(pbody->body->GetPosition().x), (float)METERS_TO_PIXELS(pbody->body->GetPosition().y) }, 5))
				{
					routeDestinationIndex++;
					if (routeDestinationIndex == route.size()) routeDestinationIndex = 0;
					destinationPoint = route[routeDestinationIndex];
					ResetPath();
				}
			}
			else if (state == CHASING) {

				Vector2D playerPos = player->pbody->GetPhysBodyWorldPosition();
				Vector2D playerPosCenteredOnTile = Engine::GetInstance().map.get()->WorldToWorldCenteredOnTile(playerPos.getX(), playerPos.getY());
				if (destinationPoint != playerPosCenteredOnTile)
				{
					destinationPoint = playerPosCenteredOnTile;
					ResetPath();
				}
			}
			else if (state == DEAD) {
				pbody->body->SetGravityScale(1);

				if (deathTimer.ReadSec() > deathTime && !dead) {
					pbody->body->SetEnabled(false);
					dead = true;
				}
			}

			//PATHFINDING CONTROLER
			if (state == PATROL || state == CHASING) {

				if (pbody->GetPhysBodyWorldPosition().distanceEuclidean(player->pbody->GetPhysBodyWorldPosition()) <= (float)chaseArea * 1.5f && !playingSound) {
					Engine::GetInstance().audio.get()->PlayFx(farBatWingsSFX, 1);
					playingSound = true;
				}
				else if (pbody->GetPhysBodyWorldPosition().distanceEuclidean(player->pbody->GetPhysBodyWorldPosition()) >= chaseArea * 1.5f && playingSound) {
					Engine::GetInstance().audio.get()->PlayFx(noSound, 1);
					playingSound = false;
				}



				if (pathfinding->pathTiles.empty())
				{
					while (pathfinding->pathTiles.empty())
					{
						pathfinding->PropagateAStar(SQUARED, destinationPoint);
					}
					pathfinding->pathTiles.pop_back();
				}
				else
				{

					Vector2D nextTile = pathfinding->pathTiles.back();
					Vector2D nextTileWorld = Engine::GetInstance().map.get()->MapToWorldCentered(nextTile.getX(), nextTile.getY());


					if (CheckIfTwoPointsNear(nextTileWorld, { (float)METERS_TO_PIXELS(pbody->body->GetPosition().x), (float)METERS_TO_PIXELS(pbody->body->GetPosition().y) }, 3)) {

						pathfinding->pathTiles.pop_back();
						if (pathfinding->pathTiles.empty()) ResetPath();
					}
					else {
						Vector2D nextTilePhysics = { PIXEL_TO_METERS(nextTileWorld.getX()),PIXEL_TO_METERS(nextTileWorld.getY()) };
						b2Vec2 direction = { nextTilePhysics.getX() - pbody->body->GetPosition().x, nextTilePhysics.getY() - pbody->body->GetPosition().y };
						direction.Normalize();
						pbody->body->SetLinearVelocity({ direction.x * speed, direction.y * speed });
					}
				}
			}



			
		}
		else {
			
			pbody->body->SetLinearVelocity({0,0});
		}
		
		
		if (pbody->body->GetLinearVelocity().x > 0.2f) {
			dir = RIGHT;
		}
		else if (pbody->body->GetLinearVelocity().x < -0.2f) {
			dir = LEFT;
		}

		switch (state) {
			break;
		case CHASING:
			currentAnimation = &attack;
			break;
		case PATROL:
			currentAnimation = &idle;
			break;
		case ATTACK:
			currentAnimation = &attack;
			break;
		case DEAD:
			currentAnimation = &death;
			break;
		default:
			break;
		}

		if (state == DEAD && !droppedLoot) {
			DropLoot();
			pbody->ctype = ColliderType::DEADENEMY;
			droppedLoot = true;
		}

		//DRAW

		if (pbody->body->IsEnabled()) {
			if (Engine::GetInstance().GetDebug())
			{
				Engine::GetInstance().render.get()->DrawCircle(position.getX() + texW / 2, position.getY() + texH / 2, chaseArea * 2, 255, 255, 255);
				Engine::GetInstance().render.get()->DrawCircle(destinationPoint.getX(), destinationPoint.getY(), 3, 255, 0, 0);
				pathfinding->DrawPath();
			}

			currentAnimation->Update();

			b2Transform pbodyPos = pbody->body->GetTransform();
			position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2 + drawOffsetX);
			position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2 + drawOffsetY);
			if (dir == LEFT) {
				Engine::GetInstance().render.get()->DrawTextureFlipped(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
			}
			else if (dir == RIGHT) {
				Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
			}
		}
	
	}

	

	return true;
}

void BatEnemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	bool push = false;

	switch (physB->ctype) {
	case ColliderType::WEAPON:
		break;
	case ColliderType::PICKAXE:
		if (state != DEAD) 	DMGEnemyPickaxe();
		break;
	case ColliderType::MELEE_AREA:
		if (state != DEAD) {
			if (canPush) push = true;
			DMGEnemyMelee();
		}
		break;
	case ColliderType::SPYKE:
		break;
	case ColliderType::ENEMY:
		break;
	case ColliderType::ABYSS:
		break;
	case ColliderType::UNKNOWN:
		break;
	default:
		break;
	}
	if (push) {
		//PUSHING THE ENEMY WHEN HURT
		b2Vec2 pushVec((physA->body->GetPosition().x - player->pbody->body->GetPosition().x),
			(physA->body->GetPosition().y - player->pbody->body->GetPosition().y));
		pushVec.Normalize();
		pushVec *= pushForce;

		pbody->body->SetLinearVelocity(b2Vec2_zero);
		pbody->body->ApplyLinearImpulseToCenter(pushVec, true);
		pbody->body->SetLinearDamping(pushFriction);
	}
}