#pragma once

#include "Enemy.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "PickaxeManager.h"
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
	bool attacked = false;
    
    // Render & animación
    int drawOffsetX = 0, drawOffsetY = 0;
    Animation idle, walk, jump, attack, death;
    Animation* currentAnimation = nullptr;

    // Movimiento / física
    float speed = 0.0f;
    float bossJumpSpeedV = 700.0f;  // altura del salto en px/s
    int bossJumpTargetXLeft = 9635;
    int bossJumpTargetXRight = 10265;
    int bossDirection = -1;         // -1 = salta a la izq, +1 = a la der
    bool isBossJumping = false;

    // Contador de impactos
    int damageAccumulated = 0;

    // Para disparar al aterrizar
    ProjectileManager* projectileManager = nullptr;
    bool shouldShoot = false;

    // Sonidos
    int swordSlashSFX = 0, skeletonDeathSFX = 0;
};