#pragma once

#include "Module.h"
#include "Vector2D.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "list"
#include "map"

#define MAX_RENDER_LAYERS 11
#define IN_CAMERA_VIEW_MARGIN 50



enum RenderLayers
{
	PARALAX,
	BEHIND_MAP,
	BETWEEN_MAP,
	MAP,
	DEFAULT,
	ENTITIES,
	PLAYER,
	FRONT,
	HUD,
	MENUS,
	FADE_LAYER
};

class Render : public Module
{
public:

	enum class RenderType {
		Texture,
		Text,
		Rectangle,
		Line,
		Circle
	};

	struct RenderOrder {
		RenderType type = RenderType::Texture;

		// Común
		int x = 0, y = 0;
		int zbuffer = 0;
		float speed = 1.0f;
		Uint8 r = 255;
		Uint8 g = 255;
		Uint8 b = 255;
		Uint8 a = 255;
		bool filled = true;
		bool use_camera = false;

		// Para textura
		SDL_Texture* texture = nullptr;
		SDL_Rect section = { 0,0,0,0 };
		bool flipped = false;
		double angle = 0;
		int pivotX = INT_MAX, pivotY = INT_MAX;

		// Para texto
		std::string text;
		TTF_Font* font = nullptr;
		SDL_Color color = { 255,255,255,255 };
		int width = 0, height = 0;

		//Para rectangulo
		SDL_Rect rect = { 0,0,0,0 };


		//Para linea
		int x1, x2, y1, y2;

		//Para circulo
		int radius;
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
	
	bool DrawTexture(SDL_Texture* texture, int x, int y, const SDL_Rect* section = nullptr, bool fliped = false, float speed = 1.0f, double angle = 0, int pivotX = INT_MAX, int pivotY = INT_MAX) const;
	bool DrawRectangle(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool useCamera = true) const;
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool useCamera = true) const;
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool useCamera = true) const;
	bool DrawText(const char* text, int posx, int posy, int w, int h) const;
	bool DrawTextEx(const char* text, int posx, int posy, int w, int h, TTF_Font* font, SDL_Color color) const;

	bool DrawRectangleBuffer(const SDL_Rect& rect, Uint8 r = 255, Uint8 g = 255, Uint8 b=255, Uint8 a = 255, RenderLayers = DEFAULT, bool filled = true, bool useCamera = true);
	bool DrawTextureBuffer(SDL_Texture* texture, int x, int y, bool flip = false, RenderLayers zBuffer = DEFAULT, const SDL_Rect* section = nullptr, float speed = 1.0f, double angle = 0, int pivotX = INT_MAX, int pivotY = INT_MAX);
	bool DrawTextToBuffer(const std::string& text, int x, int y, int w, int h, TTF_Font* _font, SDL_Color color = {255,255,255,255}, RenderLayers layer = DEFAULT, float speed = 1.0f);
	bool DrawLineBuffer(int x1, int y1, int x2, int y2, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255, RenderLayers zbuffer = DEFAULT, bool useCamera = true);
	bool DrawCircleBuffer(int x1, int y1, int redius, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255, RenderLayers zbuffer = DEFAULT, bool useCamera = true);
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