#include "GuiControlCheckBox.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"

GuiControlCheckBox::GuiControlCheckBox(int id, SDL_Rect bounds, const char* text, SDL_Texture* texture) : GuiControl(GuiControlType::CHECKBOX, id)
{
	this->bounds = bounds;
	this->text = text;
	this->id = id;
	this->texture = texture;

	activateCheckBox = false;
	drawBasic = false;
	cursorFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/CursorFx.wav");

}

GuiControlCheckBox::~GuiControlCheckBox()
{

}

bool GuiControlCheckBox::Update(float dt)
{
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

		int camX = Engine::GetInstance().render->camera.x;
		int camY = Engine::GetInstance().render->camera.y;

		SDL_Rect section;
		section.x = 0;
		section.w = bounds.w;
		section.h = bounds.h;

		//L16: TODO 4: Draw the button according the GuiControl State
		switch (state)
		{
		case GuiControlState::DISABLED:
			if (activateCheckBox) {
				section.y = bounds.h * 4;
			}
			else {
				section.y = 0;
			}
			break;
		case GuiControlState::NORMAL:
			if (activateCheckBox) {
				section.y = bounds.h * 5;
			}
			else {
				section.y = bounds.h;
			}
			break;
		case GuiControlState::FOCUSED:
			if (activateCheckBox) {
				section.y = bounds.h * 6;
			}
			else {
				section.y = bounds.h* 2;
			}
			break;
		case GuiControlState::PRESSED:
			Engine::GetInstance().audio.get()->PlayFx(cursorFx);
			if (activateCheckBox) {
				section.y = bounds.h * 7;
			}
			else {
				section.y = bounds.h * 3;
			}
			break;
		}

		Engine::GetInstance().render->DrawTexture(texture, bounds.x - camX, bounds.y - camY, &section);
		if (debugMode) {
			switch (state)
			{
			case GuiControlState::DISABLED:
				Engine::GetInstance().render->DrawRectangle(bounds, 200, 200, 200, 255, true, false);
				break;
			case GuiControlState::NORMAL:
				Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 255, 255, true, false);
				break;
			case GuiControlState::FOCUSED:
				Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 20, 255, true, false);
				break;
			case GuiControlState::PRESSED:
				Engine::GetInstance().render->DrawRectangle(bounds, 0, 255, 0, 255, true, false);
				break;
			}
		}


	}

	return false;
}
