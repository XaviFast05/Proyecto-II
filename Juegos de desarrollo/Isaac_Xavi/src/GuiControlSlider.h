#pragma once

#include "GuiControl.h"
#include "Vector2D.h"

class GuiControlSlider : public GuiControl
{
public:
    GuiControlSlider(int id, SDL_Rect bounds, const char* text, int minValue, int maxValue, int defaultValue);
    virtual ~GuiControlSlider();

    bool Update(float dt);

    int GetVolume() const { return currentValue; }

    bool canClick = true;
    bool drawBasic = false;
    bool visible = true;

private:
    int minValue;
    int maxValue;
    int currentValue;
    int scale;
	int defaultValue;

    bool sliderCreated;
    bool sliderUpdate;
    bool focused = false;
    bool pressed = false;

	SDL_Rect TempBounds;
    SDL_Rect scaledBounds;
    SDL_Rect sliderBar;
    SDL_Rect slider;
    SDL_Rect visualSlider;

    SDL_Color white = { 255, 255, 255, 255 };

    //Slider FX
    int focusedFxId;
    int pressedFxId;
};
