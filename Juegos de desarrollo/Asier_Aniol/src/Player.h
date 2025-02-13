#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"
#include "Bullet.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();

	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void ResetPlayerPosition();

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();
	Vector2D GetDirection() const;


	SDL_RendererFlip hflip = SDL_FLIP_NONE;

public:

	//Declare player parameters
	float speed = 0.3f;
	SDL_Texture* texture = NULL;
	int texW, texH;
	bool flipSprite = false;

	//Audio fx
	int pickCoinFxId;
	int dieFxId;
	int fallFxId;
	int shootFxId;
	int jumpFxId;

	int lives = 3;
	bool hasLost = false;

	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;
	float jumpForce = 2.5f; // The force to apply when jumping

	float originalJumpForce = 2.5f;
	float powerUpJump = 3.25f;
	float originalSpeed = 0.3f;
	float powerUpSpeed = 0.4f;

	bool powerUpJumpActive = false;
	bool powerUpSpeedActive = false;

	bool hasToUpdateCam = false;

	bool isJumping = false; // Flag to check if the player is currently jumping
	bool canJump;
	bool isFalling = false;
	bool isRunning = false; //Flag to check if the player is currently running
	bool isShooting = false;
	bool isDead = false;
	bool godMode = false;
	bool respawn = false;
	bool reachedCheckpoint = false;

	int coinCount = 0;

	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation move;
	Animation jump;
	Animation fall;
	Animation shoot;
	Animation shootup;
	Animation jumpShoot;
	Animation jumpShootUp;
	Animation moveShoot;
	Animation moveShootUp;
	Animation die;

};
