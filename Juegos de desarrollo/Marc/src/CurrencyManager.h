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

	Entity* CreateOrb(int size);

	void EnableOrb(float x, float y, int size);

private:

	int currencyNum;
	int orbNum;
};

