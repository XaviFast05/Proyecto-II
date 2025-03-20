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


	jumpForce = parameters.child("propierties").attribute("gJumpForce").as_float();
	pushForce = parameters.child("propierties").attribute("pushForce").as_float();
	moveSpeed = parameters.child("propierties").attribute("moveSpeed").as_float();
	friction = parameters.child("propierties").attribute("friction").as_float();
	gravity = parameters.child("propierties").attribute("gravity").as_float();
	hurtTime = parameters.child("propierties").attribute("hurtTime").as_float();
	respawnTime = parameters.child("propierties").attribute("respawnTime").as_float();
	playerState = (state)parameters.child("propierties").attribute("playerState").as_int();
	dir = (Direction)parameters.child("propierties").attribute("direction").as_int();

	godMode = false;
	canClimb = false;
	reachedCheckPoint = false;

	pugi::xml_document baseFile;
	pugi::xml_parse_result result = baseFile.load_file("config.xml");

	
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

	
	hurtTimer = Timer();
	respawnTimer = Timer();


	return true;
}

void Player::Restart()
{
	if (active) {
		playerState = (state)parameters.child("propierties").attribute("playerState").as_int();
		dir = (Direction)parameters.child("propierties").attribute("direction").as_int();
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

		pbody->body->SetAwake(true);

		currentFrame = currentAnim->GetCurrentFrame();

		velocity = b2Vec2_zero;

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		{
			godMode = !godMode;
			pbody->body->SetGravityScale(godMode == true || canClimb == true || playerState == DEAD ? 0 : gravity);
			pbody->body->SetLinearVelocity(godMode == true ? b2Vec2_zero : pbody->body->GetLinearVelocity());
			LOG("God mode = %d", (int)godMode);
		}

		if (godMode) {
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Z) == KEY_DOWN) {

					jumpForce = parameters.child("propierties").attribute("gJumpForce").as_float();
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
				}




				velocity = { velocity.x, pbody->body->GetLinearVelocity().y };
			}

			pbody->body->SetLinearVelocity(velocity);

			if (pbody->body->GetLinearVelocity().y < -0.0001)
			{
				playerState = JUMP;
			}

			if (pbody->body->GetLinearVelocity().y > 0.0001)
			{
				playerState = FALL;
			}



		}

		else if (playerState == HURT) {

			if (hurtTimer.ReadSec() >= hurtTime) {
				playerState = IDLE;
				hurt.Reset();
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

	b2Transform pbodyPos = pbody->body->GetTransform();

	if (renderable) {

		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

		if (dir == RIGHT) {
			Engine::GetInstance().render.get()->DrawTexture(texture, position.getX(), position.getY(), &currentFrame);

		}
		else if (dir == LEFT) {
			Engine::GetInstance().render.get()->DrawTextureFlipped(texture, position.getX(), position.getY(), &currentFrame);
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
		break;
	case ColliderType::SPYKE:
			
		LOG("Collision SPYKE");
			
		break;

	case ColliderType::ENEMY:
		LOG("Collision ENEMY");
		break;
	
	case ColliderType::ABYSS:
	{ 
		if (!godMode) {
			playerState = DEAD;

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
		Engine::GetInstance().scene.get()->SaveState();
		break;
	case ColliderType::LEVEL_CHANGER:
		if (Engine::GetInstance().scene.get()->level == LVL1) {
			Engine::GetInstance().scene.get()->ChangeLevel();
		}
		else if (Engine::GetInstance().scene.get()->level == LVL2) {
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
	}
}


void Player::LoadData(pugi::xml_node playerNode)
{
	position.setX(playerNode.attribute("x").as_int());
	position.setY(playerNode.attribute("y").as_int());
	SetPosition(position);
}

void Player::KillPlayer() {

	playerState = DEAD;

	pbody->body->SetGravityScale(0);
	respawnTimer.Start();
}

