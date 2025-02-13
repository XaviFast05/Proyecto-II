#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Bullet.h"
#include "Timer.h"

struct SDL_Texture;

class Boss : public Entity {
public:
    Boss();
    virtual ~Boss();

    bool Awake();
    bool Start();
    bool Update(float dt);
    bool CleanUp();

    void SetParameters(pugi::xml_node parameters) {
        this->parameters = parameters;
    }

    void SetPosition(Vector2D pos);
    Vector2D GetPosition();

    void MoveTowardsTargetTile(float dt);
    void ResetPath();
    void OnCollision(PhysBody* physA, PhysBody* physB);
    void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

    int GetLevel();
	void SetActive(bool var);

    bool isAlive() const;
    void SetAlive();
    void SetDead();
    int health = 50;

    bool isDying = false;

private:
    void NormalBehavior(float dt);
    void MoveToPoint(float dt, const Vector2D& target);
    void StayIdle(float dt);
    void ShootMovingToPoint(float dt, const Vector2D& target);
    void UpdatePositionAndRender();
    void Shoot();

    // Variables
    int level = 0;

    int maxHealth = 50;

    float movementSpeed = 200.0f;
    float normalSpeed = 200.0f;
    float angrySpeed = 300.0f;

    bool isShooting = false;
    bool isalive = true;

    bool flipSprite = false;
    bool draw = false;

    int deathSfx;
    int shootFxId;

    float timeSinceLastAction = 0.0f;

    Timer bossTimer;
    Timer bossAngryTimer;

    SDL_Texture* texture = nullptr;
    SDL_RendererFlip hflip = SDL_FLIP_NONE;

    int texW = 0, texH = 0;
    pugi::xml_node parameters;

    Animation* currentAnimation = nullptr;
    Animation idle;
    Animation move;
    Animation shootD;
    Animation die;

    PhysBody* pbody = nullptr;
    Pathfinding* pathfinding = nullptr;
};
