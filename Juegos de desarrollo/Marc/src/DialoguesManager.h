#pragma once
#include "Engine.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "Map"

class DialoguesManager
{
public:

	DialoguesManager();
	virtual ~DialoguesManager() {};

	bool Start();
	bool Update(float dt);

	void NextVignette();
	void StartDialogue(std::string dialogueKey);

	bool CleanUp();

	bool GetOnDialogue();

private:

	//PARAMETERS
	pugi::xml_node configParameters;
	pugi::xml_node vignetteParameters;

	//RECTANGLE
	SDL_Texture* rectangleTexture;
	int rectangleTextureW;
	int rectangleTextureH;
	int rectangleTextureX;
	int rectangleTextureY;

	//IMAGES
	std::map<std::string, SDL_Texture*> images;
	int imageOffsetX;
	int imageOffsetY;

	//TEXT
	std::string textToShow;
	TTF_Font* textFont;
	int textOffsetX;
	int textOffsetY;
	int textMaxW;
	int textMaxH;

	//LOGIC
	bool onDialogue;
	std::string dialogueKey;
	float nextLetterTime;
	Timer textTimer;
	bool skipLetterByLetter;
	
};
