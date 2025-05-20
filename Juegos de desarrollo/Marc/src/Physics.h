#pragma once
//#include "Module.h"
#include "Entity.h"
#include "box2d/box2d.h"
#include <vector>

#define GRAVITY_X 0.0f
#define GRAVITY_Y -8.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

// types of bodies
enum bodyType {
	DYNAMIC,
	STATIC,
	KINEMATIC
};

enum class ColliderType {
	PLAYER,
	PLAYER_SENSOR,
	WEAPON,
	MELEE_AREA,
	MELEE_AREA_CHARGED,
	SHOT,
	PICKAXE,
	PLATFORM,
	SPYKE,
	ABYSS,
	MAPLIMITS,
	LADDER,
	ENEMY,
	DEADENEMY,
	CHECKPOINT,
	LEVELEND,
	LEVEL_CHANGER,
	BOSS_STARTER,
	CLIMBINGWALL,
	ORB,
	SOUL_ROCK,
	ALLY,
	JUMP,
	FINALBOSS,
	UNKNOWN
};

enum CollisionCategory {
	CATEGORY_PLAYER = 0x0001,
	CATEGORY_PICKAXE = 0x0002,
	CATEGORY_DEFAULT = 0x0004
};

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL), ctype(ColliderType::UNKNOWN)
	{}


	~PhysBody() {}

	Vector2D GetPosition();
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;
	Vector2D GetPhysBodyWorldPosition();
	void SetPhysPositionWithWorld(int x, int y);
	void SetPhysPosition(Vector2D position) {
		body->SetTransform({ position.getX(), position.getY() }, body->GetAngle());
	};

public:
	int width = 0;
	int height = 0;
	b2Body* body;
	Entity* listener;
	ColliderType ctype;
};

// Module --------------------------------------
class Physics : public Module, public b2ContactListener // TODO
{
public:

	// Constructors & Destructors
	Physics(bool startEnabled);
	~Physics();

	// Main module steps
	bool Start();
	bool PreUpdate();
	bool PostUpdate();
	bool CleanUp();

	// Create basic physics objects
	PhysBody* CreateRectangle(int x, int y, int width, int height, bodyType type);
	PhysBody* CreateCircle(int x, int y, int radious, bodyType type);
	PhysBody* CreateCircleSensor(int x, int y, int radious, bodyType type);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, bodyType type);
	PhysBody* CreateChain(int x, int y, int* points, int size, bodyType type);
	
	// b2ContactListener ---
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
	void DeleteAllPhysBody();

	
private:

	// Box2D World
	b2World* world;
	std::vector<PhysBody*> list_physBodies;
};