#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Pathfinding.h"
#include "Physics.h"
#include "Particle.h"
#include "EntityManager.h"
#include "Candy.h"
#include "MainMenu.h"
#include "WinMenu.h"
#include "FadeToBlack.h"


 

Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
	
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: TODO 2: Initialize Player parameters
	initPos = b2Vec2_zero;
	initPos.x = 25;
	initPos.y = 350;
	position = Vector2D(initPos.x, initPos.y);
	renderable = false;

	return true;
}

bool Player::Start() {

	//L03: TODO 2: Initialize Player parameters
	
	renderable = true;
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	t_texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("t_texture").as_string());
	t_texW = parameters.attribute("t_w").as_int();
	t_texH = parameters.attribute("t_h").as_int();
	

	idle.LoadAnimations(parameters.child("animations").child("idle"));
	walk.LoadAnimations(parameters.child("animations").child("walk"));
	jump.LoadAnimations(parameters.child("animations").child("jump"));
	fall.LoadAnimations(parameters.child("animations").child("fall"));
	hurt.LoadAnimations(parameters.child("animations").child("hurt"));
	death.LoadAnimations(parameters.child("animations").child("death"));

	t_idle.LoadAnimations(parameters.child("animations").child("t_idle"));
	t_walk.LoadAnimations(parameters.child("animations").child("t_walk"));
	t_jump.LoadAnimations(parameters.child("animations").child("t_jump"));
	t_fall.LoadAnimations(parameters.child("animations").child("t_fall"));
	t_spell1.LoadAnimations(parameters.child("animations").child("t_spell1"));
	t_spell2.LoadAnimations(parameters.child("animations").child("t_spell2"));
	t_hurt.LoadAnimations(parameters.child("animations").child("t_hurt"));
	t_death.LoadAnimations(parameters.child("animations").child("t_death"));

	jumpForce = parameters.child("propierties").attribute("gJumpForce").as_float();
	pushForce = parameters.child("propierties").attribute("pushForce").as_float();
	moveSpeed = parameters.child("propierties").attribute("moveSpeed").as_float();
	friction = parameters.child("propierties").attribute("friction").as_float();
	gravity = parameters.child("propierties").attribute("gravity").as_float();
	hurtTime = parameters.child("propierties").attribute("hurtTime").as_float();
	attack1Time = parameters.child("propierties").attribute("attack1Time").as_float();
	attack2Time = parameters.child("propierties").attribute("attack2Time").as_float();
	respawnTime = parameters.child("propierties").attribute("respawnTime").as_float();
	playerState = (state)parameters.child("propierties").attribute("playerState").as_int();
	dir = (Direction)parameters.child("propierties").attribute("direction").as_int();
	lives = parameters.attribute("lives").as_int();

	destroyed = false;
	godMode = false;
	canClimb = false;
	transformed = false;
	reachedCheckPoint = false;
	shot = false;
	won = false;

	pickedCandies = 0;

	pugi::xml_document baseFile;
	pugi::xml_parse_result result = baseFile.load_file("config.xml");

	gJumpSFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("gJumpSFX").attribute("path").as_string());
	pJumpSFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("pJumpSFX").attribute("path").as_string());
	gHurtSFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("gHurtSFX").attribute("path").as_string());
	pHurtSFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("pHurtSFX").attribute("path").as_string());
	gDeathSFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("gDeathSFX").attribute("path").as_string());
	pDeathSFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("pDeathSFX").attribute("path").as_string());
	atk1SFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("atk1SFX").attribute("path").as_string());
	atk2SFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("atk2SFX").attribute("path").as_string());
	pickCandySFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("pickCandy").attribute("path").as_string());
	pourCandySFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("pourCandy").attribute("path").as_string());
	eatCandySFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("eatCandy").attribute("path").as_string());
	switchOnSFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("switchOnSFX").attribute("path").as_string());
	switchOffSFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("switchOffSFX").attribute("path").as_string());
	saveGameSFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("saveGame").attribute("path").as_string());
	loadGameSFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("loadGame").attribute("path").as_string());
	godModeSFX = Engine::GetInstance().audio.get()->LoadFx(baseFile.child("config").child("audio").child("fx").child("godModeSFX").attribute("path").as_string());

	currentAnim = &idle;

	int level = Engine::GetInstance().scene.get()->GetLevel();

	std::string nodeChar = "lvl" + std::to_string(level);
	position.setX(parameters.child("startPositions").child(nodeChar.c_str()).attribute("x").as_float());
	position.setY(parameters.child("startPositions").child(nodeChar.c_str()).attribute("y").as_float());

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), GHOST_W, bodyType::DYNAMIC);

	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	pbody->body->SetLinearDamping(friction);
	pbody->body->SetGravityScale(gravity);
	
	//ATTACK COLLIDER
	ATKcolliderW = 35;
	ATKcolliderH = 27;
	attackCollider = Engine::GetInstance().physics.get()->CreateRectangleSensor((int)position.getX(), (int)position.getY(), ATKcolliderW, ATKcolliderH, bodyType::DYNAMIC);
	attackCollider->ctype = ColliderType::WEAPON;
	attackCollider->body->SetEnabled(false);
	attackCollider->body->SetGravityScale(0);
	weaponOffset = { 10, 7 };
	
	//SHOOT
	shoot = (Particle*)Engine::GetInstance().entityManager.get()->CreateEntity(EntityType::SHOT);
	shoot->Start();
	shoot->Awake();
	shootCooldown = parameters.child("propierties").attribute("shootCooldown").as_float();;
	shootCooldownTimer.Start();
	
	hurtTimer = Timer();
	respawnTimer = Timer();


	return true;
}

