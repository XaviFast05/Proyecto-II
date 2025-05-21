#include "Engine.h"
#include "Window.h"
#include "Render.h"
#include "Log.h"
#include "tracy/Tracy.hpp"

#define VSYNC true



Render::Render(bool startEnabled) : Module(startEnabled)
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

// Called before render is available
bool Render::Awake()
{
	LOG("Create SDL rendering context");
	bool ret = true;

	Uint32 flags = SDL_RENDERER_ACCELERATED;

	//L05 TODO 5 - Load the configuration of the Render module
	if (configParameters.child("vsync").attribute("value").as_bool() == true) {
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}
	int scale = Engine::GetInstance().window.get()->GetScale();

	SDL_Window* window = Engine::GetInstance().window.get()->window;
	renderer = SDL_CreateRenderer(window, -1, flags);

	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = Engine::GetInstance().window.get()->width;
		camera.h = Engine::GetInstance().window.get()->height;
		camera.x = 0;
		camera.y = 0;
	}

	TTF_Init();

	//load a font into memory
	font = TTF_OpenFont("Assets/Fonts/Corvid Conspirator v1.1.ttf", 25);

	return ret;
}

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

bool Render::Update(float dt)
{
	

	return true;
}

bool Render::PostUpdate()
{
	ZoneScoped;

	for (int i = 0; i < MAX_RENDER_LAYERS; i++)
	{
		for (RenderOrder order : zBufferQuery[i])
		{
			int scale = Engine::GetInstance().window.get()->GetScale();

			int drawX = (int)(camera.x * order.speed) + order.x * scale;
			int drawY = (int)(camera.y * order.speed) + order.y * scale;

			switch (order.type) {
				case RenderType::Texture: {
				
					
					SDL_Rect rect;
					rect.x = (int)(camera.x * order.speed) + order.x * scale;
					rect.y = (int)(camera.y * order.speed) + order.y * scale;

					if (order.section.w != 0 && order.section.h != 0)
					{
						rect.w = order.section.w;
						rect.h = order.section.h;
					}
					else
					{
						SDL_QueryTexture(order.texture, NULL, NULL, &rect.w, &rect.h);
					}

					rect.w *= scale;
					rect.h *= scale;

					SDL_Point* p = NULL;
					SDL_Point pivot;

					if (order.pivotX != INT_MAX && order.pivotY != INT_MAX)
					{
						pivot.x = order.pivotX;
						pivot.y = order.pivotY;
						p = &pivot;
					}

					// Usamos srcRect para controlar si pasar nullptr o &order.section
					const SDL_Rect* srcRect = nullptr;
					if (order.section.w != 0 && order.section.h != 0)
					{
						srcRect = &order.section;
					}

					if (!order.flipped)
					{
						if (SDL_RenderCopyEx(renderer, order.texture, srcRect, &rect, order.angle, p, SDL_FLIP_NONE) != 0)
						{
							LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
						}
					}
					else
					{
						if (SDL_RenderCopyEx(renderer, order.texture, srcRect, &rect, order.angle, p, SDL_FLIP_HORIZONTAL) != 0)
						{
							LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
						}
					}
					break;
				}

				case RenderType::Text: {

					DrawTextEx(order.text.c_str(), order.x, order.y, order.width, order.height, order.font, order.color);
					break;
				}
				case RenderType::Rectangle: {

					DrawRectangle(order.rect, order.r, order.g, order.b, order.a, order.filled, order.use_camera);
					break;
				}
				case RenderType::Line: {

					DrawLine(order.x1, order.y1, order.x2, order.y2, order.r, order.g, order.b, order.a, order.use_camera);
					break;
				}
				case RenderType::Circle: {

					DrawCircle(order.x, order.y, order.radius, order.r, order.g, order.b, order.a, order.use_camera);
					break;
				}
			}
		}
		zBufferQuery[i].clear();
	}

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

bool Render::DrawTextureBuffer(SDL_Texture* _texture, int _x, int _y, bool _flipped, RenderLayers _zbuffer, const SDL_Rect* _section, float _speed, double _angle, int _pivotX, int _pivotY)
{
	bool ret = true;

	RenderOrder renderOrder;
	renderOrder.type = RenderType::Texture;
	renderOrder.texture = _texture;
	renderOrder.x = _x;
	renderOrder.y = _y;
	renderOrder.flipped = _flipped;
	renderOrder.zbuffer = (int)_zbuffer;
	renderOrder.speed = _speed;
	renderOrder.angle = _angle;
	renderOrder.pivotX = _pivotX;
	renderOrder.pivotY = _pivotY;

	if (_section != nullptr) {
		
		renderOrder.section = *_section;
	}
	else {
		renderOrder.section = { 0, 0, 0, 0 };
	}

	zBufferQuery[_zbuffer].push_back(renderOrder);

	return ret;
}

bool Render::DrawRectangleBuffer(const SDL_Rect& _rect, Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a, RenderLayers _zbuffer,bool _filled, bool _use_camera)
{
	bool ret = true;
	RenderOrder renderOrder;
	renderOrder.type = RenderType::Rectangle;
	renderOrder.rect = _rect;
	renderOrder.r = _r;
	renderOrder.g = _g;
	renderOrder.b = _b;
	renderOrder.a = _a;
	renderOrder.zbuffer = (int)_zbuffer;
	renderOrder.filled = _filled;
	renderOrder.use_camera = _use_camera;

	zBufferQuery[_zbuffer].push_back(renderOrder);

	
	
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

	if (!use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawLineBuffer(int _x1, int _y1, int _x2, int _y2, Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a, RenderLayers _zbuffer, bool _use_camera)
{
	bool ret = true;

	RenderOrder renderOrder;
	renderOrder.type = RenderType::Line;
	renderOrder.x1 = _x1;
	renderOrder.x2 = _x2;
	renderOrder.y1 = _y1;
	renderOrder.y2 = _y2;
	renderOrder.r = _r;
	renderOrder.g = _g;
	renderOrder.b = _b;
	renderOrder.a = _a;
	renderOrder.use_camera;
	renderOrder.zbuffer = (int)_zbuffer;

	zBufferQuery[_zbuffer].push_back(renderOrder);

	return ret;
}

bool Render::DrawTexture(SDL_Texture* texture, int x, int y, const SDL_Rect* section, bool fliped, float speed, double angle, int pivotX, int pivotY) const
{
	bool ret = true;
	int scale = Engine::GetInstance().window.get()->GetScale();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * scale;
	rect.y = (int)(camera.y * speed) + y * scale;

	if (section != NULL)
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

	if (pivotX != INT_MAX && pivotY != INT_MAX)
	{
		pivot.x = pivotX;
		pivot.y = pivotY;
		p = &pivot;
	}

	if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, fliped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
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

bool Render::DrawCircleBuffer(int _x, int _y, int _radius, Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a, RenderLayers _zbuffer, bool _use_camera)
{

	bool ret = true;

	RenderOrder renderOrder;
	renderOrder.type = RenderType::Circle;
	renderOrder.x = _x;
	renderOrder.y = _y;
	renderOrder.radius = _radius;
	renderOrder.r = _r;
	renderOrder.g = _g;
	renderOrder.b = _b;
	renderOrder.a = _a;
	renderOrder.zbuffer = (int)_zbuffer;
	renderOrder.use_camera = _use_camera;

	zBufferQuery[_zbuffer].push_back(renderOrder);

	return ret;
}

//Frustum culling
bool Render::InCameraView(int x, int y, int w, int h)
{
	bool inView = false;
	int cameraConvertedX = camera.x / -Engine::GetInstance().window.get()->scale;
	int cameraConvertedY = camera.y / -Engine::GetInstance().window.get()->scale;

	int limitX = cameraConvertedX + camera.w / Engine::GetInstance().window.get()->scale;
	int limitY = cameraConvertedY + camera.h / Engine::GetInstance().window.get()->scale;

	cameraConvertedX -= w;
	cameraConvertedY -= h;

	if (x > cameraConvertedX - IN_CAMERA_VIEW_MARGIN && x < limitX + IN_CAMERA_VIEW_MARGIN)
	{
		if (y > cameraConvertedY - IN_CAMERA_VIEW_MARGIN && y < limitY + IN_CAMERA_VIEW_MARGIN)
		{
			inView = true;
		}
	}

	return inView;
}



bool Render::DrawText(const char* text, int posx, int posy, int w, int h) const
{

	SDL_Color color = { 255, 255, 255 };
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { posx, posy, w, h };

	SDL_RenderCopy(renderer, texture, NULL, &dstrect);

	return true;
}


bool Render::DrawTextEx(const char* text, int posx, int posy, int w, int h, TTF_Font* _font, SDL_Color _color) const
{
	int scale = Engine::GetInstance().window.get()->GetScale();
	
	SDL_Surface* surface = TTF_RenderText_Solid(_font, text, _color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	int texW, texH;
	/*texW *= scale;
	texH *= scale;*/
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { posx*scale, posy*scale, w*scale, h*scale };

	SDL_RenderCopy(renderer, texture, NULL, &dstrect);

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);

	return true;
}

bool Render::DrawTextToBuffer(const std::string& text, int x, int y, int w, int h, TTF_Font* font, SDL_Color color, RenderLayers layer, float speed) {
	
	RenderOrder order;
	order.type = RenderType::Text;
	order.text = text;
	order.x = x;
	order.y = y;
	order.width = w;
	order.height = h;
	order.font = font;
	order.color = color;
	order.speed = speed;
	order.zbuffer = (int)layer;

	zBufferQuery[layer].push_back(order);
	return true;
}
