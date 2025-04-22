#pragma once
#include "Entity.h"
#include "SDL2/SDL.h"
#include "Animation.h"
#include "Player.h"

class CurrencyOrb : public Entity
{
public:
    CurrencyOrb();  // Modificar el constructor para aceptar BulletType
    virtual ~CurrencyOrb() {};

    bool Start(bool create);
    bool Update(float dt);
    bool CleanUp();

    void SetParameters(pugi::xml_node parameters) {
        this->parameters = parameters;
    }

    void SetPosition(Vector2D pos);
    void SetSize(int size_);
    void CreateBody();

    void OnCollision(PhysBody* physA, PhysBody* physB);

    bool isAlive = true;
    int distCounter = 0;

    SDL_RendererFlip hflip = SDL_FLIP_NONE;

public:
    SDL_Texture* texture;

private:

    //BASE PARAMETERS
    const char* texturePath;
    pugi::xml_node parameters;
    int texW, texH;
    int orbSize;

    //PROPERTIES
    Vector2D direction;
    Vector2D currentDirection;
    float velocity;
    float smoothingFactor;
    float distToStartMoving;
    bool disable;
    bool bodyCreated = false;

    //MODULES
    Animation* currentAnimation = nullptr;
    PhysBody* pbody;
    Player* player;
};