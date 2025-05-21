#include "GuiControlButton.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"
#include "Input.h"
#include "MainMenu.h"
#include "Window.h"
#include "TextManager.h"

GuiControlButton::GuiControlButton(const char* name, SDL_Rect bounds, const char* text, SDL_Texture* btTex) : GuiControl(GuiControlType::BUTTON, id, name)
{
	this->bounds = bounds;
	this->text = text;
	
	this->texture = btTex;
	/*this->font = _font;*/

	canClick = true;
	drawBasic = false;
	this->active = true;
	/*state = GuiControlState::NORMAL;*/
	
}

GuiControlButton::~GuiControlButton()
{

}

bool GuiControlButton::Update(float dt)
{
	/*if (state != GuiControlState::DISABLED)*/
	
	// L16: TODO 3: Update the state of the GUiButton according to the mouse position
	if (state != GuiControlState::DISABLED) {
		Vector2D mousePos = Engine::GetInstance().input.get()->GetMousePosition();

		//If the position of the mouse if inside the bounds of the button 
		if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w && mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h) {

			state = GuiControlState::FOCUSED;

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
				state = GuiControlState::PRESSED;
			}

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				NotifyObserver();
			}
		}
		else {
			state = GuiControlState::NORMAL;
		}
	}
	

	SDL_Rect camera = Engine::GetInstance().render.get()->camera;
	int windowScale = Engine::GetInstance().window.get()->GetScale();

	//L16: TODO 4: Draw the button according the GuiControl State
	switch (state)
	{
	case GuiControlState::DISABLED:
		section = { 0, bounds.h, bounds.w, bounds.h};
		break;
	case GuiControlState::NORMAL:
		section = { 0, 0, bounds.w, bounds.h };
		break;
	case GuiControlState::FOCUSED:
		section = { 0, bounds.h * 2,  bounds.w, bounds.h };
		break;
	case GuiControlState::PRESSED:
		section = { 0, bounds.h * 3,  bounds.w, bounds.h };
		break;
	}


	if (active) {
		if (texture != nullptr) {
			Engine::GetInstance().render.get()->DrawTextureBuffer(texture, -camera.x / windowScale + bounds.x, -camera.y / windowScale + bounds.y, false, MENUS,&section);
			
		}
		if (text != "")
		{
			int textW = 0, textH = 0;
			TTF_SizeUTF8(font, Engine::GetInstance().textManager.get()->GetText(text).c_str(), &textW, &textH);

			Engine::GetInstance().render.get()->DrawTextToBuffer(
				Engine::GetInstance().textManager.get()->GetText(text).c_str(),
				bounds.x + (bounds.w/2) - textW /2,
				bounds.y + (bounds.h / 2) - textH / 2,
				textW,
				textH,
				font,
				{ 255, 255, 255, 255 }, MENUS
			);
		}

		/*Engine::GetInstance().render->DrawText(text.c_str(), bounds.x * 2 + bounds.w / 2, bounds.y * 2 + bounds.h / 2, bounds.w, bounds.h);*/
	}

	return false;
}