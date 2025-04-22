#include "CurrencyManager.h"
#include "CurrencyOrb.h"
#include "Engine.h"
#include "EntityManager.h"
#include "Scene.h"
#include "Textures.h"

CurrencyManager::CurrencyManager()
{
	
}

bool CurrencyManager::Start()
{
	currencyNum = 0;
	orbNum = 20;

	for (int i = 0; i < orbNum; i++)
	{
		CurrencyOrb* orb = (CurrencyOrb*)CreateOrb();
	}

	return true;
}

bool CurrencyManager::CleanUp()
{
	return true;
}

int CurrencyManager::GetCurrency()
{
	return currencyNum;
}

void CurrencyManager::SumCurrency(int currency)
{
	currencyNum += currency;
}

Entity* CurrencyManager::CreateOrb()
{
	CurrencyOrb* orb = (CurrencyOrb*)Engine::GetInstance().entityManager->CreatePooledEntities(EntityType::CURRENCY_ORB);
	orb->SetParameters(Engine::GetInstance().scene.get()->configParameters);
	orb->Start();
	orb->Disable();

	return orb;
}

void CurrencyManager::EnableOrb(float x, float y)
{
	CurrencyOrb* orb = (CurrencyOrb*)Engine::GetInstance().entityManager->GetPooledEntity(EntityType::CURRENCY_ORB);
	orb->SetPosition({x,y});
	orb->Enable();
}
