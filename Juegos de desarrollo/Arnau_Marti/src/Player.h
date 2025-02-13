#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"
#include "Timer.h"

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

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void takeDamage();

	void heal();

	void takePoints();

	Vector2D GetPosition();
	void SetPosition(Vector2D pos);

public:

	//Declare player parameters
	float speed = 3.0f;
	SDL_Texture* texture = NULL;
	int texW, texH;

	int lives;
	int points;
	bool dead;
	bool godMode;

	PhysBody* pbody;
	float jumpForce = 1.8; // The force to apply when jumping
	bool isJumping = false; // Flag to check if the player is currently jumping
	bool isDashing = false;
	bool isAttacking = false;
	float dashDuration = 4;
	float attackDuration = 3;
	float dashTimer = 0.0f;
	float attackTimer;
	Timer dashCooldown;

	b2Vec2 velocity;

	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idleRight;
	Animation idleLeft;
	Animation runRight;
	Animation runLeft;
	Animation jumpRight;
	Animation jumpLeft;
	Animation dieRight;
	Animation dieLeft;
	Animation dashRight;
	Animation dashLeft;
	Animation damageRight;
	Animation damageLeft;
	Animation attackRight;
	Animation attackLeft;
	
	bool isLookingRight;

	//Audio
	int saveGame;
	int loadGame;
	int attack;
	int jump;
	int run;
	int dash;

};