#include "Enemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"
#include "EntityManager.h"
#include "tracy/Tracy.hpp"

Enemy::Enemy() : Entity(EntityType::ENEMY), pathfinding(nullptr), pbody(nullptr) {}

Enemy::~Enemy() {
	if (pathfinding != nullptr) {
		delete pathfinding;
	}
}

bool Enemy::Awake() {
	return true;
}

// configurar al enemy
bool Enemy::Start() {
	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();
	idle.LoadAnimations(parameters.child("animations").child("idle"));
	die.LoadAnimations(parameters.child("animations").child("die"));
	currentAnimation = &idle;
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 4, (int)position.getY() + texH / 4, texH / 4, bodyType::DYNAMIC);
	if (pbody == nullptr) {
		LOG("Error al crear el cuerpo físico del enemigo");
		return false;
	}
	pbody->ctype = ColliderType::ENEMY;
	pbody->listener = this;
	if (!parameters.attribute("gravity").as_bool()) pbody->body->SetGravityScale(0);
	deathSfx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/enemy_death.wav");
	pbody->body->SetLinearVelocity(b2Vec2(0, 0));
	pathfinding = new Pathfinding();
	ResetPath();
	isalive = true;
	return true;
}

void Enemy::MoveTowardsTargetTile(float dt) {
	if (pathfinding == nullptr || pathfinding->pathTiles.size() < 2) return;
	
	//si el player está a más de 20 tiles de distancia, el enemigo se queda quieto
	if (pathfinding->pathTiles.size() > 20) {
		pbody->body->SetLinearVelocity(b2Vec2(0, 0));
		return;
	}

	//coger el penúltimo tile de la lista de tiles para moverse hacia él
	auto it = pathfinding->pathTiles.end();
	Vector2D targetTile = *(--(--it));
	Vector2D targetWorldPos = Engine::GetInstance().map.get()->MapToWorld(targetTile.getX(), targetTile.getY());
	Vector2D enemyPos = GetPosition();
	Vector2D direction = targetWorldPos + Vector2D(16, 16) - enemyPos;
	float distance = direction.Length();
	Vector2D velocity(0, 0);
	if (distance > 0.5f) {
		direction.Normalize();
		velocity = direction * 100.0f;
	}
	b2Vec2 velocityVec = b2Vec2(PIXEL_TO_METERS(velocity.getX()), PIXEL_TO_METERS(velocity.getY()));
	pbody->body->SetLinearVelocity(velocityVec);
}

bool Enemy::Update(float dt) {
	if (active) {
		ZoneScoped;
		ResetPath();
		if (!isDying) {
			while (pathfinding->pathTiles.empty()) {
				pathfinding->PropagateAStar(SQUARED);
			}
			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
				draw = !draw;
			}
			if (draw) {
				pathfinding->DrawPath();
			}
			MoveTowardsTargetTile(dt);
			float velocityX = pbody->body->GetLinearVelocity().x;
			if (velocityX < -0.1f) {
				flipSprite = true;
				hflip = SDL_FLIP_HORIZONTAL;
			}
			else if (velocityX > 0.1f) {
				flipSprite = false;
				hflip = SDL_FLIP_NONE;
			}
		}

		if (isDying) {
			b2Vec2 currentVelocity = pbody->body->GetLinearVelocity();
			pbody->body->SetLinearVelocity(b2Vec2(0, currentVelocity.y));
		}

		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame(), hflip);
		currentAnimation->Update();
		return true;
	}
}

bool Enemy::CleanUp() {
	if (pbody != nullptr) {
		Engine::GetInstance().physics.get()->DeletePhysBody(pbody);
	}
	return true;
}

// Aqui A VECES falla, es lo que no entendemos
int Enemy::GetLevel()
{
	if (!isDying) {
		level = parameters.attribute("level").as_int();
	}
	else {
		level = -1;
	}
	
	return level;
}

void Enemy::SetActive(bool var)
{
	active = var;
}

void Enemy::CreateEnemyAtPosition(Vector2D position) {
	Enemy* newEnemy = new Enemy();
	newEnemy->SetPosition(position);
	Engine::GetInstance().entityManager.get()->AddEntity(newEnemy);
}

void Enemy::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 4);
	pos.setY(pos.getY() + texH / 4);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	if (pbody != nullptr) {
		pbody->body->SetTransform(bodyPos, 0);
	}
}

Vector2D Enemy::GetPosition() {
	if (!isalive) {
		return Vector2D(0, 0);
	}
	if (pbody != nullptr) {
		b2Vec2 bodyPos = pbody->body->GetTransform().p;
		Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
		return pos;
	}
	return Vector2D(0, 0);
}

void Enemy::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	if (pathfinding != nullptr) {
		pathfinding->ResetPath(tilePos);
	}
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype) {
	case ColliderType::BULLET:
		if (!isDying) {
			LOG("Collided with bullet - START DYING");
			Engine::GetInstance().audio.get()->PlayFx(deathSfx);
			isDying = true;
			currentAnimation = &die;
			pbody->body->SetGravityScale(1);
		}
		if (isDying && currentAnimation->HasFinished()) {
			LOG("FINISHED - DELETE ENEMY");
			Engine::GetInstance().audio.get()->PlayFx(deathSfx);
			Engine::GetInstance().entityManager.get()->DestroyEntity(this);
			isDying = true;
			die.Reset();
		}
		Engine::GetInstance().entityManager.get()->DestroyEntity(physB->listener);
		break;
	case ColliderType::ENEMYBULLET:
		Engine::GetInstance().entityManager.get()->DestroyEntity(physB->listener);
		break;
	case ColliderType::VOID:
		LOG("Collided with hazard - DESTROY");
		isDying = true;
		Engine::GetInstance().audio.get()->PlayFx(deathSfx);
		Engine::GetInstance().entityManager.get()->DestroyEntity(this);
		break;
	}
}

void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype) {
	case ColliderType::BULLET:
		LOG("Collision hazard");
		break;
	}
}

