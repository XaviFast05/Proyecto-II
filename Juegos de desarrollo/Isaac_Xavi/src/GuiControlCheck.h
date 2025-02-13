#pragma once

#include "GuiControl.h"
#include "Vector2D.h"

class GuiControlCheck : public GuiControl
{
public:
    GuiControlCheck(int id, SDL_Rect bounds, const char* text);
    virtual ~GuiControlCheck();

    bool Update(float dt);

    bool isChecked() const { return inCheck; }

    bool drawBasic = false;
    bool click = false;
    bool inCheck = false;

private:

    //Colors for the button
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color black = { 0, 0, 20, 0 };
    SDL_Color grey = { 100, 100, 100, 100 };
    SDL_Color yellow = { 255, 255, 0, 255 };

    bool checkCreated = false;
    bool focused = false;
    bool pressed = false;

    //button FX
    int focusedFxId;
    int pressedFxId;

    SDL_Texture* NoCheckNormal = nullptr;
    SDL_Texture* NoCheckHover = nullptr;
    SDL_Texture* NoCheckClick = nullptr;
    SDL_Texture* CheckNormal = nullptr;
    SDL_Texture* CheckHover = nullptr;
    SDL_Texture* CheckClick = nullptr;
};

