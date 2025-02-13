#pragma once

#include "Module.h"
#include "SDL2/SDL_mixer.h"
#include <list>

#define DEFAULT_MUSIC_FADE_TIME 1.0f
#define MAX_FX 600

struct _Mix_Music;

class Audio : public Module
{
public:

	Audio(bool startEnabled);

	// Destructor
	virtual ~Audio();

	// Called before render is available
	bool Awake();

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fadeTime = DEFAULT_MUSIC_FADE_TIME);
	bool PlayMusic(_Mix_Music* music, float fadeTime = DEFAULT_MUSIC_FADE_TIME);
	bool StopMusic(float fadeTime = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(int fx, int repeat = 0, int channel = 0);

private:

	_Mix_Music* music;
	std::list<Mix_Chunk*> fx;
	


	// An array of all the loaded sound effects
	// Allows us to keep track of all sound fx and handle them through indices
	Mix_Chunk* soundFx[MAX_FX] = { nullptr };
};
