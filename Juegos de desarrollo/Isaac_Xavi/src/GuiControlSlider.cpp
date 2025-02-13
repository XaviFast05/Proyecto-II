#include "GuiControlSlider.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"
#include "Window.h"
#include "Textures.h"

GuiControlSlider::GuiControlSlider(int id, SDL_Rect bounds, const char* text, int minValue, int maxValue, int defaultValue)
    : GuiControl(GuiControlType::SLIDER, id)
{
    this->bounds = bounds;
    this->text = text;

    canClick = true;
    drawBasic = false;
	sliderCreated = false;
	sliderUpdate = false;

	TempBounds = bounds;
	this->minValue = minValue;
	this->maxValue = maxValue;
    this->currentValue = currentValue;
	this->defaultValue = defaultValue;
}

GuiControlSlider::~GuiControlSlider()
{
}

bool GuiControlSlider::Update(float dt)
{
    if (sliderCreated == false)
    {
        // Adjust the scale of the bounds to the screen size
        int scale = Engine::GetInstance().window.get()->GetScale();
        scaledBounds = { TempBounds.x * scale, TempBounds.y * scale, TempBounds.w * scale, TempBounds.h * scale };
        sliderBar = { scaledBounds.x, scaledBounds.y + scaledBounds.h / 2 - 5, scaledBounds.w, 10 };
        slider = { bounds.x + ((defaultValue - minValue) * bounds.w / (maxValue - minValue)) - 5, bounds.y, 10, bounds.h };
        visualSlider = { scaledBounds.x + ((defaultValue - minValue) * scaledBounds.w / (maxValue - minValue)) - 5, scaledBounds.y, 10, scaledBounds.h };
        currentValue = defaultValue;
        pressedFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Menu/pressed.wav");
        focusedFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Menu/focused.wav");
        sliderCreated = true;
    }

    if (state != GuiControlState::DISABLED)
    {
        Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

        if (mousePos.getX() > slider.x && mousePos.getX() < slider.x + slider.w && mousePos.getY() > slider.y && mousePos.getY() < slider.y + slider.h)
        {
            state = GuiControlState::FOCUSED;

            if (state == GuiControlState::FOCUSED)
            {
                if (focused == false)
                {
                    Engine::GetInstance().audio->PlayFx(focusedFxId);
                    focused = true;
                    pressed = false;
                }

            }

            if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
            {
                state = GuiControlState::PRESSED;
                if (pressed == false)
                {
                    Engine::GetInstance().audio->PlayFx(pressedFxId);
                    pressed = true;
                }
				sliderUpdate = true;
            }
            if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
            {
                pressed = false;
            }
        }
        else
        {
            state = GuiControlState::NORMAL;
            pressed = false;
            focused = false;
        }

        if (state == GuiControlState::PRESSED)
        {
            int mouse_x = mousePos.getX() - bounds.x;
            // Clamp the mouse_x within the bounds of the slider
            mouse_x = std::max(0, std::min(mouse_x, bounds.w));

            currentValue = minValue + (mouse_x * (maxValue - minValue) / bounds.w);

            // Clamp the currentValue within minValue and maxValue
            currentValue = std::max(minValue, std::min(currentValue, maxValue));

            slider.x = bounds.x + mouse_x - slider.w / 2;

            NotifyObserver();
        }
        Engine::GetInstance().render->DrawRectangle(sliderBar, 200, 200, 200, 255, true, false);

        switch (state)
        {
        case GuiControlState::NORMAL:
            Engine::GetInstance().render->DrawRectangle(visualSlider, 255, 255, 255, 255, true, false);
            if (showBounds == true)
            {
                Engine::GetInstance().render->DrawRectangle(visualSlider, 255, 0, 0, 128, true, false);
            }
            break;
        case GuiControlState::FOCUSED:
            Engine::GetInstance().render->DrawRectangle(visualSlider, 255, 255, 0, 255, true, false);
            if (showBounds == true)
            {
                Engine::GetInstance().render->DrawRectangle(visualSlider, 255, 0, 0, 128, true, false);
            }
            break;
        case GuiControlState::PRESSED:
            Engine::GetInstance().render->DrawRectangle(visualSlider, 0, 0, 0, 255, true, false);
            if (showBounds == true)
            {
                Engine::GetInstance().render->DrawRectangle(visualSlider, 255, 0, 0, 128, true, false);
            }
            break;
        }

        Engine::GetInstance().render->DrawText(text.c_str(), scaledBounds.x - 200, scaledBounds.y, scaledBounds.w, scaledBounds.h, white);
    }

	// Update the visual slider position
    if (sliderUpdate == true)
    {
        visualSlider = { scaledBounds.x + ((currentValue - minValue) * scaledBounds.w / (maxValue - minValue)) - 5, scaledBounds.y, 10, scaledBounds.h };
    }

    if (state == GuiControlState::PRESSED || state == GuiControlState::FOCUSED || state == GuiControlState::NORMAL)
    {
        if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
        {
            showBounds = !showBounds;
        }
    }


    return false;
}
