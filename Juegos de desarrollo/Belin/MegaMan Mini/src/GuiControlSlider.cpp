#include "GuiControlSlider.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"

GuiControlSlider::GuiControlSlider(int id, SDL_Rect bounds, SDL_Rect sliderBounds, const char* text, SDL_Texture* texture) : GuiControl(GuiControlType::SLIDER, id)
{
	this->bounds = bounds;
	this->text = text;
	this->id = id;
	this->texture = texture;
	
	slideBounds = sliderBounds;
	canClick = true;
	drawBasic = false;
	clickOffsetX = 0;
	volumePower = 50;

	//cursorFx = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/CursorFx.wav");
}

GuiControlSlider::~GuiControlSlider()
{

}

bool GuiControlSlider::Update(float dt)
{
	if (state != GuiControlState::DISABLED)
	{
		// L16: TODO 3: Update the state of the GUiButton according to the mouse position
		Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

		//If the position of the mouse if inside the bounds of the button 
		if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w && mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h) {

			state = GuiControlState::FOCUSED;

			// Cuando se presiona el botón izquierdo del ratón
			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				state = GuiControlState::PRESSED;

				// Calcula la diferencia inicial entre el ratón y la posición del slider
				clickOffsetX = mousePos.getX() - bounds.x;
			}

			// Cuando se mantiene presionado el botón izquierdo
			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
			{
				state = GuiControlState::PRESSED;

				// Ajusta la posición del slider en base a la posición del ratón y el offset
				bounds.x = mousePos.getX() - clickOffsetX;

				// Asegúrate de que el slider no salga del área definida por slideBounds
				if (bounds.x < slideBounds.x)
					bounds.x = slideBounds.x;

				if (bounds.x + bounds.w > slideBounds.x + slideBounds.w)
					bounds.x = slideBounds.x + slideBounds.w - bounds.w;
				
				// Calcula el volumen como una proporción entre la posición del slider y el espacio disponible
				float normalizedPosition = static_cast<float>(bounds.x - slideBounds.x) / static_cast<float>(slideBounds.w - bounds.w);
				int volume = static_cast<int>(normalizedPosition * 128.0f); // Convertir la posición normalizada en un volumen (0-128)

				// Aplica el volumen según el tipo de slider
				if (text == "MUSIC")
				{
					Engine::GetInstance().audio->SetMusicVolume(volume);
				}
				else if (text == "FX")
				{
					Engine::GetInstance().audio->SetFxVolume(volume);
				}
			}
			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				NotifyObserver();
			}

		}
		else {
			state = GuiControlState::NORMAL;
		}

		Engine::GetInstance().render->DrawRectangle(slideBounds, 255, 255, 255, 255, true, false);

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
			section.y = 0;
			break;
		case GuiControlState::NORMAL:
			section.y = bounds.h;
			break;
		case GuiControlState::FOCUSED:
			section.y = bounds.h * 2;
			break;
		case GuiControlState::PRESSED:
			section.y = bounds.h * 3;
			break;
		}

		Engine::GetInstance().render->DrawTexture(texture, bounds.x - camX, bounds.y - camY, &section);

		switch (state)
		{
		case GuiControlState::DISABLED:
			if (debugMode) Engine::GetInstance().render->DrawRectangle(bounds, 200, 200, 200, 255, true, false);
			break;
		case GuiControlState::NORMAL:
			if (debugMode) Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 255, 255, true, false);
			break;
		case GuiControlState::FOCUSED:
			if (debugMode) Engine::GetInstance().render->DrawRectangle(bounds, 0, 0, 20, 255, true, false);
			break;
		case GuiControlState::PRESSED:
			if (debugMode) Engine::GetInstance().render->DrawRectangle(bounds, 0, 255, 0, 255, true, false);
			break;
		}

	}

	return false;
}
