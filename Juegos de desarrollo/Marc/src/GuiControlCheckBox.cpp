#include "GuiControlCheckBox.h"
#include "Render.h"
#include "Window.h"
#include "Engine.h"
#include "Audio.h"
#include "Input.h"

GuiControlCheckBox::GuiControlCheckBox(const char* name, SDL_Rect bounds, const char* text, SDL_Texture* boxTex) : GuiControl(GuiControlType::CHECKBOX, id, name)
{
	this->bounds = bounds;
	this->text = text;

	this->texture = boxTex;
	/*this->font = _font;*/

	//canClick = true;
	//drawBasic = false;
	this->active = true;
	state = GuiControlState::NORMAL;
	this->isChecked = false;
}

GuiControlCheckBox::~GuiControlCheckBox()
{

}

bool GuiControlCheckBox::Update(float dt)
{

	// L16: TODO 3: Update the state of the GUiButton according to the mouse position
	if (state != GuiControlState::DISABLED) {
		if (state != GuiControlState::OVERLOADED) {
			Vector2D mousePos = Engine::GetInstance().input.get()->GetMousePosition();

			//If the position of the mouse if inside the bounds of the button 
			if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w && mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h) {

				state = GuiControlState::FOCUSED;

				if (Engine::GetInstance().input.get()->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
				{
					state = GuiControlState::PRESSED;
				}

				// If mouse button pressed -> Generate event!
				if (Engine::GetInstance().input.get()->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
				{

					state = GuiControlState::SELECTED;

					/*NotifyObserver();*/
				}
			}
			else {
				//state = GuiControlState::NORMAL;
			}
		}
	}

	SDL_Rect camera = Engine::GetInstance().render.get()->camera;
	int windowScale = Engine::GetInstance().window.get()->GetScale();

	switch (state)
	{
	case GuiControlState::DISABLED:
		section = { bounds.h + bounds.h, 0, bounds.h, bounds.h };
		break;

	case GuiControlState::OVERLOADED:
		section = { bounds.h + bounds.h, 0, bounds.h, bounds.h };
		break;

	case GuiControlState::NORMAL:

		if (isChecked)
			section = { bounds.h, 0, bounds.h, bounds.h };
		if (!isChecked)
			section = { 0, 0, bounds.h, bounds.h };
		break;

	case GuiControlState::FOCUSED:
		if (isChecked)
			section = { bounds.h, 0, bounds.h, bounds.h };
		if (!isChecked)
			section = { 0, 0, bounds.h, bounds.h };
		break;
		

	case GuiControlState::PRESSED:
		if (isChecked)
			section = { bounds.h, 0, bounds.h, bounds.h };
		if (!isChecked)
			section = { 0, 0, bounds.h, bounds.h };
		break;
		
		break;

	case GuiControlState::SELECTED:

		if (isChecked)
			isChecked = false;
		else if (!isChecked)
			isChecked = true;

		NotifyObserver();
		break;

	default:
		break;
	}
	

	if (active) {
		if (texture != nullptr) {
				
			Engine::GetInstance().render.get()->DrawTextureBuffer(texture, -camera.x / windowScale + bounds.x, -camera.y / windowScale + bounds.y, false, MENUS, &section);
			
		}

		/*Engine::GetInstance().render->DrawText(text.c_str(), bounds.x * 2 + bounds.w / 2, bounds.y * 2 + bounds.h / 2, bounds.w, bounds.h);*/
	}

	return false;
}

void GuiControlCheckBox::SetChecked(bool b)
{
	isChecked = b;
}