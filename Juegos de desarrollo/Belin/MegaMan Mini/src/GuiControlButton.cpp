#include "GuiControlButton.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"

GuiControlButton::GuiControlButton(int id, SDL_Rect bounds, const char* text, SDL_Texture* texture) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;
	this->id = id;
	this->texture = texture;

	canClick = true;
	drawBasic = false;
	textureOnDisabled = false;
	cursorFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/CursorFx.wav");
	//selectedFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/SelectedFx.wav");

}

GuiControlButton::~GuiControlButton()
{

}

bool GuiControlButton::Update(float dt)
{
	int camX = Engine::GetInstance().render->camera.x;
	int camY = Engine::GetInstance().render->camera.y;

	SDL_Rect section;
	section.x = 0;
	section.w = bounds.w;
	section.h = bounds.h;

	if (state != GuiControlState::DISABLED)
	{
		// L16: TODO 3: Update the state of the GUiButton according to the mouse position
		Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

		//If the position of the mouse if inside the bounds of the button 
		if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w && mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h) {

			state = GuiControlState::FOCUSED;

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				state = GuiControlState::PRESSED;
			}

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				NotifyObserver();
			}
		}
		else {
			state = GuiControlState::NORMAL;
		}

		//L16: TODO 4: Draw the button according the GuiControl State
		switch (state)
		{
		case GuiControlState::DISABLED:
			section.y = 0;
			if (debugMode) Engine::GetInstance().render->DrawRectangle(bounds, 200, 200, 200, 255, true, false);
			break;
		case GuiControlState::NORMAL:
			section.y = bounds.h;
			if (debugMode) Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 255, 255, true, false);
			break;
		case GuiControlState::FOCUSED:
			section.y = bounds.h * 2;
			if (debugMode) Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 20, 255, true, false);
			break;
		case GuiControlState::PRESSED:
			section.y = bounds.h * 3;
			if (debugMode) Engine::GetInstance().render->DrawRectangle(bounds, 0, 255, 0, 255, true, false);
			Engine::GetInstance().audio.get()->PlayFx(cursorFx);
			break;
		}

		Engine::GetInstance().render->DrawTexture(texture, bounds.x - camX, bounds.y - camY, &section);


	}
	else {

		if (textureOnDisabled) {
			section.y = 0;
			Engine::GetInstance().render->DrawTexture(texture, bounds.x - camX, bounds.y - camY, &section);
		}

	}

	return false;
}
