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
#include "MainMenu.h"
#include "WinMenu.h"
#include "FadeToBlack.h"
#include "Bullet.h"
#include "PickaxeManager.h"
#include "CurrencyManager.h"
#include "DialoguesManager.h"
#include "DashParticle.h"

 

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

	idle.LoadAnimations(parameters.child("animations").child("idle"));
	walk.LoadAnimations(parameters.child("animations").child("walk"));
	jump.LoadAnimations(parameters.child("animations").child("jump"));
	fall.LoadAnimations(parameters.child("animations").child("fall"));
	hurt.LoadAnimations(parameters.child("animations").child("hurt"));
	death.LoadAnimations(parameters.child("animations").child("death"));
	punch.LoadAnimations(parameters.child("animations").child("punch"));
	chop.LoadAnimations(parameters.child("animations").child("chop"));
	throwPix.LoadAnimations(parameters.child("animations").child("throwPix"));
	throwPixUp.LoadAnimations(parameters.child("animations").child("throwPixUp"));
	dash.LoadAnimations(parameters.child("animations").child("dash"));
	charged.LoadAnimations(parameters.child("animations").child("charged"));

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

	upgrades.clear();
	unlockedUpgrades.clear();
	godMode = false;
	canClimb = false;
	reachedCheckPoint = false;
	pugi::xml_document baseFile;
	pugi::xml_parse_result result = baseFile.load_file("config.xml");

	audioNode = baseFile.child("config").child("audio").child("fx");

	//SFX LOAD
	playerJumpSFX = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("playerJumpSFX").attribute("path").as_string());
	playerLandSFX = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("playerLandSFX").attribute("path").as_string());
	playerAttack1SFX = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("atk1SFX").attribute("path").as_string());
	playerAttack2SFX = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("atk2SFX").attribute("path").as_string());
	playerThrowSFX = Engine::GetInstance().audio.get()->LoadFx(audioNode.child("throwSFX").attribute("path").as_string());

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
	

	leftSensor = Engine::GetInstance().physics.get()->CreateRectangleSensor(position.getX(), position.getY() + 4 / 2, texH / 6, texH - 2, DYNAMIC);
	leftSensor->ctype = ColliderType::PLAYER_SENSOR;
	leftSensor->listener = this;

	rightSensor = Engine::GetInstance().physics.get()->CreateRectangleSensor(position.getX(), position.getY() + 4 / 2, texH / 6, texH - 2, DYNAMIC);
	rightSensor->ctype = ColliderType::PLAYER_SENSOR;
	rightSensor->listener = this;

	hurtTimer = Timer();
	respawnTimer = Timer();

	projectileManager = new ProjectileManager();
	currencyManager = new CurrencyManager();
	currencyManager->Start();
	dialoguesManager = new DialoguesManager();
	dialoguesManager->Start();

	LoadDefaults();
	projectileManager->Start();

	b2Fixture* fixture = pbody->body->GetFixtureList();
	if (fixture) {
		b2Filter filter = fixture->GetFilterData();
		filter.categoryBits = CATEGORY_PLAYER;
		filter.maskBits = 0xFFFF & ~CATEGORY_PICKAXE;
		fixture->SetFilterData(filter);
	}

	dashParticle = (DashParticle*)(Engine::GetInstance().entityManager->CreateEntity(EntityType::DASH_PARTICLE));

	dashParticle->SetParameters(
		Engine::GetInstance().scene->configParameters);
	dashParticle->Start();
	Vector2D p1(METERS_TO_PIXELS(pbody->body->GetPosition().x), METERS_TO_PIXELS(pbody->body->GetPosition().y));
	dashParticle->SetPosition(p1);

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
		b2Vec2 mainPos = pbody->body->GetPosition();

		//Colocar sensores
		leftSensor->body->SetTransform(
			b2Vec2(mainPos.x - 0.25, mainPos.y), 0);

		rightSensor->body->SetTransform(
			b2Vec2(mainPos.x + 0.25, mainPos.y), 0);


		velocity = b2Vec2_zero;
		grounded = VALUE_NEAR_TO_0(pbody->body->GetLinearVelocity().y);

		//UPDATE SUBMODULES
		projectileManager->Update(dt);
		dialoguesManager->Update(dt);

		//GODMODE
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
			godMode = !godMode;
			pbody->body->SetGravityScale(godMode ? 0.0f : gravity);
			pbody->body->SetLinearVelocity(b2Vec2_zero);
		}

		if (godMode) {
			velocity = b2Vec2_zero;

			if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) ||
				(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) ||
				(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) ||
				(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT))
			{
				
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {
					velocity.x = -moveSpeed * 25;
					CheckMoveX();
					playerState = RUN;
				}
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
					velocity.x = moveSpeed * 25;
					CheckMoveX();
					playerState = RUN;
				}
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
				{
					velocity.y = -moveSpeed * 25;
					playerState = JUMP;
				}
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) 
				{
					velocity.y = moveSpeed * 25;
					playerState = FALL;
				}
			}
			else
			{
				playerState = IDLE;
			}

			pbody->body->SetLinearVelocity(velocity);

		}
		else
		{
			//DAMAGE BOOST
			if (damageBoost && hits == 1) damageAdded = damageBoostAdded;
			else if (damageBoost && hits != 1 && damageSmallBoost) damageAdded = 2;
			else if (damageBoost && hits != 1 && !damageSmallBoost) damageAdded = 0;

			//CHANGERS
			if (playerState == DEAD || playerState == CHARGED) {
			}
			else if (playerState == HURT) {
				if (hurtTimer.ReadSec() >= hurtTime) playerState = IDLE;
			}
			else if (
				(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT ||
					Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT ||
					SDL_GameControllerGetAxis(Engine::GetInstance().input.get()->controller, SDL_CONTROLLER_AXIS_LEFTX) > 8000 ||
					SDL_GameControllerGetAxis(Engine::GetInstance().input.get()->controller, SDL_CONTROLLER_AXIS_LEFTX) < -8000) &&
				stateFlow[playerState][RUN] && grounded
				) 
			{
				playerState = RUN;
			}
			else if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_A) == KEY_DOWN) && ((stateFlow[playerState][JUMP] && grounded) /*MODIFICAR GODMODE*/ || godMode)) {

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
			else if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_X) || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_X) == KEY_DOWN)
				&& stateFlow[playerState][CHOP] && projectileManager->GetNumPickaxes() > 0) {
				stateTimer.Start();
				playerState = CHOP;

				if (meleeTimerOn) deleteCharged = true;
				charging = true;
				meleeTimer.Start();
				meleeTimerOn = true;
			}
			else if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_X) || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_X) == KEY_DOWN)
				&& stateFlow[playerState][PUNCH] && projectileManager->GetNumPickaxes() <= 0) {
				stateTimer.Start();
				playerState = PUNCH;

				meleeTimer.Start();
				meleeTimerOn = true;
			}
			else if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_C) || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_Y) == KEY_DOWN) && stateFlow[playerState][THROW] && projectileManager->GetNumPickaxes() > 0) {

				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) || SDL_GameControllerGetAxis(Engine::GetInstance().input.get()->controller, SDL_CONTROLLER_AXIS_LEFTY) < -6000) projectileManager->ThrowPickaxe({ 0,1 }, pbody->GetPhysBodyWorldPosition());
				else projectileManager->ThrowPickaxe(GetDirection(), pbody->GetPhysBodyWorldPosition());

				stateTimer.Start();
				playerState = THROW;
			}
			else if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == KEY_DOWN)
				&& stateFlow[playerState][DASH] && canDash == true && unlockedDash == true ) {
				pbody->body->SetLinearVelocity(b2Vec2_zero);
				dashParticle->isCasted = true;
				dashParticle->active = true;
				if (dir == RIGHT) pbody->body->ApplyLinearImpulseToCenter(b2Vec2(dashForce, 0), true);
				else if (dir == LEFT) pbody->body->ApplyLinearImpulseToCenter(b2Vec2(-dashForce, 0), true); 

				if (grounded && (playerState == IDLE || playerState == RUN)) {
					dashCooldownTimerOn = true;
					dashCooldownTimer.Start();
				}

				canDash = false;
				dashTimer.Start();
				dashTimerOn = true;
				playerState = DASH;
			}
			else if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_A) == KEY_DOWN) && stateFlow[playerState][TALK] && !dialoguesManager->GetOnDialogue()) {

				dialoguesManager->StartDialogue("DIALOG01");
				playerState = TALK;
			}

			//COYOTE TIME LOGIC
			if (coyoteTimerOn) {
				if (coyoteTimer.ReadSec() < coyoteTimerMax && (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)) {
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
					playerState = JUMP;
					coyoteTimerOn = false;

					plusJumpTimer.Start();
					plusJumpTimerOn = true;
				}
				if (coyoteTimer.ReadSec() >= coyoteTimerMax) coyoteTimerOn = false;
			}

			//DASH LOGIC
			if (dashTimerOn) {
				if (dashTimer.ReadSec() > dashTimerMax) {
					playerState = IDLE;
					dashTimerOn = false;
				}
			}

			//DASH PREVENT SPAMMING LOGIC
			if (dashCooldownTimerOn) {
				if (dashCooldownTimer.ReadSec() > dashCooldownTimerMax) {
					dashCooldownTimerOn = false;
					canDash = true;
				}
			}
			else if (grounded && (playerState == IDLE || playerState == RUN)) canDash = true;

			//PLUS JUMP LOGIC
			if (plusJumpTimerOn) {
				if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_A) == KEY_REPEAT) && !grounded && playerState == JUMP && plusJumpTimer.ReadSec() < plusJumpTimerMax) {
					float jumpPlusForce = 1;
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpPlusForce), true);
				}
				if (plusJumpTimer.ReadSec() >= plusJumpTimerMax) plusJumpTimerOn = false;
			}

			//CHARGED ATTACK LOGIC
			if (chargedCooldown) {
				if (chargedCooldownTimer.ReadSec() > chargedCooldownTimerMax) chargedCooldown = false;
			}
			if (playerState == CHARGED && (meleeTimerOn == false && (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_E) == KEY_UP || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_X) == KEY_UP) || chargeAttackTimer.ReadSec() > chargeAttackTimerMax)) {
				meleeTimer.Start();
				meleeTimerOn = true;
			}

			//MELEE ATTACKS LOGIC
			if (meleeTimerOn) {
				if (deleteCharged) {
					meleeArea->body->SetEnabled(false);
					deleteCharged = false;
				}
				if (meleeTimer.ReadSec() == 0.0) {
					if (playerState == CHARGED) {
						meleeDisplace = (dir == RIGHT) ? (2 * texW / 3 + MELEE_AREA_WIDTH) : (texW / 3 - MELEE_AREA_WIDTH);
						meleeArea = Engine::GetInstance().physics.get()->CreateRectangleSensor((position.getX() + meleeDisplace), position.getY() + texH / 2, MELEE_AREA_WIDTH * 2, texH * 1.5, DYNAMIC);
						meleeArea->ctype = ColliderType::MELEE_AREA_CHARGED;
					}
					else {
						meleeDisplace = (dir == RIGHT) ? (2 * texW / 3 + MELEE_AREA_WIDTH / 2) : (texW / 3 - MELEE_AREA_WIDTH / 2);
						meleeArea = Engine::GetInstance().physics.get()->CreateRectangleSensor((position.getX() + meleeDisplace), position.getY() + texH / 2, MELEE_AREA_WIDTH, texH, DYNAMIC);
						meleeArea->ctype = ColliderType::MELEE_AREA;
					}
				}
				if (meleeTimer.ReadSec() < meleeTimerMax) {
					b2Vec2 meleeAreaMovePos = b2Vec2(pbody->body->GetPosition().x + PIXEL_TO_METERS(meleeDisplace) - PIXEL_TO_METERS(texW / 2), pbody->body->GetPosition().y);
					meleeArea->body->SetTransform(meleeAreaMovePos, 0);
				}
				if (meleeTimer.ReadSec() >= meleeTimerMax) {
					meleeArea->body->SetEnabled(false);
					meleeTimerOn = false;
				}
			}

			if (respawnHeal == true) respawnHeal = false;

			//LOGIC & SFX
			switch (playerState) {
			case IDLE:
				if (playSound == false) {
					playSound = true;
				}
				break;
			case RUN:
				// SALTO: Evaluar siempre primero
				if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)
					&& (grounded || godMode))
				{
					playerState = JUMP;
					pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
					grounded = false;
					plusJumpTimer.Start();
					plusJumpTimerOn = true;
					break;
				}

				// MOVIMIENTO HORIZONTAL si est� en suelo y no hay colisi�n lateral
				if (grounded) {
					if (CheckMoveX()) {
						playerState = RUN;
						MoveX();
					}
					else {
						playerState = IDLE;
					}
				}
				break;
			case JUMP:
				if (playSound == true) {
					Engine::GetInstance().audio.get()->PlayFx(playerJumpSFX);
					playSound = false;
				}
				if (CheckMoveX()) MoveX();
				if (grounded)
				{
					playerState = IDLE;
					if (playSound == false) {
						playSound = true;
					}
				}
				break;
			case FALL:
				if (playSound == false) {
					playSound = true;
				}
				if (CheckMoveX()) MoveX();
				if (grounded) {
					Engine::GetInstance().audio.get()->PlayFx(playerLandSFX);
					playerState = IDLE;
				}
				break;
			case PUNCH:
				if (playSound == true) {
					Engine::GetInstance().audio.get()->PlayFx(playerAttack2SFX);
					playSound = false;
				}
				if (CheckMoveX() && !grounded) MoveX();
				if (stateTimer.ReadSec() >= punchTimerAnimation)
				{
					playerState = IDLE;
					if (playSound == false) {
						playSound = true;
					}
				}
				break;
			case CHOP:
				if (playSound == true) {
					Engine::GetInstance().audio.get()->PlayFx(playerAttack1SFX);
					playSound = false;
				}
				if (CheckMoveX() && !grounded) MoveX();

				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_E) == KEY_UP) charging = false;
				if (stateTimer.ReadSec() >= pickaxeTimerAnimation)
				{
					if (charging == true && chargedCooldown == false && unlockedCharged == true) {
						playerState = CHARGED;
						chargeAttackTimer.Start();
						break;
					}
					else playerState = IDLE;
					if (playSound == false) {
						playSound = true;
					}
				}
				break;
			case THROW:
				if (playSound == true) {
					Engine::GetInstance().audio.get()->PlayFx(playerThrowSFX);
					playSound = false;
				}
				if (CheckMoveX() && !grounded) MoveX();
				if (stateTimer.ReadSec() >= pickaxeTimerAnimation)
				{
					playerState = IDLE;
					if (playSound == false) {
						playSound = true;
					}
				}
				break;
			case DEAD:
			{
				pbody->body->SetLinearVelocity(b2Vec2(0, 0));
				if (respawnTimer.ReadSec() >= respawnTime) {
					Engine::GetInstance().scene.get()->LoadState();
					playerState = IDLE;
					hits = 3;
				}
				break;
			}
			case CHARGED:
				if (chargeAttackTimer.ReadSec() > chargeAttackTimerMax || Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_E) == KEY_UP) {
					chargedCooldownTimer.Start();
					chargedCooldown = true;
					playerState = IDLE;
				}
				break;
			case TALK:

				if (!dialoguesManager->GetOnDialogue()) {
					playerState = IDLE;
				}
				break;
			default:
				break;
			}

			if (playerState == DASH) velocity = { pbody->body->GetLinearVelocity().x, 0 };
			else velocity = { velocity.x, pbody->body->GetLinearVelocity().y };
			pbody->body->SetLinearVelocity(velocity);


		}
	}

	//ANIMS
	currentFrame = currentAnim->GetCurrentFrame();
	drawOffsetX = currentAnim->GetCurrentOffsetX();
	drawOffsetY = currentAnim->GetCurrentOffsetY();
	
	switch (playerState) {
	case IDLE:
		if (resetAnimation == false) {
			currentAnim->Reset();
			resetAnimation = true;
		}
		currentAnim = &idle;
		break;
	case RUN:
		currentAnim = &walk;
		break;
	case JUMP:
		currentAnim = &jump;
		if (resetAnimation == true) {
			currentAnim->Reset();
			resetAnimation = false;
		}
		break;
	case FALL:
		currentAnim = &fall;
		if (resetAnimation == true) {
			currentAnim->Reset();
			resetAnimation = false;
		}
		break;
	case PUNCH:
		currentAnim = &punch;
		if (resetAnimation == true) {
			currentAnim->Reset();
			resetAnimation = false;
		}
		break;
	case CHOP:
		currentAnim = &chop;
		if (resetAnimation == true) {
			currentAnim->Reset();
			resetAnimation = false;
		}
		break;
	case THROW:
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) || SDL_GameControllerGetAxis(Engine::GetInstance().input.get()->controller, SDL_CONTROLLER_AXIS_LEFTY) < -6000) {
			currentAnim = &throwPixUp;
		}
		else {
			currentAnim = &throwPix;
		}
		if (resetAnimation == true) {
			currentAnim->Reset();
			resetAnimation = false;
		}
		break;

	case DASH:
		currentAnim = &dash;
		if (resetAnimation == true) {
			currentAnim->Reset();
			resetAnimation = false;

		}
		break;
	case CHARGED:
		currentAnim = &charged;
		if (resetAnimation == true) {
			currentAnim->Reset();
			resetAnimation = false;
		}
		break;
	case HURT:
		currentAnim = &hurt;
		if (resetAnimation == true) {
			currentAnim->Reset();
			resetAnimation = false;
		}
		break;
	case DEAD:
		currentAnim = &death;
		if (resetAnimation == true) {
			currentAnim->Reset();
			resetAnimation = false;
		}
		break;
	case TALK:
		if (resetAnimation == false) {
			currentAnim->Reset();
			resetAnimation = true;
		}
		currentAnim = &idle;
		break;
	}

	b2Transform pbodyPos = pbody->body->GetTransform();

	if (renderable) {
		position.setX(METERS_TO_PIXELS((pbodyPos.p.x) - texW / 2 ) + drawOffsetX);
		position.setY(METERS_TO_PIXELS((pbodyPos.p.y) - texH / 2) + drawOffsetY);

		if (dir == RIGHT) Engine::GetInstance().render.get()->DrawTextureBuffer(texture, position.getX(), position.getY(), false, PLAYER, &currentFrame);
		else if (dir == LEFT) Engine::GetInstance().render.get()->DrawTextureBuffer(texture, position.getX(), position.getY(), true,PLAYER, &currentFrame);
	}

	currentAnim->Update();

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_X)) currencyManager->SumCurrency(200);

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
	int soulAmount = 1;
	
	//Colision de los sensores
	if (physA == leftSensor && physB->ctype == ColliderType::PLATFORM || physA == leftSensor && physB->ctype == ColliderType::CLIMBINGWALL || physA == leftSensor && physB->ctype == ColliderType::PICKAXE)
	{
		leftBlocked = true;
	}

	if (physA == rightSensor && physB->ctype == ColliderType::PLATFORM || physA == rightSensor && physB->ctype == ColliderType::CLIMBINGWALL || physA == rightSensor && physB->ctype == ColliderType::PICKAXE)
	{
		rightBlocked = true;
	}

	//Colisi�n del cuerpo principal
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;
	case ColliderType::PICKAXE:
		onPickaxe = true;
		LOG("Collision PICKAXE");
		break;
	case ColliderType::SPYKE:
		LOG("Collision SPYKE");
		break;
	case ColliderType::JUMP:
		LOG("Collision BULLET");
		if (!godMode || !canHurt) {
			if (playerState != DEAD) {
				//HURT LOGIC
				if (hits >= 1 && playerState != HURT) DamagePlayer();
				if (hits == 0) KillPlayer();
				//PUSHING THE PLAYER WHEN HURT
				b2Vec2 pushVec((physA->body->GetPosition().x - physB->body->GetPosition().x),
					(physA->body->GetPosition().y - physB->body->GetPosition().y));
				pushVec.Normalize();
				pushVec *= pushForce;
				pushVec.x *= 6;

				pbody->body->SetLinearVelocity(b2Vec2(0, 0));
				pbody->body->ApplyLinearImpulseToCenter(pushVec, true);
			}

		}
		break;
	case ColliderType::ENEMY:
		LOG("Collision ENEMY");
		if (!godMode || !canHurt) {
			if (playerState != DEAD) {
				//HURT LOGIC
				if (hits >= 1 && playerState != HURT) DamagePlayer();
				if (hits == 0) KillPlayer();
				//PUSHING THE PLAYER WHEN HURT
				b2Vec2 pushVec((physA->body->GetPosition().x - physB->body->GetPosition().x),
					(physA->body->GetPosition().y - physB->body->GetPosition().y));
				pushVec.Normalize();
				pushVec *= pushForce;
				pushVec.x *= 6;

				pbody->body->SetLinearVelocity(b2Vec2(0, 0));
				pbody->body->ApplyLinearImpulseToCenter(pushVec, true);
			}
			
		}
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
		hits = 3;
		Engine::GetInstance().scene.get()->SaveState();
		break;
	case ColliderType::LEVEL_CHANGER:
		if (Engine::GetInstance().scene.get()->level == LVL1) {
			Engine::GetInstance().scene.get()->ChangeLevel();
		}
		else if (Engine::GetInstance().scene.get()->level == LVL2) {
		}
		else if (Engine::GetInstance().scene.get()->level == LVL3) {
			Engine::GetInstance().scene.get()->ChangeLevel();
		}
		else if (Engine::GetInstance().scene.get()->level == LVL4) {
			Engine::GetInstance().scene.get()->ChangeLevel();
		}
		else if (Engine::GetInstance().scene.get()->level == LVL5) {
			Engine::GetInstance().scene.get()->ChangeLevel();
		}
		break;
	
	case ColliderType::BOSS_STARTER:
		if (!Engine::GetInstance().scene.get()->GetStartBossFight())Engine::GetInstance().scene.get()->SetStartBossFight(true);
		break;
	
	case ColliderType::ORB:
		if (physB->width == 4) soulAmount = rand() % 4 + 1;
		else if (physB->width == 7) soulAmount = rand() % 4 + 5;
		else if (physB->width == 10) soulAmount = rand() % 5 + 10;

		currencyManager->SumCurrency(soulAmount);
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
	//Colision de los sensores
	if (physA == leftSensor && physB->ctype == ColliderType::PLATFORM || physA == leftSensor && physB->ctype == ColliderType::CLIMBINGWALL || physA == leftSensor && physB->ctype == ColliderType::PICKAXE)
	{
		leftBlocked = false;
	}

	if (physA == rightSensor && physB->ctype == ColliderType::PLATFORM || physA == rightSensor && physB->ctype == ColliderType::CLIMBINGWALL || physA == rightSensor && physB->ctype == ColliderType::PICKAXE)
	{
		rightBlocked = false;
	}
	
	//Colisi�n del cuerpo principal
	switch (physB->ctype)
	{
	case ColliderType::PICKAXE:
		onPickaxe = false;
		break;
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::CHECKPOINT:
		LOG("End Collision Checkpoint");
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

bool Player::HaveUpgrade(int index) {
	bool haveUnlocked = false;
	if (!unlockedUpgrades.empty()) {
		for (int i = 0; i < unlockedUpgrades.size(); i++) {
			if (unlockedUpgrades[i] == index) haveUnlocked = true;
		}
	}
	return haveUnlocked;
}

bool Player::HaveActiveUpgrade(int index) {
	bool haveUnlocked = false;
	if (!upgrades.empty()) {
		for (int i = 0; i < upgrades.size(); i++) {
			if (upgrades[i] == index) haveUnlocked = true;
		}
	}
	return haveUnlocked;
}

void Player::SaveData(pugi::xml_node playerNode, pugi::xml_node upgradesNode) {
	if (active) {
		playerNode.attribute("x").set_value(pbody->GetPhysBodyWorldPosition().getX());
		playerNode.attribute("y").set_value(pbody->GetPhysBodyWorldPosition().getY());
		upgradesNode.attribute("dash").set_value(unlockedDash);
		upgradesNode.attribute("charged").set_value(unlockedCharged);

		bool up0 = false, up1 = false, up2 = false, up3 = false, up4 = false, up5 = false, up6 = false, up7 = false;
		if (!unlockedUpgrades.empty()) {
			for (int i = 0; i < unlockedUpgrades.size(); i++) {
				switch (unlockedUpgrades[i]) {
				default:
					break;
				case 0: up0 = true;
					break;
				case 1: up1 = true;
					break;
				case 2: up2 = true;
					break;
				case 3: up3 = true;
					break;
				case 4: up4 = true;
					break;
				case 5: up5 = true;
					break;
				case 6: up6 = true;
					break;
				case 7: up7 = true;
					break;
				}
			}
		}
		upgradesNode.attribute("up0").set_value(up0);
		upgradesNode.attribute("up1").set_value(up1);
		upgradesNode.attribute("up2").set_value(up2);
		upgradesNode.attribute("up3").set_value(up3);
		upgradesNode.attribute("up4").set_value(up4);
		upgradesNode.attribute("up5").set_value(up5);
		upgradesNode.attribute("up6").set_value(up6);
		upgradesNode.attribute("up7").set_value(up7);
	}
}

void Player::LoadData(pugi::xml_node playerNode, pugi::xml_node upgradesNode)
{
	position.setX(playerNode.attribute("x").as_int());
	position.setY(playerNode.attribute("y").as_int());
	SetPosition(position);
	unlockedDash = upgradesNode.attribute("dash").as_bool();
	unlockedCharged = upgradesNode.attribute("charged").as_bool();

	if (upgradesNode.attribute("up0").as_bool() == true) unlockedUpgrades.push_back(0);
	if (upgradesNode.attribute("up1").as_bool() == true) unlockedUpgrades.push_back(1);
	if (upgradesNode.attribute("up2").as_bool() == true) unlockedUpgrades.push_back(2);
	if (upgradesNode.attribute("up3").as_bool() == true) unlockedUpgrades.push_back(3);
	if (upgradesNode.attribute("up4").as_bool() == true) unlockedUpgrades.push_back(4);
	if (upgradesNode.attribute("up5").as_bool() == true) unlockedUpgrades.push_back(5);
	if (upgradesNode.attribute("up6").as_bool() == true) unlockedUpgrades.push_back(6);
	if (upgradesNode.attribute("up7").as_bool() == true) unlockedUpgrades.push_back(7);
}

void Player::KillPlayer() {
	playerState = DEAD;
	//pbody->body->SetGravityScale(0);
	respawnTimer.Start();
}

bool Player::CheckMoveX()
{
	Input* input = Engine::GetInstance().input.get();

	bool moveRight = input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT;
	bool moveLeft = input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT;
	

	if ((moveRight || SDL_GameControllerGetAxis(input->controller, SDL_CONTROLLER_AXIS_LEFTX) > 8000) && !rightBlocked)
	{
		dir = RIGHT;
		Engine::GetInstance().scene.get()->cameraDirectionChangeActivation = true;
		return true;
	}
	else if ((moveLeft || SDL_GameControllerGetAxis(input->controller, SDL_CONTROLLER_AXIS_LEFTX) < -8000) && !leftBlocked)
	{
		dir = LEFT;
		Engine::GetInstance().scene.get()->cameraDirectionChangeActivation = true;
		return true;
	}

	return false;
}


void Player::MoveX() {
	velocity.x = (dir == RIGHT ? moveSpeed * 16 : -moveSpeed * 16);
}

void Player::CheckJump() {
	if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_X) == KEY_DOWN ) && grounded) {
		pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
		grounded = false; // ya no est� en el suelo hasta que colisione de nuevo
	}
}


