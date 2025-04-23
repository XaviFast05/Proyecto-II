#include "GroundEnemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Physics.h"
#include "Scene.h"
#include "Player.h"
#include "LOG.h"
#include "tracy/Tracy.hpp"

GroundEnemy::GroundEnemy()
{

}

GroundEnemy::~GroundEnemy() {

}


bool GroundEnemy::Start() {

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
	

	AddAnimation(attack, 48, texW, 4);
	AddAnimation(attack, 0, texW, 1);
	attack.speed = 0.1f;
	attack.loop = false;

	AddAnimation(death, 96, texW, 3);
	death.speed = 0.2f;
	death.loop = false;

	idle.LoadAnimations(parameters.child("animations").child("idle"));
	walk.LoadAnimations(parameters.child("animations").child("walk"));
	attack.LoadAnimations(parameters.child("animations").child("attack"));
	death.LoadAnimations(parameters.child("animations").child("death"));

	currentAnimation = &idle;
	attacked = false;

	//INIT ROUTE
	for (int i = 0; i < route.size(); i++)
	{
		route[i] = Engine::GetInstance().map.get()->WorldToWorldCenteredOnTile(route[i].getX(), route[i].getY());
	}
	routeDestinationIndex = 0;
	destinationPoint = route[routeDestinationIndex];

	//INIT PHYSICS
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(),  32/4, bodyType::DYNAMIC);
	pbody->ctype = ColliderType::ENEMY;
	pbody->body->SetGravityScale(1.2f);
	pbody->body->SetFixedRotation(true);
	pbody->listener = this;
	pbody->body->SetTransform({ PIXEL_TO_METERS(destinationPoint.getX()), PIXEL_TO_METERS(destinationPoint.getY()) }, 0);

	//INIT PATH
	pathfinding = new Pathfinding();
	ResetPath();

	//INIT VARIABLES
	state = PATROL;
	speed = parameters.child("properties").attribute("speed").as_float();
	chaseArea = parameters.child("properties").attribute("chaseArea").as_float();
	attackArea = parameters.child("properties").attribute("attackArea").as_float();
	jumpForce = parameters.child("properties").attribute("jumpForce").as_float();
	attackTime = parameters.child("properties").attribute("attackTime").as_float();
	deathTime = parameters.child("properties").attribute("deathTime").as_float();

	pushForce = parameters.child("properties").attribute("pushForce").as_float();
	pushFriction = parameters.child("properties").attribute("pushFriction").as_float();
	lootAmount = parameters.child("properties").attribute("lootAmount").as_float();
	droppedLoot = parameters.child("properties").attribute("droppedLoot").as_bool();
	dir = LEFT;

	//LOAD SFX
	pugi::xml_document audioFile;
	pugi::xml_parse_result result = audioFile.load_file("config.xml");
	audioNode = audioFile.child("config").child("audio").child("fx");

	swordSlashSFX = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("swordSFX").attribute("path").as_string());
	skeletonDeathSFX = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("skeletonDeathSFX").attribute("path").as_string());

	return true;
}

bool GroundEnemy::Update(float dt) {

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
			dist = pbody->GetPhysBodyWorldPosition().distanceEuclidean(player->pbody->GetPhysBodyWorldPosition());

			//STATES CHANGERS
			if (state != ATTACK && state != DEAD)
			{
				if (dist > chaseArea && state != PATROL)
				{
					state = PATROL;
					ResetPath();
					destinationPoint = route[routeDestinationIndex];
				}
				else if (dist <= chaseArea/* && state != CHASING*/)
				{
					if (dist <= attackArea && state != ATTACK) {
						state = ATTACK;
						Engine::GetInstance().audio.get()->PlayFx(swordSlashSFX, 0, -1);
						attackTimer.Start();
						pbody->body->SetLinearVelocity({ 0,0 });
						attack.Reset();
					}
					else if (state != CHASING)
					{
						state = CHASING;
						ResetPath();
					}
				}
			}



			Vector2D playerPos = player->pbody->GetPhysBodyWorldPosition();
			Vector2D playerPosCenteredOnTile = Engine::GetInstance().map.get()->WorldToWorldCenteredOnTile(playerPos.getX(), playerPos.getY());

			//STATES CONTROLER
			if (state == DEAD) {

				if (deathTimer.ReadSec() > deathTime) {

					pbody->body->SetEnabled(false);
					dead = true;
				}

			}

			else if (state == PATROL) {

				Vector2D physPos = pbody->GetPhysBodyWorldPosition();
				if (CheckIfTwoPointsNear(destinationPoint, { physPos.getX(), physPos.getY() }, 7))
				{
					routeDestinationIndex++;
					if (routeDestinationIndex == route.size()) routeDestinationIndex = 0;
					destinationPoint = route[routeDestinationIndex];
					ResetPath();
				}
			}
			else if (state == CHASING) {

				if (destinationPoint != playerPosCenteredOnTile)
				{
					destinationPoint = playerPosCenteredOnTile;
					ResetPath();
				}
			}
			else if (state == ATTACK) {

				if (attackTimer.ReadSec() > attackTime) {

					state = PATROL;
				}
			}

			//PATHFINDING CONTROLER
			if (state == PATROL || state == CHASING)
			{
				if (pathfinding->pathTiles.empty())
				{
					while (pathfinding->pathTiles.empty())
					{
						pathfinding->PropagateAStar(SQUARED, destinationPoint, Pathfinding::WALK);

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
						pbody->body->SetLinearVelocity({ direction.x * speed, pbody->body->GetLinearVelocity().y });
					}
				}
				Vector2D currentTile = Engine::GetInstance().map.get()->WorldToMap(pbody->GetPhysBodyWorldPosition().getX(), pbody->GetPhysBodyWorldPosition().getY());

				if (pathfinding->IsJumpable(currentTile.getX(), currentTile.getY()) && VALUE_NEAR_TO_0(pbody->body->GetLinearVelocity().LengthSquared()))
				{
					pbody->body->ApplyLinearImpulseToCenter({ 0, -jumpForce }, true);
				}
			}

		} 
		else 
		{
			pbody->body->SetLinearVelocity({ 0,0 });

		}
	

		switch (state) {
			break;
		case CHASING:
			currentAnimation = &walk;
			break;
		case PATROL:
			currentAnimation = &walk;
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

		//DIRECTION
		if (pbody->body->GetLinearVelocity().x > 0.2f) {
			dir = RIGHT;
		}
		else if (pbody->body->GetLinearVelocity().x < -0.2f) {
			dir = LEFT;
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
			position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 1.5 + drawOffsetY);



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

void GroundEnemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	bool push = false;

	switch (physB->ctype) {
	case ColliderType::WEAPON:
		break;
	case ColliderType::SHOT:
		if (state != DEAD) 	DMGEnemy();
		break;
	case ColliderType::MELEE_AREA:
		if (state != DEAD) {
			if (canPush) push = true;
			DMGEnemy();
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