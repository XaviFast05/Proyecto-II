#include "Slash.h"
//#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"


Slash::Slash() : Entity(EntityType::SLASH)
{
	name = "Slash";
}

Slash::~Slash() {

}

bool Slash::Awake() {

	position = Vector2D(0, 0);
	return true;
}

bool Slash::Start() {

	texture = Engine::GetInstance().textures.get()->Load(parameters.attribute("texture").as_string());
	position.setX(parameters.attribute("x").as_int());
	position.setY(parameters.attribute("y").as_int());
	texW = parameters.attribute("w").as_int();
	texH = parameters.attribute("h").as_int();

	//Load animations
	slashRight.LoadAnimations(parameters.child("animations").child("slashRight"));
	slashLeft.LoadAnimations(parameters.child("animations").child("slashLeft"));
	currentAnimation = &slashRight;
	isLookingRight = true;

	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);

	pbody->listener = this;

	//pbody->body->SetEnabled(true);

	pbody->ctype = ColliderType::SLASH;

	return true;
}

bool Slash::Update(float dt)
{
	if (Engine::GetInstance().scene.get()->isAttacking()) {

		//pbody->body->SetEnabled(true);

		playerY = PIXEL_TO_METERS(Engine::GetInstance().scene.get()->GetPlayerPosition().getY());
		
		if (Engine::GetInstance().scene.get()->isLookingRight()) {
			currentAnimation = &slashRight;
			playerX = PIXEL_TO_METERS(Engine::GetInstance().scene.get()->GetPlayerPosition().getX())+PIXEL_TO_METERS(32);
			playerPos = { playerX, playerY };
			pbody->body->SetTransform(playerPos, 0);
		}
		else
		{
			currentAnimation = &slashLeft;
			playerX = PIXEL_TO_METERS(Engine::GetInstance().scene.get()->GetPlayerPosition().getX()) - PIXEL_TO_METERS(32);
			playerPos = { playerX, playerY };
			pbody->body->SetTransform(playerPos, 0);
		}
	}
	else {
		pbody->body->SetTransform({ 0,0 }, 0);
	}

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY(), &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();

	return true;
}

bool Slash::CleanUp()
{
	LOG("Cleanup slash");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}


Vector2D Slash::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}

void Slash::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

void Slash::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	case ColliderType::GROUND:
		LOG("Collision GROUND");
		break;
	case ColliderType::SPIKES:
		LOG("Collision SPIKES");
		break;
	case ColliderType::ENEMY:
		LOG("Collision ENEMY");
		break;
	default:
		break;
	}

}