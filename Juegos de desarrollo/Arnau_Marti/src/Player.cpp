#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "tracy/Tracy.hpp"


Player::Player() : Entity(EntityType::PLAYER)
{
	name = "Player";
}

Player::~Player() {

}

bool Player::Awake() {

	position = Vector2D(0, 0);
	return true;
}

bool Player::Start() {

	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();
	lives = 5;
	dead = false;
	godMode = false;

	//Load animations
	idleRight.LoadAnimations(parameters.child("animations").child("idleRight"));
	idleLeft.LoadAnimations(parameters.child("animations").child("idleLeft"));
	runRight.LoadAnimations(parameters.child("animations").child("runRight"));
	runLeft.LoadAnimations(parameters.child("animations").child("runLeft"));
	jumpRight.LoadAnimations(parameters.child("animations").child("jumpRight"));
	jumpLeft.LoadAnimations(parameters.child("animations").child("jumpLeft"));
	dieRight.LoadAnimations(parameters.child("animations").child("dieRight"));
	dieLeft.LoadAnimations(parameters.child("animations").child("dieLeft"));
	dashRight.LoadAnimations(parameters.child("animations").child("dashRight"));
	dashLeft.LoadAnimations(parameters.child("animations").child("dashLeft"));
	damageRight.LoadAnimations(parameters.child("animations").child("damageRight"));
	damageLeft.LoadAnimations(parameters.child("animations").child("damageLeft"));
	attackRight.LoadAnimations(parameters.child("animations").child("attackfrontRight"));
	attackLeft.LoadAnimations(parameters.child("animations").child("attackfrontLeft"));
	currentAnimation = &idleRight;
	isLookingRight = true;

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);
	
	pbody->listener = this;

	pbody->ctype = ColliderType::PLAYER;

	pugi::xml_document audioFile;
	pugi::xml_parse_result result = audioFile.load_file("config.xml");

	saveGame = Engine::GetInstance().audio.get()->LoadFx(audioFile.child("config").child("audio").child("fx").child("saveGame").attribute("path").as_string()); 
	loadGame = Engine::GetInstance().audio.get()->LoadFx(audioFile.child("config").child("audio").child("fx").child("loadGame").attribute("path").as_string()); 
	run = Engine::GetInstance().audio.get()->LoadFx(audioFile.child("config").child("audio").child("fx").child("run").attribute("path").as_string()); 
	attack = Engine::GetInstance().audio.get()->LoadFx(audioFile.child("config").child("audio").child("fx").child("attack").attribute("path").as_string()); 
	jump = Engine::GetInstance().audio.get()->LoadFx(audioFile.child("config").child("audio").child("fx").child("jump").attribute("path").as_string()); 
	dash = Engine::GetInstance().audio.get()->LoadFx(audioFile.child("config").child("audio").child("fx").child("dash").attribute("path").as_string()); 

	 
	return true;
}      

