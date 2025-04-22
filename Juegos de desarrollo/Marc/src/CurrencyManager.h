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

	Entity* CreateOrb();

	void EnableOrb(float x, float y);

private:

	int currencyNum;
	int orbNum;
};