void Player::Restart()
{
	if (active) {
		playerState = (state)parameters.child("propierties").attribute("playerState").as_int();
		dir = (Direction)parameters.child("propierties").attribute("direction").as_int();
		lives = parameters.attribute("lives").as_int();
		SetPosition({ 
			parameters.child("startPositions").child(Engine::GetInstance().scene.get()->GetCurrentLevelString().c_str()).attribute("x").as_float(),
			parameters.child("startPositions").child(Engine::GetInstance().scene.get()->GetCurrentLevelString().c_str()).attribute("y").as_float()
		});
	}
	
}

bool Player::Update(float dt)
{
	//FRUSTRUM
	if (!Engine::GetInstance().render.get()->InCameraView(pbody->GetPosition().getX() - texW, pbody->GetPosition().getY() - texH, texW, texH))
	{
		return true;
	}

	if (!Engine::GetInstance().scene.get()->paused) {
		//ATTACK COLLIDERS
		shoot->Update(dt);
		if (playerState != ATTACK1) {
			attackCollider->body->SetEnabled(false);
		}

		pbody->body->SetAwake(true);

		currentFrame = currentAnim->GetCurrentFrame();

		velocity = b2Vec2_zero;

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		{
			godMode = !godMode;
			if(godMode) Engine::GetInstance().audio.get()->PlayFx(godModeSFX);
			pbody->body->SetGravityScale(godMode == true || canClimb == true || playerState == DEAD ? 0 : gravity);
			pbody->body->SetLinearVelocity(godMode == true ? b2Vec2_zero : pbody->body->GetLinearVelocity());
			LOG("God mode = %d", (int)godMode);
		}

		if (transformable || godMode) {
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Z) == KEY_DOWN) {
				if (!transformed) {
					transformed = true;
					jumpForce = parameters.child("propierties").attribute("pJumpForce").as_float();
					Engine::GetInstance().audio.get()->PlayFx(switchOnSFX);
				}
				else if (transformed) {
					transformed = false;
					jumpForce = parameters.child("propierties").attribute("gJumpForce").as_float();

					Engine::GetInstance().audio.get()->PlayFx(switchOffSFX);
				}
			}
		}

		if (playerState != HURT && playerState != DEAD && playerState != ATTACK1 && playerState != ATTACK2)
		{
			playerState = IDLE;


			// Move left
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				velocity.x = -moveSpeed * 16;
				playerState = WALK;
				dir = LEFT;
			}

			// Move right
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
				velocity.x = moveSpeed * 16;
				playerState = WALK;
				dir = RIGHT;
			}

			if (godMode || canClimb)
			{
				velocity.y = 0;
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
					velocity.y = -moveSpeed * 16;
					playerState = WALK;
				}

				// Move right
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
					velocity.y = moveSpeed * 16;
					playerState = WALK;
				}

			}
			else {
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && VALUE_NEAR_TO_0(pbody->body->GetLinearVelocity().y)) {
					// Apply an initial upward force
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);

					if (transformed) Engine::GetInstance().audio.get()->PlayFx(pJumpSFX, 0, 6);
					else Engine::GetInstance().audio.get()->PlayFx(gJumpSFX, 0, 6);


				}




				velocity = { velocity.x, pbody->body->GetLinearVelocity().y };
			}

			pbody->body->SetLinearVelocity(velocity);

			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_X) == KEY_DOWN && transformed && playerState != ATTACK1 && VALUE_NEAR_TO_0(pbody->body->GetLinearVelocity().y)) {


				playerState = ATTACK1;
				Engine::GetInstance().audio.get()->PlayFx(atk1SFX);
				attack1Timer.Start();
				pbody->body->SetLinearVelocity({ 0, velocity.y });
				b2Vec2 attackColliderPos = { pbody->body->GetPosition().x + (dir == LEFT ? -PIXEL_TO_METERS(weaponOffset.getX()) : PIXEL_TO_METERS(weaponOffset.getX())), pbody->body->GetPosition().y - PIXEL_TO_METERS(weaponOffset.getY()) };
				attackCollider->body->SetTransform(attackColliderPos, 0);
				attackCollider->body->SetEnabled(true);
			}


			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_C) == KEY_DOWN && transformed && playerState != ATTACK2 && shootCooldownTimer.ReadSec() > shootCooldown && VALUE_NEAR_TO_0(pbody->body->GetLinearVelocity().y)) {

				playerState = ATTACK2;
				attack2Timer.Start();
				pbody->body->SetLinearVelocity({ 0, velocity.y });
				Engine::GetInstance().audio.get()->PlayFx(atk2SFX);
				shot = true;
				shootCooldownTimer.Start();
			}

			if (pbody->body->GetLinearVelocity().y < -0.0001)
			{
				playerState = JUMP;
			}

			if (pbody->body->GetLinearVelocity().y > 0.0001)
			{
				playerState = FALL;
			}



		}
		else if (playerState == ATTACK1) {

			b2Vec2 attackColliderPos = { pbody->body->GetPosition().x + (dir == LEFT ? -PIXEL_TO_METERS(weaponOffset.getX()) : PIXEL_TO_METERS(weaponOffset.getX())), pbody->body->GetPosition().y - PIXEL_TO_METERS(weaponOffset.getY()) };
			attackCollider->body->SetTransform(attackColliderPos, 0);
			if (attack1Timer.ReadSec() >= attack1Time) {

				playerState = IDLE;
				t_spell1.Reset();

			}
			if (attack1Timer.ReadSec() >= 0.4) {
				attackCollider->body->SetEnabled(false);
			}

		}
		else if (playerState == ATTACK2) {

			if (shot) {
				shoot->Restart({ pbody->GetPhysBodyWorldPosition().getX(), pbody->GetPhysBodyWorldPosition().getY() }, { (float)(dir == LEFT ? -1 : 1), (float)0 });
				shot = false;
				t_spell2.Reset();
			}
			if (attack2Timer.ReadSec() >= attack2Time) {

				playerState = IDLE;
				t_spell2.Reset();
			}
		}
		else if (playerState == HURT) {

			if (hurtTimer.ReadSec() >= hurtTime) {
				playerState = IDLE;
				hurt.Reset();
				t_hurt.Reset();
			}
			else
			{
				velocity = pbody->body->GetLinearVelocity();
				pbody->body->SetLinearVelocity(velocity);
			}
		}
		else if (playerState == DEAD) {

			pbody->body->SetLinearVelocity(b2Vec2_zero);
		}
	}
	

	
	if (transformed) {
		switch (playerState) {
		case IDLE:
			
			currentAnim = &t_idle;
			break;
		case WALK:
			
			currentAnim = &t_walk;
			break;
		case JUMP:
			
			currentAnim = &t_jump;
			break;
		case FALL:
			
			currentAnim = &t_fall;
			break;
		case ATTACK1:
			
			currentAnim = &t_spell1;
			break;

		case ATTACK2:
			currentAnim = &t_spell2;
			break;

		case HURT:
			hurt.Reset();
			currentAnim = &t_hurt;
			break;
		case DEAD:
			
			currentAnim = &t_death;
			break;
		}
	}
	else {
		switch (playerState) {
		case IDLE:
			currentAnim = &idle;
			break;
		case WALK:
			currentAnim = &walk;
			break;
		case JUMP:
			currentAnim = &jump;
			break;
		case FALL:
			currentAnim = &fall;
			break;
		case HURT:
			currentAnim = &hurt;
			break;
		case DEAD:
			currentAnim = &death;
			break;
		}
	}

	b2Transform pbodyPos = pbody->body->GetTransform();

	if (renderable) {
		if (!transformed) {


			position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
			position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

			if (dir == RIGHT) {
				Engine::GetInstance().render.get()->DrawTexture(texture, position.getX(), position.getY(), &currentFrame);

			}
			else if (dir == LEFT) {
				Engine::GetInstance().render.get()->DrawTextureFlipped(texture, position.getX(), position.getY(), &currentFrame);
			}
		}
		else {

			position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - t_texW / 2);
			position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - t_texH / 2);

			if (dir == RIGHT) {

				Engine::GetInstance().render.get()->DrawTexture(t_texture, position.getX(), position.getY(), &currentFrame);
			}
			else if (dir == LEFT) {

				Engine::GetInstance().render.get()->DrawTextureFlipped(t_texture, position.getX(), position.getY(), &currentFrame);
			}
		}
	}
	

	currentAnim->Update();
	
	return true;
}

