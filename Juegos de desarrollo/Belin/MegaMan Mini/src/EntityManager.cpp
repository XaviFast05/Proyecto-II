#include "EntityManager.h"
#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Scene.h"
#include "Log.h"
#include "AmmoCharger.h"
#include "LifeCharger.h"
#include "Bullet.h"
#include "MetallC15.h"
#include "Battonbone.h"
#include "tracy/Tracy.hpp"

EntityManager::EntityManager() : Module()
{
	name = "entitymanager";
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake()
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	for(const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->Awake();
	}

	return ret;

}

bool EntityManager::Start() {

	bool ret = true; 

	//Iterates over the entities and calls Start
	for(const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->Start();
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;

	for(const auto entity : entities)
	{
		if (entity->active == false) continue;
		ret = entity->CleanUp();
	}

	entities.clear();

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* entity = nullptr; 

	//L04: TODO 3a: Instantiate entity according to the type and add the new entity to the list of Entities
	switch (type)
	{
	case EntityType::PLAYER:
		entity = new Player();
		break;
	case EntityType::BULLET:
		entity = new Bullet();
		break;
	case EntityType::AMMO_CHARGER:
		entity = new AmmoCharger();
		break;
	case EntityType::LIFE_CHARGER:
		entity = new LifeCharger();
		break;
	case EntityType::POINTS_CONTAINER:
		entity = new PointsContainer();
		break;
	case EntityType::METALLC15:
		entity = new MetallC15();
		break;
	case EntityType::BATTONBONE:
		entity = new BattonBone();
		break;
	default:
		break;
	}

	entities.push_back(entity);

	return entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	for (auto it = entities.begin(); it != entities.end(); ++it)
	{
		if (*it == entity) {
			(*it)->CleanUp();
			delete* it; // Free the allocated memory
			entities.erase(it); // Remove the entity from the list
			break; // Exit the loop after removing the entity
		}
	}
}

void EntityManager::AddEntity(Entity* entity)
{
	if ( entity != nullptr) entities.push_back(entity);
}

bool EntityManager::Update(float dt)
{
	ZoneScoped;

	int camX = Engine::GetInstance().scene.get()->player->GetPosition().getX() - (Engine::GetInstance().render->camera.w / 2);
	int camW = Engine::GetInstance().scene.get()->player->GetPosition().getX() + (Engine::GetInstance().render->camera.w / 2);

	if (camX < 0) {
		camW = camW + (-camX);
		camX = 0;
	}
	
	bool ret = true;
	for(const auto entity : entities)
	{
		// Pause menu activation or lazy updates solution
		if (Engine::GetInstance().scene.get()->pauseEntities || Engine::GetInstance().scene.get()->state != LEVEL1) {
			return ret;
		}

		// Lazy updates controlled
		if (entity->position.getX() + 32 > camX  && entity->position.getX() + 32 < camW)  
		{
			if (entity->active == false) continue;
			ret = entity->Update(dt);
		}



	}
	return ret;
}