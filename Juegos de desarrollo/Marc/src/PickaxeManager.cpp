#include "PickaxeManager.h"

PickaxeManager::PickaxeManager()
{

}

bool PickaxeManager::Start()
{
	maxNumBullets = 3;
	for (int i = 0; i < maxNumBullets; i++)
	{
		Bullet* bullet = (Bullet*)Engine::GetInstance().entityManager->CreatePooledEntities(EntityType::SHOT);
		bullet->ChangeType(BulletType::HORIZONTAL);
		bullet->SetParameters(Engine::GetInstance().scene.get()->configParameters);
		bullet->texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/bala.png");
		bullet->Start();
	}

	return true;
}

bool PickaxeManager::Update(float dt)
{
	return true;
}

bool PickaxeManager::CleanUp()
{
	return true;
}

void PickaxeManager::ThrowPickaxe(Vector2D Direction, Vector2D Position)
{
	Bullet* bullet = (Bullet*)Engine::GetInstance().entityManager->GetPooledEntity(EntityType::SHOT);
	Vector2D bulletPosition = Position;
	bulletPosition.setX(bulletPosition.getX() + (Direction.getX() * 20));
	bullet->SetPosition(bulletPosition);
	bullet->SetDirection(Direction);
	bullet->Enable();
}