bool Player::PostUpdate(float dt) {
	
	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");

	active = false;

	
	return true;
}

// L08 TODO 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;
	case ColliderType::PUMPKIN:
		LOG("Collision PUMPKIN");
		transformable = true;
		break;
	case ColliderType::SPYKE:

		DMGPlayer(physA, physB);
			
		LOG("Collision SPYKE");
			
		break;

	case ColliderType::ENEMY:
		LOG("Collision ENEMY");
		break;
	
	case ColliderType::ABYSS:
	{ 
		if (!godMode) {
			playerState = DEAD;
			lives = 0;

			if (transformed) Engine::GetInstance().audio.get()->PlayFx(pDeathSFX);
			else Engine::GetInstance().audio.get()->PlayFx(gDeathSFX);

			pbody->body->SetGravityScale(0);
			pbody->body->SetGravityScale(0);
			respawnTimer.Start();
		}
		LOG("Collision ABYSS");
		break;
	}
	case ColliderType::LADDER:
		canClimb = true;
		pbody->body->SetGravityScale(0);
		LOG("Collision LADDER");
		break;
	case ColliderType::CHECKPOINT:
		reachedCheckPoint = true;
		Engine::GetInstance().audio.get()->PlayFx(saveGameSFX);
		Engine::GetInstance().scene.get()->SaveState();
		break;
	case ColliderType::LEVEL_CHANGER:
		if (Engine::GetInstance().scene.get()->level == LVL1) {
			Engine::GetInstance().scene.get()->ChangeLevel();
		}
		else if (Engine::GetInstance().scene.get()->level == LVL2) {
			won = true;
		}
		break;
	
	case ColliderType::BOSS_STARTER:
		if (!Engine::GetInstance().scene.get()->GetStartBossFight())Engine::GetInstance().scene.get()->SetStartBossFight(true);
		break;
	
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	
	default:
		break;
	}
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::PUMPKIN:
		transformable = false;
		LOG("End Collision PUMPKIN");
		break;
	case ColliderType::LADDER:
		LOG("End Collision LADDER");
		canClimb = false;
		pbody->body->SetGravityScale(godMode == true || canClimb == true || playerState == DEAD ? 0 : gravity);
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	default:
		break;
	}
}


