#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Pathfinding.h"
#include "Player.h"

struct SDL_Texture;

enum class BulletDirection {
    HORIZONTAL,
    VERTICAL
};

enum class BulletType {
	BOSSJUMP,
	FINALBOSS,
	PICKAXE
};

class Bullet : public Entity
{
public:
    Bullet(BulletType bulletType, BulletDirection direction);
    virtual ~Bullet();

    bool Awake();
    bool Start();
    bool Update(float dt);
    bool CleanUp();

    void SetParameters(pugi::xml_node parameters) {
        this->parameters = parameters;
    }

    void SetPosition(Vector2D pos);
    Vector2D GetPosition();
    void SetDirection(const Vector2D& dir) { direction = dir; }
    void ChangeDirection(BulletDirection bullet_direction);

    void OnCollision(PhysBody* physA, PhysBody* physB);

    void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

    bool isAlive = true;
    int distCounter = 0;

    SDL_RendererFlip hflip = SDL_FLIP_NONE;

public:
    SDL_Texture* texture;
    BulletDirection bullet_direction;
    BulletType bullet_type;

private:

    int dieFxId;

    b2Vec2 bulletfixedpos;
    
    bool stuckOnWall = false;
    bool destroyPickaxe = false;

    bool flipSprite = false;
    const char* texturePath;
    pugi::xml_node parameters;
    int texW, texH;
    Vector2D direction;
    Animation* currentAnimation = nullptr;
    Animation travel;
    PhysBody* pbody;

    Player* player;

    int hitboxWidth = 48;
    int hitboxHeight = 12;
    b2Fixture* fixture;

    bool onPlayer = false;
    bool isActive = true;

    Timer inactiveTimer;
    float inactiveTimerMax = 0.5;
};