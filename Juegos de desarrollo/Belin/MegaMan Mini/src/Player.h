#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"
#include "Bullet.h"

# define MAX_LIFE 16
# define MAX_AMMO 4 

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

	void UpdateRender();
	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	void Respawn();

	void MoveToNextCheckpoint();

public:


	Vector2D checkpoints[6];


	int life, ammo;

	//Declare player parameters
	float speed = 5.0f;
	SDL_Texture* texture = NULL;
	SDL_Texture* infoBars = NULL;
	
	int texW, texH;

	//Audio fx
	int ammoCollectedFx;
	int lifeCollectedFx;
	int pointCollectedFx;
	int shotFx;
	int hurtFx;
	int punchFx;
	int deadFx;
	int levelCompletedFx;
	int hitFx;

	Bullet* bullet;

	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;
	PhysBody* pbody2;
	float jumpForce = 2.2f; // The force to apply when jumping
	bool isJumping = false; // Flag to check if the player is currently jumping

	pugi::xml_node parameters;
	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation idleL;
	Animation run;
	Animation runL;
	Animation jump;
	Animation jumpL;
	Animation die;
	Animation dieL;
	Animation punch;
	Animation punchL;
	Animation idleShoot;
	Animation idleShootL;

	bool attack;

	bool shoot;

	bool dead;

	bool lookRight;

	bool godmode;

	bool hit;             
	int hitRepetitions;       
	Uint32 hitTimer;
	bool isVisible;       
	int maxHitRepetitions;
	Uint32 hitDuration;

	float punchTimer;

	float shootTimer;

	float timerFinish;
};