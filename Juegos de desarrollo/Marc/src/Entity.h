#pragma once

#include "Animation.h"
#include "Input.h"
#include "Render.h"

enum class EntityType
{
	PLAYER,
	SHOT,
	PUMPKIN,
	CANDY,
	ENEMY,
	BAT_ENEMY,
	GROUND_ENEMY,
	BOSS,
	CURRENCY_ORB,
	UNKNOWN
};

class PhysBody;

class Entity
{
public:

	Entity(EntityType type) : type(type), active(false) {}
	virtual ~Entity() {}

	virtual bool Awake()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual bool Update(float dt)
	{
		return true;
	}

	virtual bool CleanUp()
	{
		active = false;
		return true;
	}

	void Enable()
	{
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Disable()
	{
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

	virtual void OnCollision(PhysBody* physA, PhysBody* physB) {

	};

	virtual void OnCollisionEnd(PhysBody* physA, PhysBody* physB) {

	};
	
	virtual void SetParameters(pugi::xml_node _parameters) {
		parameters = _parameters;
	}

	virtual void SetInstanceParameters(pugi::xml_node _parameters) {
		instanceParameters = _parameters;
	}



public:

	std::string name;
	EntityType type;
	bool active = false;

	// Possible properties, it depends on how generic we
	// want our Entity class, maybe it's not renderable...
	Vector2D position;       
	bool renderable = false;

	Vector2D GetPosition()
	{
		return position;
	}
	pugi::xml_node parameters;
	pugi::xml_node instanceParameters;

};