#include "GuiSlider.h"
#include "Engine.h"
#include "Scene.h"
#include "GuiManager.h"
#include "GuiControl.h"
#include "Audio.h"
#include "Window.h"
#include "MainMenu.h"

//GuiControlButton::GuiControlButton(const char* name, SDL_Rect bounds, const char* text, SDL_Texture* btTex) : GuiControl(GuiControlType::BUTTON, id, name)
GuiSlider::GuiSlider(const char* name, const char* text, SDL_Rect bounds, SDL_Texture* tex) : GuiControl(GuiControlType::SLIDER, id, name)
{
    this->bounds = bounds;
    this->unit = bounds.w / 100.0f;
    this->texture = tex;
    //if (id == 1)
    //{
    //    value = Engine::GetInstance().guiManager.get()->musicVolume;
    //}
    //else if (id == 2)
    //{
    //    value = Engine::GetInstance().guiManager.get()->fxVolume;
    //}

    value = round(value);
    this->active = false;
    sliderPosX = ((value * unit) + bounds.x) - unit;
}

GuiSlider::~GuiSlider()
{
}

bool GuiSlider::Update(float dt)
{
    if (state != GuiControlState::DISABLED)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Check collision between mouse and button bounds
        if ((mouseX > bounds.x) && (mouseX < (bounds.x + bounds.w)) &&
            (mouseY > bounds.y) && (mouseY < (bounds.y + bounds.h)))
        {
            state = GuiControlState::FOCUSED;

            unit = bounds.w / 100.0f;
            value = (mouseX - bounds.x) / unit;
            value = round(value);


            value2 = (sliderPosX - bounds.x) / unit;
            value2 = round(value2);


            if (value2 < 2)
                value2 = 0;

            if (Engine::GetInstance().input.get()->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT)
            {
                state = GuiControlState::PRESSED;
                sliderPosX = ((value * unit) + bounds.x) - unit - 5;
            }

            // If mouse button pressed -> Generate event!
            if (Engine::GetInstance().input.get()->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
            {
                state = GuiControlState::SELECTED;
            }
        }
        else state = GuiControlState::NORMAL;

        switch (state)
        {
        case GuiControlState::DISABLED:
            break;

        case GuiControlState::NORMAL:

            Engine::GetInstance().render.get()->DrawRectangle(bounds, 0, 100, 50, 255, false, false);
            Engine::GetInstance().render.get()->DrawRectangle({sliderPosX, bounds.y, 20,20}, 200, 200, 200, 255, true, false);

            if (sliderPosX - bounds.x >= 0)
                Engine::GetInstance().render.get()->DrawRectangle({ bounds.x,bounds.y, sliderPosX - bounds.x,20 }, 0, 100, 50, 255, true, false);
            break;

        case GuiControlState::FOCUSED:

            Engine::GetInstance().render.get()->DrawRectangle(bounds, 0, 100, 50, 255, false, false);
            Engine::GetInstance().render.get()->DrawRectangle({ sliderPosX, bounds.y, 20,20 }, 200, 200, 200, 255, true, false);
            if (sliderPosX - bounds.x >= 0)
                Engine::GetInstance().render.get()->DrawRectangle({ bounds.x,bounds.y, sliderPosX - bounds.x,20 }, 0, 100, 50, 255, true, false);

            break;

        case GuiControlState::PRESSED:

            Engine::GetInstance().render.get()->DrawRectangle(bounds, 0, 100, 50, 255, false, false);
            Engine::GetInstance().render.get()->DrawRectangle({ sliderPosX, bounds.y,  20,20 }, 200, 200, 200, 255, true, false);
            if (sliderPosX - bounds.x >= 0)
                Engine::GetInstance().render.get()->DrawRectangle({ bounds.x,bounds.y, sliderPosX - bounds.x,20 }, 0, 100, 50, 255, true, false);

            Engine::GetInstance().scene.get()->lvl1Volume = value2;
            break;

        case GuiControlState::SELECTED:
            /*if (id == 2)*/
            Engine::GetInstance().render.get()->DrawRectangle(bounds, 0, 100, 50, 255, false, false);
            Engine::GetInstance().render.get()->DrawRectangle({ sliderPosX, bounds.y, 20,20 }, 200, 200, 200, 255, true, false);
            if (sliderPosX - bounds.x >= 0)
                Engine::GetInstance().render.get()->DrawRectangle({ bounds.x,bounds.y, sliderPosX - bounds.x,20 }, 0, 100, 50, 255, true, false);

            NotifyObserver();
            break;

        default:
            break;
        }
        Engine::GetInstance().render.get()->DrawText(text.c_str(), bounds.x, bounds.y - 20, bounds.w, bounds.h);
    }

    Engine::GetInstance().scene.get()->lvl1Volume = value2;
    Engine::GetInstance().mainMenu.get()->titleVolume = value2;

    return false;
}

//bool GuiSlider::Draw(Render* render)
//{
//    // Draw the right button depending on state
//
//
//
//    return false;
//}
//
//bool GuiSlider::Draw()
//{
//    int scale = Engine::GetInstance().window.get()->GetScale();
//
//    SDL_Rect drawBounds = SDL_Rect({ bounds.x * scale, bounds.y * scale, bounds.w * scale, bounds.h * scale });
//
//    switch (state)
//    {
//    case GuiControlState::DISABLED:
//        Engine::GetInstance().render.get()->DrawRectangle(drawBounds, 255, 0, 0, 128, true, false);
//        break;
//    case GuiControlState::FOCUSED:
//        Engine::GetInstance().render.get()->DrawRectangle(drawBounds, 0, 255, 0, 128, true, false);
//        break;
//    case GuiControlState::NORMAL:
//        Engine::GetInstance().render.get()->DrawRectangle(drawBounds, 0, 0, 255, 128, true, false);
//        break;
//    case GuiControlState::PRESSED:
//        Engine::GetInstance().render.get()->DrawRectangle(drawBounds, 255, 255, 0, 128, true, false);
//        break;
//    case GuiControlState::SELECTED:
//        Engine::GetInstance().render.get()->DrawRectangle(drawBounds, 0, 255, 255, 128, true, false);
//        break;
//    }
//
//    return true;
//}