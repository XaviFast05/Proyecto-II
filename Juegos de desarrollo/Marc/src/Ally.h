#pragma once


#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Timer.h"

class Player;

struct SDL_Texture;

class Ally : public Entity
{
public:
	enum Type {
		FLY,
		GROUND
	};

	enum State {
		PATROL,
		INTERACTION
	};

	enum Dir {
		LEFT,
		RIGHT
	};

	Allie();
	virtual ~Allie();

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

	Timer deathTimer;
	float deathTime;
	bool dead;
	bool playingSound;

	float pushForce; //rango óptimo alrededor de 1
	float pushFriction; //rango óptimo alrededor de 5
	int lootAmount; //entre 0 y 5

	CurrencyManager* currencyManager;
	bool canPush;
	bool droppedLoot = false;
};
