#pragma once
#include "GuiControl.h"

#include "Vector2D.h"
#include <string.h>

class GuiSlider : public GuiControl
{
public:

    GuiSlider(const char* name, const char* text, SDL_Rect bounds, SDL_Texture* tex);
    virtual ~GuiSlider();

    bool Update(float dt);
    bool Draw();

    float value = 90;
    float value2 = 90;
    int sliderPosX;
    float unit;

private:

    // GuiSlider specific properties
    // Maybe some animation properties for state change?
    GuiControlId id;
    int minValue;
    int maxValue;

    /*bool playFxOnce = true;*/
};


