#pragma once

#include "Module.h"
#include "Vector2D.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "list"
#include "map"

#define MAX_RENDER_LAYERS 5

class Render : public Module
{
public:

	struct RenderOrder
	{
		SDL_Texture* texture;
		int x;
		int y;
		const SDL_Rect* section;
		bool flipped;
		int zbuffer;
		float speed;
		double angle;
		int pivotX;
		int pivotY;
	};

	Render(bool startEnabled);

	// Destructor
	virtual ~Render();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void SetViewPort(const SDL_Rect& rect);
	void ResetViewPort();

	// Drawing
	bool DrawTexture(SDL_Texture* texture, int x, int y, bool flip = false, int zBuffer = 0, const SDL_Rect* section = NULL, float speed = 1.0f, double angle = 0, int pivotX = INT_MAX, int pivotY = INT_MAX);
	bool DrawRectangle(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool useCamera = true) const;
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool useCamera = true) const;
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool useCamera = true) const;
	bool DrawText(const char* text, int posx, int posy, int w, int h) const;
	bool DrawTextEx(const char* text, int posx, int posy, int w, int h, TTF_Font* font, SDL_Color color) const;

	// Set background color
	void SetBackgroundColor(SDL_Color color);

	//Frustum culling
	bool InCameraView(int x, int y, int w, int h);

public:

	SDL_Renderer* renderer;
	std::map<int, std::list<RenderOrder>> zBufferQuery;
	SDL_Rect camera;
	SDL_Rect viewport;
	SDL_Color background;
	TTF_Font* font;
};