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
#include "Bullet.h"
#include "PickaxeManager.h"

 

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

	for (pugi::xml_node stateNode : parameters.child("statesFlow").children())
	{
		std::vector<bool> flow;
		flow.clear();
		for (pugi::xml_attribute stateAttribute : stateNode.attributes())
		{
			flow.push_back(stateAttribute.as_bool());
		}
		stateFlow.push_back(flow);

		
	}

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

	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX(), (int)position.getY(), texW / 3, texH, bodyType::DYNAMIC);

	pbody->listener = this;
	pbody->ctype = ColliderType::PLAYER;
	pbody->body->SetLinearDamping(friction);
	pbody->body->SetGravityScale(gravity);
	pbody->body->SetFixedRotation(true);

	
	hurtTimer = Timer();
	respawnTimer = Timer();

	pickaxeManager = new PickaxeManager();
	pickaxeManager->Start();

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

		velocity = b2Vec2_zero;
		grounded = VALUE_NEAR_TO_0(pbody->body->GetLinearVelocity().y);

		//UPDATE SUBMODULES
		pickaxeManager->Update(dt);

		//GODMODE
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
			godMode = !godMode;
			pbody->body->SetGravityScale(godMode == true || canClimb == true);
			pbody->body->SetLinearVelocity(godMode == true ? b2Vec2_zero : pbody->body->GetLinearVelocity());
			LOG("God mode = %d", (int)godMode);
		}

		if (godMode) {
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Z) == KEY_DOWN) {

				jumpForce = parameters.child("propierties").attribute("gJumpForce").as_float();
			}
		}

		//CHANGERS
		if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) || Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) && stateFlow[playerState][RUN] && grounded) {
			playerState = RUN;
		}
		else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && ((stateFlow[playerState][JUMP] && grounded) /*MODIFICAR GODMODE*/ || godMode)) {
			playerState = JUMP;
			pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
			grounded = false;
			plusJumpTimer.Start();
			plusJumpTimerOn = true;

		}
		else if (pbody->body->GetLinearVelocity().y > 0.001 && stateFlow[playerState][FALL]) {
			if (playerState == RUN) {
				coyoteTimer.Start();
				coyoteTimerOn = true;
			}
			playerState = FALL;
			grounded = false;
		}
		else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_E) && stateFlow[playerState][CHOP] && pickaxeManager->GetNumPickaxes() > 0) {
			stateTimer.Start();
			playerState = CHOP;

			meleeTimer.Start();
			meleeTimerOn = true;
		}
		else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_E) && stateFlow[playerState][PUNCH] && pickaxeManager->GetNumPickaxes() <= 0) {
			stateTimer.Start();
			playerState = PUNCH;

			meleeTimer.Start();
			meleeTimerOn = true;
		}
		else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Q) && stateFlow[playerState][THROW] && pickaxeManager->GetNumPickaxes() > 0) {
			
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_W)) pickaxeManager->ThrowPickaxe({0,1}, pbody->GetPhysBodyWorldPosition());
			else pickaxeManager->ThrowPickaxe(GetDirection(), pbody->GetPhysBodyWorldPosition());

			stateTimer.Start();
			playerState = THROW;
		}

		//COYOTE TIME LOGIC
		if (coyoteTimerOn) {
			if (coyoteTimer.ReadSec() < coyoteTimerMax && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE)) {
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
				playerState = JUMP;
				coyoteTimerOn = false;

				plusJumpTimer.Start();
				plusJumpTimerOn = true;
			}
			if (coyoteTimer.ReadSec() >= coyoteTimerMax) coyoteTimerOn = false;
		}

		//PLUS JUMP LOGIC
		if (plusJumpTimerOn) {
			if (!grounded && playerState == JUMP && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT && plusJumpTimer.ReadSec() < plusJumpTimerMax) {
				float jumpPlusForce = 1;
				pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpPlusForce), true);
			}
			if (plusJumpTimer.ReadSec() >= plusJumpTimerMax) plusJumpTimerOn = false;
		}

		//MELEE ATTACKS LOGIC
		if (meleeTimerOn) {
			if (meleeTimer.ReadSec() == 0.0) {
				meleeDisplace = (dir == RIGHT) ? (2 * texW / 3 + MELEE_AREA_WIDTH / 2) : (texW / 3 - MELEE_AREA_WIDTH / 2);
				meleeArea = Engine::GetInstance().physics.get()->CreateRectangleSensor((position.getX() + meleeDisplace), position.getY() + texH / 2, MELEE_AREA_WIDTH, texH, STATIC);
				meleeArea->ctype = ColliderType::MELEE_AREA;
			}
			if (meleeTimer.ReadSec() < meleeTimerMax) {
				b2Vec2 meleeAreaMovePos = b2Vec2(pbody->body->GetPosition().x + PIXEL_TO_METERS(meleeDisplace) - PIXEL_TO_METERS(texW/2), pbody->body->GetPosition().y);
				meleeArea->body->SetTransform(meleeAreaMovePos, 0);
			}
			if (meleeTimer.ReadSec() >= meleeTimerMax) {
				meleeArea->body->SetEnabled(false);
				meleeTimerOn = false;
			}
		}

		//LOGIC
		switch (playerState) {
		case IDLE:
			break;
		case RUN:
			if (CheckMoveX()) MoveX();
			else playerState = IDLE;
			break;
		case JUMP:
			if (CheckMoveX()) MoveX();
			if (grounded) playerState = IDLE;
			break;
		case FALL:
			if (CheckMoveX()) MoveX();
			if (grounded) playerState = IDLE;
			break;
		case PUNCH:
			if (CheckMoveX()) MoveX();
			if (stateTimer.ReadSec() >= punchTimerAnimation) playerState = IDLE;
			break;
		case CHOP:
			if (CheckMoveX()) MoveX();
			if (stateTimer.ReadSec() >= pickaxeTimerAnimation) playerState = IDLE;
			break;
		case THROW:
			if (CheckMoveX()) MoveX();
			if (stateTimer.ReadSec() >= pickaxeTimerAnimation) playerState = IDLE;
			break;
		default:
			break;
		}

		velocity = { velocity.x, pbody->body->GetLinearVelocity().y };
		pbody->body->SetLinearVelocity(velocity);
	}

	//ANIMS
	currentFrame = currentAnim->GetCurrentFrame();
	
	switch (playerState) {
	case IDLE:
		currentAnim = &idle;
		break;
	case RUN:
		currentAnim = &walk;
		break;
	case JUMP:
		currentAnim = &jump;
		break;
	case FALL:
		currentAnim = &fall;
		break;
	case PUNCH:
		currentAnim = &hurt;
		break;
	case CHOP:
		currentAnim = &hurt;
		break;
	case THROW:
		currentAnim = &hurt;
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
			/*playerState = DEAD;*/

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
		pbody->body->SetGravityScale(godMode == true || canClimb == true /*|| playerState == DEAD ? 0 : gravity*/);
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

Vector2D Player::GetDirection() const {

	if (dir == LEFT) {
		return Vector2D(-1, 0);  // Izquierda
	}
	else {
		return Vector2D(1, 0);   // Derecha
	}
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

	/*playerState = DEAD;*/

	pbody->body->SetGravityScale(0);
	respawnTimer.Start();
}

bool Player::CheckMoveX() {
	if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) || Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT))
	{
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) dir = RIGHT;
		else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) dir = LEFT;

		Engine::GetInstance().scene.get()->cameraDirectionChangeActivation = true;
		return true;
	}
	else return false;
}

void Player::MoveX()
{
	velocity.x = (dir == RIGHT ? moveSpeed * 16 : -moveSpeed * 16);
}

void Player::CheckJump()
{
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) && grounded)
		pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
}