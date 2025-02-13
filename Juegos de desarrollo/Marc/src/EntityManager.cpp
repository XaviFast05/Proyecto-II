#include "EntityManager.h"
#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Scene.h"
#include "Log.h"
#include "Pumpkin.h"
#include "Candy.h"
#include "BatEnemy.h"
#include "GroundEnemy.h"
#include "Particle.h"
#include "Santa.h"

EntityManager::EntityManager(bool startEnabled) : Module(startEnabled)
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
		/*if (entity->active == false) continue;*/
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
	LOG("Cleaning entities");

	for (auto entity : entities)
	{
		if (!entity->CleanUp()) {
			LOG("Error cleaning up entity");
			ret = false;
		}

		delete entity;
	}

	entities.clear();

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* entity = nullptr; 

	switch (type)
	{
	case EntityType::PLAYER:
		entity = new Player();
		break;
	case EntityType::CANDY:
		entity = new Candy();
		break;
	case EntityType::PUMPKIN:
		entity = new Pumpkin();
		break;
	case EntityType::ENEMY:
		entity = new Enemy();
		break;
	case EntityType::BAT_ENEMY:
		entity = new BatEnemy();
		break;
	case EntityType::GROUND_ENEMY:
		entity = new GroundEnemy();
		break;
	case EntityType::BOSS:
		entity = new Santa();
		break;
	case EntityType::SHOT:
		entity = new Particle();
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
	bool ret = true;
	for(const auto entity : entities)
	{
		if (entity->active == false) continue;

		
		ret = entity->Update(dt);
	}
	return ret;
}