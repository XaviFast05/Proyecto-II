#pragma once
#include "GuiControl.h"

#include "Vector2D.h"
#include <string.h>

class GuiControlSlider : public GuiControl
{
public:

    GuiControlSlider(const char* name, const char* text, SDL_Rect bounds, SDL_Rect sliderBounds, SDL_Texture* tex, SDL_Texture* sliderTex);
    virtual ~GuiControlSlider();

    bool Update(float dt);
    void SetVolumeValue(int value);
   
    
   /* SDL_Rect sliderBounds;*/
    //PhysBody* button;
    //PhysBody* sliderBar;
    int minValue;
    int maxValue;
        
    float sliderPosX;
    float volumeValue;

private:

    // GuiControlSlider specific properties
    // Maybe some animation properties for state change?
    GuiControlId id;
 

    /*bool playFxOnce = true;*/
};


