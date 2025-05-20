#pragma once

#include "Enemy.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "PickaxeManager.h"
#include "Timer.h"

struct SDL_Texture;

class FinalBoss : public Enemy
{
public:

	FinalBoss();
	~FinalBoss();

	bool Start() override;
	bool Update(float dt) override;
	void OnCollision(PhysBody* physA, PhysBody* physB) override;

private:

	// Posición de destino (cuando se usa pathfinding, aunque no se persigue al jugador)
	Vector2D destinationPoint;

	// Movimiento
	float speed = 0.0f;
	bool goingRight = true;
	int leftTargetX = 1275;
	int rightTargetX = 2240;
	bool playerInRange = false;
	bool reverseCycle = false;


	bool hasJumped = false;
	bool hasShot = false;
	bool isJumping = false;
	float jumpForce = 10.5f; // ajusta según sea necesario
	int jumpsDone = 0;



	// Ciclos de movimiento
	int completedCycles = 0;   // Cuántas idas/vueltas se han hecho (máx 2)
	bool isTired = false;

	// Timer de descanso
	Timer tiredTimer;
	float tiredDuration = 5.0f; // segundos

	// Muerte
	float deathTime = 2.0f;
	Timer deathTimer;
	bool droppedLoot = false;

	// Offset para dibujar correctamente
	int drawOffsetX = 0;
	int drawOffsetY = 0;

	ProjectileManager* projectileManager;
};
