#pragma once
#pragma once

#include "Enemy.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "PickaxeManager.h"
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

	int bossJumpTargetXLeft = 2765; //ESTO HAY QUE CAMBIARLO SEG�N DONDE SE QUIERA PONER EL BOSS
	int bossJumpTargetXRight = 3465;

	float jumpStartX = 0.0f;   // X (px) donde empieza el salto
	float jumpTargetX = 0.0f;   // X destino (px)
	float jumpTimeFlight = 0.0f;   // Duraci�n total del salto (s)
	Timer jumpTimer;                // Crono para medir el salto

	//sound FX
	int swordSlashSFX;
	int skeletonDeathSFX;

	// Contador de da�o recibido desde el �ltimo salto
	int damageAccumulated = 0;

	// �Est� ejecutando un salto de boss?
	bool isBossJumping = false;

	// Direcci�n actual del salto: -1 = izquierda, +1 = derecha
	int bossDirection = -1;

	// Velocidad de impulso vertical (altura del salto)
	float bossJumpSpeedV = 700.0f;

	ProjectileManager* projectileManager;
	bool shouldShootProjectiles = false;

};

