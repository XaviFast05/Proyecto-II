#pragma once
#pragma once

#include "Enemy.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include <vector>
#include "Timer.h"



struct SDL_Texture;

class JumpingEnemy : public Enemy
{
public:

	JumpingEnemy();
	~JumpingEnemy();


	bool Start() override;

	bool Update(float dt) override;

	void OnCollision(PhysBody* physA, PhysBody* physB) override;

	void TriggerBossJump();

private:

	int drawOffsetX = 0;
	int drawOffsetY = 0;
	Vector2D destinationPoint;
	Animation walk;
	float attackTime;
	Timer attackTimer;

	bool attacked;

	float dist;

	float jumpForce;

	//sound FX
	int swordSlashSFX;
	int skeletonDeathSFX;

	// Contador de daño recibido desde el último salto
	int damageAccumulated = 0;

	// ¿Está ejecutando un salto de boss?
	bool isBossJumping = false;

	// Dirección actual del salto: -1 = izquierda, +1 = derecha
	int bossDirection = -1;

	// Distancia horizontal del salto: 12 bloques de 64px
	const float bossJumpDistance = 20 * 64.0f;

	// Velocidad de impulso horizontal (en unidades de tu mundo físico)
	float bossJumpSpeedH = 320.0f;    // ajústalo a tu gusto

	// Velocidad de impulso vertical (altura del salto)
	float bossJumpSpeedV = 750.0f;

	// Posición X objetivo del salto (en píxeles)
	float bossJumpTargetX = 0.0f;
};