void Player::SetPosition(Vector2D pos) {
	
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);	
	
}

void Player::SaveData(pugi::xml_node playerNode)
{
	if (active) {
		playerNode.attribute("x").set_value(pbody->GetPhysBodyWorldPosition().getX());
		playerNode.attribute("y").set_value(pbody->GetPhysBodyWorldPosition().getY());
		playerNode.attribute("lives").set_value(lives);
		playerNode.attribute("transformed").set_value(transformed);
		playerNode.attribute("candies").set_value(pickedCandies);
	}
	
}


void Player::LoadData(pugi::xml_node playerNode)
{
	position.setX(playerNode.attribute("x").as_int());
	position.setY(playerNode.attribute("y").as_int());
	lives = playerNode.attribute("lives").as_int();
	transformed = playerNode.attribute("transformed").as_bool();
	pickedCandies = playerNode.attribute("candies").as_int();
	if (transformed) jumpForce = parameters.child("propierties").attribute("pJumpForce").as_float();
	else jumpForce = parameters.child("propierties").attribute("gJumpForce").as_float();
	SetPosition(position);
}

void Player::DMGPlayer(PhysBody* physA, PhysBody* physB) {

	if (playerState != DEAD && !godMode && playerState != HURT) {

		lives--;
		if (lives <= 0) {
			playerState = DEAD;

			if (transformed) Engine::GetInstance().audio.get()->PlayFx(pDeathSFX);
			else Engine::GetInstance().audio.get()->PlayFx(gDeathSFX);

			pbody->body->SetGravityScale(0);
			respawnTimer.Start();
		}
		else
		{

			hurtTimer.Start();
			playerState = HURT;
			pbody->body->SetGravityScale(godMode == true || canClimb == true || playerState == DEAD ? 0 : gravity);

			if (transformed) Engine::GetInstance().audio.get()->PlayFx(pHurtSFX);
			else Engine::GetInstance().audio.get()->PlayFx(gHurtSFX);
				
			pushDir = b2Vec2_zero;
			pushDir.x = physA->body->GetPosition().x - physB->body->GetPosition().x;
			pushDir.y = physA->body->GetPosition().y - physB->body->GetPosition().y;

			pushDir.Normalize();

			physA->body->SetLinearVelocity(b2Vec2_zero);
			physA->body->ApplyLinearImpulseToCenter(b2Vec2(pushForce * pushDir.x, pushForce * pushDir.y), true);
		}
	}
}
void Player::KillPlayer() {

	lives = 0;
	playerState = DEAD;

	if (transformed) Engine::GetInstance().audio.get()->PlayFx(pDeathSFX);
	else Engine::GetInstance().audio.get()->PlayFx(gDeathSFX);

	pbody->body->SetGravityScale(0);
	respawnTimer.Start();
}

void Player::SetCandies(int candies)
{
	pickedCandies = candies;
}

void Player::SetLives(int l)
{
	lives = l;
}

void Player::PickCandies()
{
	pickedCandies++;
	candySFX = eatCandySFX;
	Engine::GetInstance().audio.get()->PlayFx(candySFX);
}

void Player::Heal()
{
	lives++;
	candySFX = loadGameSFX;
	Engine::GetInstance().audio.get()->PlayFx(candySFX);
}