void Player::DamagePlayer() {
	hits--;
	playerState = HURT;
	hurtTimer.Start();
}

void Player::LoadDefaults() {
	plusJumpTimerMax = 0.2; // salto extra del salto
	dashForce = 8; // impulso de dash
	chargedCooldownTimerMax = 5; // tiempo entre ataque cargado
	projectileManager->pickaxeRecollectCount = 2.5; // tiempo entre piqueta y piqueta
	moveSpeed = 0.7; // velocidad del player
	damageAdded = 0; // a�adido de da�o al base
	maxPickaxes = 3; // piquetas m�ximas
	projectileManager->maxPickaxes = maxPickaxes;
	damageSmallBoost = false;
	damageBoost = false; // hacer mas da�o cuando tienes un coraz�n
	damageBoostAdded = 5; // el da�o que haces cuando tienes un coraz�n y el boost
}

void Player::LoadUpgrades() {
	LoadDefaults();
	if (!upgrades.empty()) {
		for (int i = 0; i < upgrades.size(); i++) {
			switch (upgrades[i]) {
			case 0:
				plusJumpTimerMax = 0.25;
				break;
			case 1:
				dashForce = 12;
				break;
			case 2:
				chargedCooldownTimerMax = 3;
				break;
			case 3:
				projectileManager->pickaxeRecollectCount = 1.25;
				break;
			case 4:
				moveSpeed = 0.9;
				break;
			case 5:
				damageSmallBoost = true;
				damageAdded = 2;
				break;
			case 6:
				maxPickaxes = 4;
				projectileManager->pickaxeCount = maxPickaxes;
				projectileManager->maxPickaxes = maxPickaxes;
				break;
			case 7:
				damageBoost = true;
				break;
			case 8:
				break;
			case 9:
				break;
			default:
				break;
			}
		}
	}
}

