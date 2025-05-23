#pragma once


#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Timer.h"

class Player;
class CurrencyManager;

struct SDL_Texture;

class Enemy : public Entity
{
public:
	enum Type {
		FLY,
		GROUND
	};

	enum State {
		PATROL,
		CHASING,
		ATTACK,
		JUMP,
		RUN,
		TIRED,
		DEAD,
		HURT
	};

	enum Dir {
		LEFT,
		RIGHT
	};

	Enemy();
	virtual ~Enemy();

	bool Awake();

	virtual bool Start();

	virtual bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB) override;

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB) override;

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	void ResetPath();

	void AddAnimation(Animation& anim, int startPosY, int frameSize, int numFrames);

	bool CheckIfTwoPointsNear(Vector2D point1, Vector2D point2, float nearDistance);

	void SetPlayer(Player* player);
	
	void SetPath(pugi::xml_node pathNode);

	virtual void SaveData(pugi::xml_node enemyNode);

	virtual void LoadData(pugi::xml_node enemyNode);

	virtual void Restart();

	virtual void DMGEnemyPickaxe();
	virtual void DMGEnemyMelee();

	void DropLoot();

public:

//protected:
	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node audioNode;
	
	PhysBody* pbody;
	Player* player;

	//STATES
	State state;
	Dir dir;

	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation attack;
	Animation hurt;
	Animation death;
	Animation jump;
	Animation fall;
	Animation walk;
	Animation run;
	Animation tired;

	//PATH
	Pathfinding* pathfinding;
	std::vector<Vector2D> route;
	int routeDestinationIndex;
	Vector2D destinationPoint;

	//PROPERTIES
	int speed;
	int lives;
	int chaseArea;
	int attackArea;
	int noSound;

	int pickaxeDamage = 3;
	int meleeDamage = 4;

	Timer deathTimer;
	float deathTime;
	bool damaged = false;
	bool dead;
	bool playingSound;

	float pushForce; //rango �ptimo alrededor de 1
	float pushFriction; //rango �ptimo alrededor de 5
	int lootAmount; //entre 0 y 5

	CurrencyManager* currencyManager;
	bool canPush;
	bool droppedLoot = false;
};