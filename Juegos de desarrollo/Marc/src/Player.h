#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"
#include "Animation.h"
#include "Timer.h"
#include "DashParticle.h"

#define GHOST_W 32
//#define MAX_PICKAXES 4
#define MELEE_AREA_WIDTH 30

//FORWARD DECLARATION
class ProjectileManager;
class CurrencyManager;
class DialoguesManager;

struct SDL_Texture;

enum state {
	IDLE = 0,
	RUN,
	JUMP,
	FALL,
	PUNCH,
	THROW,
	CHOP,
	DASH,
	CHARGED,
	HURT,
	DEAD,
	TALK
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

	void SaveData(pugi::xml_node playerNode, pugi::xml_node upgradesNode);

	void LoadData(pugi::xml_node playerNode, pugi::xml_node upgradesNode);

	void Restart();

	void KillPlayer();
	
	bool CheckMoveX();

	void MoveX();

	void CheckJump();

	void DamagePlayer();

	void LoadDefaults();

	void LoadUpgrades();

	void UnlockUpgrade(int num);

	void AddUpgrade(int num);

	void RemoveUpgrade(int num);

	Vector2D GetDirection() const;
	
	bool HaveUpgrade(int index);

	bool HaveActiveUpgrade(int index);

public:
	int maxPickaxes; // piquetas máximas
	int hits = 3;
	Timer hurtTimer;
	float hurtTime = 0.5;
	pugi::xml_node audioNode;

	SDL_Texture* texture;
	int texW, texH;
	int t_texW, t_texH;
	SDL_Rect currentFrame;
	int drawOffsetX;
	int drawOffsetY;

	PhysBody* pbody;

	PhysBody* leftSensor;
	PhysBody* rightSensor;
	bool leftBlocked = false;
	bool rightBlocked = false;

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
	bool canHurt = true;
	
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
	Animation throwPixUp;
	Animation dash;
	Animation charged;

	bool resetAnimation = false;

	//MANAGERS
	ProjectileManager* projectileManager;
	CurrencyManager* currencyManager;
	DialoguesManager* dialoguesManager;

	Timer coyoteTimer;
	bool coyoteTimerOn = false;
	float coyoteTimerMax = 0.1; // margen de coyote time

	Timer plusJumpTimer;
	bool plusJumpTimerOn = false;
	float plusJumpTimerMax; // salto extra del salto

	Timer dashTimer;
	float dashTimerMax = 0.2; // lo que dura el dash
	bool dashTimerOn = false;
	float dashForce; // impulso de dash
	bool canDash = true;

	Timer dashCooldownTimer;
	float dashCooldownTimerMax = 0.5; // cooldown entre dashes
	bool dashCooldownTimerOn = false;

	bool charging = false;
	Timer chargeAttackTimer;
	float chargeAttackTimerMax = 1; // lo que dura el ataque cargado de máximo

	bool chargedCooldown = false;
	Timer chargedCooldownTimer;
	float chargedCooldownTimerMax; // tiempo entre ataque cargado
	bool deleteCharged = false;

	int damageAdded; // añadido de daño al base
	int damageBoostAdded; // añadido de daño al base cargado
	bool damageBoost = false;
	bool damageSmallBoost = false;

	std::vector <int> upgrades;
	std::vector <int> unlockedUpgrades;
	int maxUpgrades = 2;

	bool unlockedDash = true;
	bool unlockedCharged = true;

	PhysBody* meleeArea;
	Timer meleeTimer;
	bool meleeTimerOn = false;
	float meleeTimerMax = 0.15;
	float meleeDisplace = 0.0;

	bool onPickaxe = false;

	DashParticle* dashParticle;

	//STATES FLOW
	state playerState; 
	std::vector<std::vector<bool>> stateFlow;
	Timer stateTimer;
	
	float pickaxeTimerAnimation = 0.5;
	float punchTimerAnimation = 0.5;

	//SFX
	int playerJumpSFX;
	int playerLandSFX;
	int playerAttack1SFX;
	int playerAttack2SFX;
	int playerThrowSFX;

	bool playSound = true;

	bool respawnHeal = false;
	Direction dir;
};