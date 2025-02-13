#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"

struct SDL_Texture;

class Enemy : public Entity
{
public:

	Enemy();
	virtual ~Enemy();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void SetParameters(pugi::xml_node parameters) {
		this->parameters = parameters;
	}

	void SetPosition(Vector2D pos);

	Vector2D GetPosition();

	int GetLevel();
	void SetActive(bool var);

	bool isDying = false;

	void ResetPath();
	void OnCollision(PhysBody* physA, PhysBody* physB);
	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);
	void MoveTowardsTargetTile(float dt);
	bool isAlive() const { return isalive; }
	void SetAlive();
	void SetDead();
	void CreateEnemyAtPosition(Vector2D position);
	
	int distCounter = 0;

	SDL_RendererFlip hflip = SDL_FLIP_NONE;
public:

private:
	Vector2D lastEnemyTile;
	Vector2D lastPlayerTile;
	std::vector<Vector2D> pathTiles;  // Almacena la ruta de tiles
	int currentPathIndex = 0;  // Índice actual en la ruta
	float ENEMY_SPEED = 50.0f;
	
	bool isalive = true;
	bool flipSprite = false;
	bool draw = false;

	int level = 0;

	int deathSfx;

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;
	pugi::xml_node parameters;

	Animation* currentAnimation = nullptr;
	Animation idle;
	Animation die;

	PhysBody* pbody;
	Pathfinding* pathfinding;
};
