#include "Engine.h"
#include "Input.h"
#include "Window.h"
#include "Render.h"
#include "Log.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#define VSYNC true

struct Button {
	SDL_Rect rect;
	const char* text;
	SDL_Color color;
	SDL_Color hoverColor;
	bool isHovered;
};

Button buttons[5];

Render::Render() : Module()
{
	name = "render";
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
Render::~Render()
{}

void Render::InitButtons()
{
	int buttonWidth = 200;
	int buttonHeight = 50;
	int startX = 100;
	int startY = 100;
	int spacing = 10;

	//for (int i = 0; i < 5; ++i) {
	//	buttons[i].rect = { startX, startY + i * (buttonHeight + spacing), buttonWidth, buttonHeight };
	//	buttons[i].text = "Button";
	//	buttons[i].color = { 255, 255, 255, 255 }; // Blanco
	//	buttons[i].hoverColor = { 200, 200, 200, 255 }; // Gris claro
	//	buttons[i].isHovered = false;
	//}
}

void Render::HandleHover(int mouseX, int mouseY)
{
	for (int i = 0; i < 5; ++i) {
		if (mouseX >= buttons[i].rect.x && mouseX <= buttons[i].rect.x + buttons[i].rect.w &&
			mouseY >= buttons[i].rect.y && mouseY <= buttons[i].rect.y + buttons[i].rect.h) {
			buttons[i].isHovered = true;
		}
		else {
			buttons[i].isHovered = false;
		}
	}
}

bool Render::DrawText(const char* text, int x, int y, int w, int h, SDL_Color color) const
{
	TTF_Font* font = TTF_OpenFont("path/to/font.ttf", 24);
	if (font == nullptr)
	{
		return false;
	}

	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
	if (surface == nullptr)
	{
		TTF_CloseFont(font);
		return false;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	if (texture == nullptr)
	{
		TTF_CloseFont(font);
		return false;
	}

	SDL_Rect dstRect = { x, y, w, h };
	SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
	SDL_DestroyTexture(texture);
	TTF_CloseFont(font);

	return true;
}

bool Render::Awake()
{
	LOG("Create SDL rendering context");
	bool ret = true;

	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if (configParameters.child("vsync").attribute("value").as_bool() == true) {
		usingVsync = true;
	}
	int scale = Engine::GetInstance().window.get()->GetScale();

	SDL_Window* window = Engine::GetInstance().window.get()->window;
	renderer = SDL_CreateRenderer(window, -1, flags);

	if (renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = Engine::GetInstance().window.get()->width * scale;
		camera.h = Engine::GetInstance().window.get()->height * scale;
		camera.x = 0;
		camera.y = 0;
	}

	InitButtons();

	return ret;
}

bool Render::Update(float dt)
{
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	HandleHover(mouseX, mouseY);

	for (int i = 0; i < 5; ++i) {
		SDL_Color color = buttons[i].isHovered ? buttons[i].hoverColor : buttons[i].color;
		DrawRectangle(buttons[i].rect, color.r, color.g, color.b, color.a, true, false);
		DrawText(buttons[i].text, buttons[i].rect.x, buttons[i].rect.y, buttons[i].rect.w, buttons[i].rect.h, { 0, 0, 0, 255 });
	}

	return true;
}

bool Render::DrawText(const char* text, int x, int y, int w, int h) const
{
	SDL_Color color = { 255, 255, 255, 255 }; // Color blanco
	return DrawText(text, x, y, w, h, color);
}

// Called before render is available


// Called before the first frame
bool Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	return true;
}

// Called each loop iteration
bool Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}



bool Render::PostUpdate()
{
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

void Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

// Blit to screen
bool Render::DrawTexture(SDL_Texture* texture, int x, int y, const SDL_Rect* section, SDL_RendererFlip flip_, float speed, double angle, int pivotX, int pivotY) const
{
	bool ret = true;
	int scale = Engine::GetInstance().window.get()->GetScale();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * scale;
	rect.y = (int)(camera.y * speed) + y * scale;

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivotX != INT_MAX && pivotY != INT_MAX)
	{
		pivot.x = pivotX;
		pivot.y = pivotY;
		p = &pivot;
	}

	if(SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, flip_) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawRectangle(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	int scale = Engine::GetInstance().window.get()->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	int scale = Engine::GetInstance().window.get()->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	int scale = Engine::GetInstance().window.get()->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for(int i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x * scale + camera.x) + (int)(radius * cos(i * factor));
		points[i].y = (int)(y * scale + camera.y) + (int)(radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;

}
