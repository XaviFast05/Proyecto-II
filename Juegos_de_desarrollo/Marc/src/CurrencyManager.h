#pragma once

//FORWARD DECLARATION
class Entity;

class CurrencyManager
{
public:

	CurrencyManager();
	virtual ~CurrencyManager() {};

	bool Start();

	bool CleanUp();

	int GetCurrency();

	void SumCurrency(int currency);
	void SetCurrency(int currency);

	Entity* CreateOrb();

	void CreateOrbs(int amount);

	void EnableOrb(float x, float y, int size);

public:

	int currencyNum;
	int orbNum;
};