bool Player::Update(float dt)
{
	ZoneScoped;

	if (godMode) {
		lives = 5;
		dead = false;
		isJumping = false; 

		pbody->body->SetGravityScale(0);
		velocity = b2Vec2(0, 0);

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			velocity.y = -0.2 * 16;
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			velocity.y = 0.2 * 16;
		}
	}
	else {
		pbody->body->SetGravityScale(1);
		velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);
	}

	// Move left
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		
		if (dead) {
			velocity.x = 0;

		}
		else {
			velocity.x = -0.2 * 16;
			currentAnimation = &runLeft;
			isLookingRight = false;
			//Engine::GetInstance().audio.get()->PlayFx(run, 1);
		}
		
	}

	// Move right
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		if (dead) {
			velocity.x = 0;

		}
		else {
			velocity.x = 0.2 * 16;
			currentAnimation = &runRight;
			isLookingRight = true;
			//Engine::GetInstance().audio.get()->PlayFx(run, 1);
		}

	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		isDashing = true;
		dashTimer = dashDuration;
		Engine::GetInstance().audio.get()->PlayFx(dash);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
		isAttacking = true;
		attackTimer = attackDuration;
		Engine::GetInstance().audio.get()->PlayFx(attack);
	}

	if (isDashing) {
		dashTimer -= 0.16;
		if (dashTimer <= 0) {
			isDashing = false;
		}
		if (isLookingRight && isDashing) {
			velocity.x = 0.4 * 16;
			currentAnimation = &dashRight;
		}
		else {
			velocity.x = -0.4 * 16;
			currentAnimation = &dashLeft;
		}
	}
	
	if (isAttacking) {
		attackTimer -= 0.16;
		if (attackTimer <= 0) {
			isAttacking = false;
			
		}
		if (isLookingRight && isAttacking) {
			//Efecte atacar
			currentAnimation = &attackRight;
		}
		else {
			//Efecte
			currentAnimation = &attackLeft;
		}
	}
	
	//Jump
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false) {
		// Apply an initial upward force
		pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
		Engine::GetInstance().audio.get()->PlayFx(jump);
		isJumping = true;
	}

	// If the player is jumpling, we don't want to apply gravity, we use the current velocity prduced by the jump
	if(isJumping == true)
	{
		
		if (dead) {
			velocity.x = 0;
			velocity.y = 0;
		}
		else {
			velocity.y = pbody->body->GetLinearVelocity().y;
			if (isLookingRight) {
				if (isDashing) {
					currentAnimation = &dashRight;
				}
				else {
					currentAnimation = &jumpRight;
				}
			}
			else {
				if (isDashing) {
					currentAnimation = &dashLeft;
				}
				else {
					currentAnimation = &jumpLeft;
				}
			}
		}
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		takeDamage();
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		lives = lives + 1;

		if (lives <= 0)
		{
			lives = 0;
			dead = true;
		}
		if (lives > 0) {
			dead = false;
		}
		if (lives >= 5) {
			lives = 5;
		}
		
		LOG("Curar vides");
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		godMode = !godMode;
	}

	// Apply the velocity to the player
	pbody->body->SetLinearVelocity(velocity);

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();
	if (isLookingRight) {
		if (dead == true) {
			currentAnimation = &dieRight;
		}
		else {
			currentAnimation = &idleRight;
		}
	}
	else {
		if (dead == true) {
			currentAnimation = &dieLeft;
		}
		else {
			currentAnimation = &idleLeft;
		}
	}

	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		takePoints();
		break;
	case ColliderType::POTION:
		LOG("Collision POTION");
		heal();
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	case ColliderType::GROUND:
		LOG("Collision GROUND");
		isJumping = false;
		break;
	case ColliderType::SPIKES:
		LOG("Collision SPIKES");
		isJumping = false;
		takeDamage();
		break;
	case ColliderType::ENEMY:
		LOG("Collision ENEMY");
		takeDamage();
		break;
	case ColliderType::BOSS:
		break;
	case ColliderType::CHECKPOINT:
		Engine::GetInstance().scene.get()->SaveState();
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
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		break;
	case ColliderType::POTION:
		LOG("End Collision POTION");
		break;
	case ColliderType::UNKNOWN:
		LOG("End Collision UNKNOWN");
		break;
	case ColliderType::SPIKES:
		LOG("End Collision SPIKES");
		break;
	default:
		break;
	}
}

void Player::takeDamage() {

	if (lives <= 0) {
		dead = true;
		pbody->body->SetLinearVelocity(b2Vec2(0, 0));
		if (isLookingRight) {
			currentAnimation = &dieRight;
		}
		else {
			currentAnimation = &dieLeft;
		}
	}
	else {
		lives = lives-1;
		LOG("Treure vides");
		if (isLookingRight) {
			pbody->body->ApplyLinearImpulseToCenter(b2Vec2(-10, -2.0f), true);
			currentAnimation = &damageRight;
		}
		else {
			pbody->body->ApplyLinearImpulseToCenter(b2Vec2(10, -2.0f), true);
			currentAnimation = &damageLeft;
		}
		velocity.y = pbody->body->GetLinearVelocity().y;
		velocity.x = pbody->body->GetLinearVelocity().x;
	}
}

Vector2D Player::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Player::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

void Player::heal() {
	lives++;
}

void Player::takePoints() {
	points += 10;
}

