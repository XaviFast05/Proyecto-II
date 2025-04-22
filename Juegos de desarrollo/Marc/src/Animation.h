#pragma once

#include "pugixml.hpp"
#include "SDL2/SDL_rect.h"
#define MAX_FRAMES 25

class Animation
{
public:
    float speed = 1.0f;
    SDL_Rect frames[MAX_FRAMES];
    int offsetX[MAX_FRAMES] = { 0 }; // Arreglo para los offsets X
    int offsetY[MAX_FRAMES] = { 0 }; // Arreglo para los offsets Y
    bool loop = true;
    // Allows the animation to keep going back and forth
    bool pingpong = false;

    float currentFrame = 0.0f;
    int totalFrames = 0;
    int loopCount = 0;
    int pingpongDirection = 1;

public:

    void PushBack(const SDL_Rect& rect, int xOffset = 0, int yOffset = 0)
    {
        frames[totalFrames] = rect;
        offsetX[totalFrames] = xOffset;
        offsetY[totalFrames] = yOffset;
        totalFrames++;
    }

    void Reset()
    {
        currentFrame = 0;
    }

    bool HasFinished()
    {
        return !loop && !pingpong && loopCount > 0;
    }

    void Update()
    {
        currentFrame += speed;
        if (currentFrame >= totalFrames)
        {
            currentFrame = (loop || pingpong) ? 0.0f : totalFrames - 1;
            ++loopCount;

            if (pingpong)
                pingpongDirection = -pingpongDirection;
        }
    }

    const SDL_Rect& GetCurrentFrame() const
    {
        int actualFrame = static_cast<int>(currentFrame);
        if (pingpongDirection == -1)
            actualFrame = totalFrames - 1 - actualFrame;

        return frames[actualFrame];
    }

    int GetCurrentOffsetX() const
    {
        int actualFrame = static_cast<int>(currentFrame);
        if (pingpongDirection == -1)
            actualFrame = totalFrames - 1 - actualFrame;

        return offsetX[actualFrame];
    }

    int GetCurrentOffsetY() const
    {
        int actualFrame = static_cast<int>(currentFrame);
        if (pingpongDirection == -1)
            actualFrame = totalFrames - 1 - actualFrame;

        return offsetY[actualFrame];
    }

    void LoadAnimations(pugi::xml_node animationNode)
    {
        speed = animationNode.attribute("speed").as_float();
        loop = animationNode.attribute("loop").as_bool();

        for (pugi::xml_node animation = animationNode.child("frame"); animation; animation = animation.next_sibling("frame"))
        {
            PushBack(
                {
                    animation.attribute("x").as_int(),
                    animation.attribute("y").as_int(),
                    animation.attribute("w").as_int(),
                    animation.attribute("h").as_int()
                },
                animation.attribute("offsetX").as_int(0), // Leer offsetX
                animation.attribute("offsetY").as_int(0)  // Leer offsetY
            );
        }
    }
};