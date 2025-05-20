#include "PickaxeManager.h"


ProjectileManager::ProjectileManager()
{

}

bool ProjectileManager::Start()
{
	startNumPickaxe = 3;
	for (int i = 0; i < startNumPickaxe; i++)
	{
		Bullet* bullet = (Bullet*)CreatePickaxe();
	}

	startNumJumpProjectiles = 5;
	for (int j = 0; j < startNumJumpProjectiles; j++)
	{
		Bullet* jumpBullet = (Bullet*)CreateJumpProjectile();
	}

	pickaxeCount = startNumPickaxe;
	return true;
}


bool ProjectileManager::Update(float dt)
{
	//PICKAXE LOGIC
	if (pickaxeCount < maxPickaxes && not recollectingPickaxes) {
		pickaxeRecollectTimer.Start();
		recollectingPickaxes = true;
	}
	if (pickaxeRecollectTimer.ReadSec() >= pickaxeRecollectCount && recollectingPickaxes) {
		pickaxeCount++;
		recollectingPickaxes = false;
	}


	return true;
}

bool ProjectileManager::CleanUp()
{
	return true;
}

void ProjectileManager::ThrowPickaxe(Vector2D Direction, Vector2D Position)
{
	pickaxeCount--;
	pickaxeRecollectTimer.Start();
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
	if (Direction.getX() != 0) bullet->ChangeDirection(BulletDirection::HORIZONTAL);
	else bullet->ChangeDirection(BulletDirection::VERTICAL);
	bullet->Enable(); 
}

Entity* ProjectileManager::CreatePickaxe()
{
	Bullet* bullet = (Bullet*)Engine::GetInstance().entityManager->CreatePooledEntities(EntityType::SHOT);
	bullet->bullet_direction = BulletDirection::HORIZONTAL;
	bullet->SetParameters(Engine::GetInstance().scene.get()->configParameters);
	bullet->texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/bala.png");
	bullet->Start();

	return bullet;
}

void ProjectileManager::ThrowJumpProjectiles(Vector2D position)
{
	// Dirección derecha
	Vector2D dirRight(1, 0);
	Bullet* bulletRight = (Bullet*)Engine::GetInstance().entityManager->GetPooledEntity(EntityType::JUMPSHOT);
	if (!bulletRight)
	{
		bulletRight = (Bullet*)CreateJumpProjectile();
	}
	Vector2D rightPos = position;
	rightPos.setX(rightPos.getX() + 90); // desplazamiento opcional
	rightPos.setY(rightPos.getY() + 50); // desplazamiento opcional
	bulletRight->SetPosition(rightPos);
	bulletRight->SetDirection(dirRight);
	bulletRight->ChangeDirection(BulletDirection::HORIZONTAL);
	bulletRight->Enable();

	// Dirección izquierda
	Vector2D dirLeft(-1, 0);
	Bullet* bulletLeft = (Bullet*)Engine::GetInstance().entityManager->GetPooledEntity(EntityType::JUMPSHOT);
	if (!bulletLeft)
	{
		bulletLeft = (Bullet*)CreateJumpProjectile();
	}
	Vector2D leftPos = position;
	leftPos.setX(leftPos.getX() - 90); // desplazamiento opcional
	leftPos.setY(rightPos.getY()); // desplazamiento opcional
	bulletLeft->SetPosition(leftPos);
	bulletLeft->SetDirection(dirLeft);
	bulletLeft->ChangeDirection(BulletDirection::HORIZONTAL);
	bulletLeft->Enable();
}

Entity* ProjectileManager::CreateJumpProjectile()
{
	Bullet* bullet = (Bullet*)Engine::GetInstance().entityManager->CreatePooledEntities(EntityType::JUMPSHOT);
	bullet->bullet_direction = BulletDirection::HORIZONTAL;
	bullet->SetParameters(Engine::GetInstance().scene.get()->configParameters);
	bullet->texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/balaJumper.png");
	bullet->Start();
	return bullet;
}

void ProjectileManager::ThrowChild(Vector2D position, Vector2D direction)
{
	Bullet* bullet = (Bullet*)Engine::GetInstance().entityManager->GetPooledEntity(EntityType::CHILD);
	if (!bullet)
	{
		bullet = (Bullet*)CreateChild();
	}

	Vector2D bulletPos = position;
	bulletPos.setX(bulletPos.getX() + (direction.getX() * 90)); // Ajusta si hace falta
	bulletPos.setY(bulletPos.getY() + 50); // Ajusta si hace falta

	bullet->SetPosition(bulletPos);
	bullet->SetDirection(direction);
	bullet->ChangeDirection(BulletDirection::HORIZONTAL);
	bullet->Enable();
}

Entity* ProjectileManager::CreateChild()
{
	Bullet* bullet = (Bullet*)Engine::GetInstance().entityManager->CreatePooledEntities(EntityType::CHILD);
	bullet->bullet_direction = BulletDirection::HORIZONTAL;
	bullet->SetParameters(Engine::GetInstance().scene.get()->configParameters);
	bullet->texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/balaEnemy.png");
	bullet->Start();
	return bullet;
}

int ProjectileManager::GetNumPickaxes()
{
	return pickaxeCount;
}

int ProjectileManager::GetNumRed() {
	float timeFactor = pickaxeRecollectTimer.ReadSec() / (pickaxeRecollectCount - 0.2f);
	int num = timeFactor * 8;
	return num;
}