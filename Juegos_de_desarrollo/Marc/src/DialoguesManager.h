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

	int GetUtf8CharLen(unsigned char c) {
		if (c < 0x80) return 1;
		else if ((c >> 5) == 0x6) return 2;
		else if ((c >> 4) == 0xE) return 3;
		else if ((c >> 3) == 0x1E) return 4;
		return 1;
	}

	int CountUtf8Chars(const std::string& str)
	{
		int count = 0;
		for (size_t i = 0; i < str.size(); )
		{
			int charLen = GetUtf8CharLen((unsigned char)str[i]);
			i += charLen;
			count++;
		}
		return count;
	}

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

	//NAME
	TTF_Font* nameFont;
	int nameOffsetX;
	int nameOffsetY;

	//LOGIC
	bool onDialogue;
	std::string dialogueKey;
	float nextLetterTime;
	Timer textTimer;
	bool skipLetterByLetter;
};
