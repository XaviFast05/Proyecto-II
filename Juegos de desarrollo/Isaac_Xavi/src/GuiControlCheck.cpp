#include "GuiControlCheck.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"
#include "Window.h"
#include "Textures.h"

GuiControlCheck::GuiControlCheck(int id, SDL_Rect bounds, const char* text)
    : GuiControl(GuiControlType::CHECKBOX, id)
{
    this->bounds = bounds;
    this->text = text;
}

GuiControlCheck::~GuiControlCheck()
{
    // Cleanup resources if necessary
    
}

bool GuiControlCheck::Update(float dt)
{
    if (checkCreated == false)
    {
        pressedFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Menu/pressed.wav");
        focusedFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Menu/focused.wav");
        NoCheckNormal = Engine::GetInstance().textures->Load("Assets/Textures/menu/NoCheckNormal.png");
        NoCheckHover = Engine::GetInstance().textures->Load("Assets/Textures/menu/NoCheckHover.png");
        NoCheckClick = Engine::GetInstance().textures->Load("Assets/Textures/menu/NoCheckClicked.png");
        CheckNormal = Engine::GetInstance().textures->Load("Assets/Textures/menu/CheckNormal.png");
        CheckHover = Engine::GetInstance().textures->Load("Assets/Textures/menu/CheckHover.png");
        CheckClick = Engine::GetInstance().textures->Load("Assets/Textures/menu/CheckClicked.png");
        checkCreated = true;
    }

    if (state != GuiControlState::DISABLED)
    {
        Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();
        int scale = Engine::GetInstance().window.get()->GetScale();
        SDL_Rect scaledBounds = { bounds.x * scale, bounds.y * scale, bounds.w * scale, bounds.h * scale };

        if (mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w && mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h)
        {
            state = GuiControlState::FOCUSED;

            if (focused == false)
            {
                Engine::GetInstance().audio->PlayFx(focusedFxId);
                focused = true;
                pressed = false;
            }
            if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
            {
                state = GuiControlState::PRESSED;
                if (pressed == false)
                {
                    Engine::GetInstance().audio->PlayFx(pressedFxId);
                    pressed = true;
                }
            }

			// If mouse button pressed change a bool state
            if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
            {
                inCheck = !inCheck;
                NotifyObserver();
                click = true;
                pressed = false;
            }
        }
        else
        {
            state = GuiControlState::NORMAL;
            pressed = false;
            focused = false;
        }


        switch (state)
        {
        case GuiControlState::DISABLED:
            Engine::GetInstance().render->DrawRectangle(scaledBounds, 200, 200, 200, 128, true, false);
            break;
        case GuiControlState::NORMAL:
			if (inCheck == true)
			{
                
                Engine::GetInstance().render->DrawTexture(CheckNormal, bounds.x + 25, bounds.y - 5);

                if (showBounds == true)
                {
                    Engine::GetInstance().render->DrawRectangle(scaledBounds, 255, 0, 0, 128, true, false);
                }
			}
			else
			{
                Engine::GetInstance().render->DrawTexture(NoCheckNormal, bounds.x + 25, bounds.y - 5);
                if (showBounds == true)
                {
                    Engine::GetInstance().render->DrawRectangle(scaledBounds, 255, 0, 0, 128, true, false);
                }
			}

            break;
        case GuiControlState::FOCUSED:
            if (inCheck == true)
            {
                Engine::GetInstance().render->DrawTexture(CheckHover, bounds.x + 10, bounds.y - 5);
                if (showBounds == true)
                {
                    Engine::GetInstance().render->DrawRectangle(scaledBounds, 255, 0, 0, 128, true, false);
                }
            }
            else
            {
                Engine::GetInstance().render->DrawTexture(NoCheckHover, bounds.x + 10, bounds.y - 5);
                if (showBounds == true)
                {
                    Engine::GetInstance().render->DrawRectangle(scaledBounds, 255, 0, 0, 128, true, false);
                }
            }
            break;
        case GuiControlState::PRESSED:
            if (inCheck == true)
            {
                Engine::GetInstance().render->DrawTexture(CheckClick, bounds.x + 10, bounds.y - 5);
                if (showBounds == true)
                {
                    Engine::GetInstance().render->DrawRectangle(scaledBounds, 255, 0, 0, 128, true, false);
                }
            }
            else
            {
                Engine::GetInstance().render->DrawTexture(NoCheckClick, bounds.x + 10, bounds.y - 5);
                if (showBounds == true)
                {
                    Engine::GetInstance().render->DrawRectangle(scaledBounds, 255, 0, 0, 255, true, false);
                }
            }
            break;
        }

        Engine::GetInstance().render->DrawText(text.c_str(), scaledBounds.x - 200, scaledBounds.y , scaledBounds.w , scaledBounds.h , white);

        if (state == GuiControlState::PRESSED || state == GuiControlState::FOCUSED || state == GuiControlState::NORMAL)
        {
            if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
            {
                showBounds = !showBounds;
            }
        }

    }

    return false;
}

