#include "PickaxeManager.h"

PickaxeManager::PickaxeManager()
{

}

bool PickaxeManager::Start()
{
	startNumPickaxe = 3;
	for (int i = 0; i < startNumPickaxe; i++)
	{
		Bullet* bullet = (Bullet*)CreatePickaxe();
	}

	pickaxeCount = startNumPickaxe;
	return true;
}

bool PickaxeManager::Update(float dt)
{
	//PICKAXE LOGIC
	if (pickaxeCount < MAX_PICKAXES && not recollectingPickaxes) {
		pickaxeRecollectTimer.Start();
		recollectingPickaxes = true;
	}
	if (pickaxeRecollectTimer.ReadSec() >= pickaxeRecollectCount && recollectingPickaxes) {
		pickaxeCount++;
		recollectingPickaxes = false;
	}


	return true;
}

bool PickaxeManager::CleanUp()
{
	return true;
}

void PickaxeManager::ThrowPickaxe(Vector2D Direction, Vector2D Position)
{
	pickaxeCount--;
	Bullet* bullet = (Bullet*)Engine::GetInstance().entityManager->GetPooledEntity(EntityType::SHOT);
	if (!bullet)
	{
		bullet = (Bullet*)CreatePickaxe();
	}
	Vector2D bulletPosition = Position;
	bulletPosition.setX(bulletPosition.getX() + (Direction.getX() * 20));
	bulletPosition.setY(bulletPosition.getY() + (Direction.getY() * -50));
	bullet->SetPosition(bulletPosition);
	bullet->SetDirection(Direction);
	if (Direction.getX() != 0) bullet->ChangeType(BulletType::HORIZONTAL);
	else bullet->ChangeType(BulletType::VERTICAL);
	bullet->Enable(); 
}

Entity* PickaxeManager::CreatePickaxe()
{
	Bullet* bullet = (Bullet*)Engine::GetInstance().entityManager->CreatePooledEntities(EntityType::SHOT);
	bullet->type = BulletType::HORIZONTAL;
	bullet->SetParameters(Engine::GetInstance().scene.get()->configParameters);
	bullet->texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/bala.png");
	bullet->Start();

	return bullet;
}

int PickaxeManager::GetNumPickaxes()
{
	return pickaxeCount;
}