void Player::UnlockUpgrade(int num) {
	bool canAdd = true;
	if (!upgrades.empty()) {
		for (int i = 0; i < unlockedUpgrades.size(); i++) {
			if (num == unlockedUpgrades[i]) canAdd = false; 
		}
	}
	if (canAdd) unlockedUpgrades.push_back(num);
}

void Player::AddUpgrade(int num) {
	bool haveUpgrade = false;
	if (!unlockedUpgrades.empty()) {
		for (int i = 0; i < unlockedUpgrades.size(); i++) {
			if (num == unlockedUpgrades[i]) haveUpgrade = true;
		}
	}
	if (haveUpgrade) {
		bool canAdd = true;
		if (upgrades.empty()) upgrades.push_back(num);
		else if (upgrades.size() <= maxUpgrades && num >= 0 && num <= 9) {
			for (int i = 0; i < upgrades.size(); i++) {
				if (upgrades[i] == num) canAdd = false;
			}
			if (canAdd == false) {
			}
			else upgrades.push_back(num);
		}
	}
	LoadUpgrades();
}

void Player::RemoveUpgrade(int num) {
	int index = -1;
	if (!upgrades.empty()) {
		for (int i = 0; i < upgrades.size(); i++) {
			if (upgrades[i] == num) index = i;
		}
	}
	if (index >= 0) upgrades.erase(upgrades.begin() + index);
	LoadUpgrades();
}