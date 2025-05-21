#include "GuiControlSlider.h"
#include "Engine.h"
#include "Scene.h"
#include "GuiManager.h"
#include "GuiControl.h"
#include "Audio.h"
#include "Window.h"
#include "Textures.h"
#include "Audio.h"
#include "MainMenu.h"
#include <algorithm>

GuiControlSlider::GuiControlSlider(const char* name, const char* text, SDL_Rect circleBounds, SDL_Rect barBounds, SDL_Texture* circleTex, SDL_Texture* barTex) : GuiControl(GuiControlType::SLIDER, id, name)
{
    
    this->texture = circleTex;
    this->sliderTexture = barTex;

    this->active = false;
    
    
    
}

GuiControlSlider::~GuiControlSlider()
{
}

bool GuiControlSlider::Update(float dt)
{
    minValue = sliderBounds.x + 10;
    maxValue = sliderBounds.x + sliderBounds.w - 45;
   
    
    if (state != GuiControlState::DISABLED)
    {
        /*int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);*/
        Vector2D mousePos = Engine::GetInstance().input.get()->GetMousePosition();
        
        // Check collision between mouse and button bounds
        if (mousePos.getX() > sliderBounds.x && mousePos.getX() < sliderBounds.x + sliderBounds.w && mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h && state != GuiControlState::PRESSED)
        {
            state = GuiControlState::FOCUSED;

            
            if (Engine::GetInstance().input.get()->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
            {
                state = GuiControlState::PRESSED;
            }
        }
        else if (state == GuiControlState::PRESSED)
        {
            sliderPosX = mousePos.getX() - bounds.w/2;
            sliderPosX = sliderPosX < minValue ? minValue : sliderPosX;
            sliderPosX = sliderPosX > maxValue ? maxValue : sliderPosX;

            // If mouse button pressed -> Generate event!
            if (Engine::GetInstance().input.get()->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
            {
                state = GuiControlState::NORMAL;
                NotifyObserver();
            }
        }
        else state = GuiControlState::NORMAL;
    }

    SDL_Rect camera = Engine::GetInstance().render.get()->camera;
    int windowScale = Engine::GetInstance().window.get()->GetScale();


    if (active) {
       
        Engine::GetInstance().render.get()->DrawTextureBuffer(sliderTexture, -camera.x / windowScale + sliderBounds.x, -camera.y / windowScale + sliderBounds.y, false,MENUS);
        Engine::GetInstance().render.get()->DrawTextureBuffer(texture, -camera.x / windowScale + sliderPosX, -camera.y / windowScale + sliderBounds.y - bounds.h/4, false,MENUS);
    }


    volumeValue = ((sliderPosX - minValue) / (float)(maxValue - minValue)) * MIX_MAX_VOLUME;
    /*volumeValue = std::max(0.0f, std::min(100.0f, volumeValue));*/

    return false;
}

void GuiControlSlider::SetVolumeValue(int value)
{
    minValue = sliderBounds.x + 10;
    maxValue = sliderBounds.x + sliderBounds.w - 10;
    volumeValue = value;
    sliderPosX = (volumeValue / (float)MIX_MAX_VOLUME) * (maxValue - minValue) + minValue;
}




