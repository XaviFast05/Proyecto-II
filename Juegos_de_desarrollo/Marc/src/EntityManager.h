#pragma once

#include "Module.h"
#include "Entity.h"
#include <list>
#include <map>

class EntityManager : public Module
{
public:

	EntityManager(bool startEnabled);

	// Destructor
	virtual ~EntityManager();

	// Called before render is available
	bool Awake();

	// Called after Awake
	bool Start();

	// Called every frame
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Additional methods
	Entity* CreateEntity(EntityType bullet_direction);

	std::list<Entity*> CreatePooledEntities(EntityType bullet_direction, int num);
	Entity* CreatePooledEntities(EntityType bullet_direction);

	Entity* GetPooledEntity(EntityType bullet_direction);

	void DestroyEntity(Entity* entity);

	void AddEntity(Entity* entity);

public:

	std::list<Entity*> entities;
	std::map<EntityType, std::list<Entity*>> pooledEntities;
};
