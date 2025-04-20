#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"
#include "Timer.h"
#include "Particle.h"


#define GHOST_W 32
#define MAX_PICKAXES 3
#define MELEE_AREA_WIDTH 10

//FORWARD DECLARATION
class PickaxeManager;

struct SDL_Texture;

enum state {
	IDLE = 0,
	RUN,
	JUMP,
	FALL,
	PUNCH,
	THROW,
	CHOP
};

enum Direction {

	LEFT,
	RIGHT
};

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool PostUpdate(float dt);

	bool CleanUp();

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void SetPosition(Vector2D pos);

	void SaveData(pugi::xml_node playerNode);

	void LoadData(pugi::xml_node playerNode);

	void Restart();

	void KillPlayer();
	
	bool CheckMoveX();

	void MoveX();

	void CheckJump();

	Vector2D GetDirection() const;

public:

	SDL_Texture* texture;
	SDL_Texture* t_texture;
	int texW, texH;
	int t_texW, t_texH;
	SDL_Rect currentFrame;

	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;
	float jumpForce; // The force to apply when jumping
	float pushForce; // The force to apply when getting pushed;
	float moveSpeed;
	float friction;
	float gravity;

	bool flipSprite = false;

	bool grounded;
	bool godMode;
	bool canClimb;
	bool reachedCheckPoint;

	Timer hurtTimer;
	float hurtTime;
	
	Timer respawnTimer;
	float respawnTime;

	b2Vec2 velocity = b2Vec2_zero;
	b2Vec2 initPos;
	b2Vec2 pushDir;

	pugi::xml_node savedDataNode;

	Animation* currentAnim = nullptr;

	Animation idle;
	Animation walk;
	Animation jump;
	Animation fall;
	Animation hurt;
	Animation death;
	Animation punch;
	Animation chop;
	Animation throwPix;

	bool resetAnimation = false;

	PickaxeManager* pickaxeManager;

	Timer coyoteTimer;
	bool coyoteTimerOn = false;
	float coyoteTimerMax = 0.15;

	Timer plusJumpTimer;
	bool plusJumpTimerOn = false;
	float plusJumpTimerMax = 0.15; // 0.1 - 0.15 to adjust

	PhysBody* meleeArea;
	Timer meleeTimer;
	bool meleeTimerOn = false;
	float meleeTimerMax = 0.15;
	float meleeDisplace = 0.0;

	//STATES FLOW
	state playerState; 
	std::vector<std::vector<bool>> stateFlow;
	Timer stateTimer;
	
	float pickaxeTimerAnimation = 0.5;
	float punchTimerAnimation = 0.5;


	Direction dir;
};