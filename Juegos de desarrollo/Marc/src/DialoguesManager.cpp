#include "DialoguesManager.h"
#include "Engine.h"
#include "Scene.h"
#include "Textures.h"
#include "vector"
#include "TextManager.h"


DialoguesManager::DialoguesManager()
{

}

bool DialoguesManager::Start()
{
	configParameters = Engine::GetInstance().scene.get()->configParameters.child("dialogues");
	rectangleTexture = Engine::GetInstance().textures.get()->Load(configParameters.child("dialoguePanel").attribute("path").as_string());
	rectangleTextureW = configParameters.child("dialoguePanel").attribute("w").as_int();
	rectangleTextureH = configParameters.child("dialoguePanel").attribute("h").as_int();
	rectangleTextureX = configParameters.child("dialoguePanel").attribute("x").as_int();
	rectangleTextureY = configParameters.child("dialoguePanel").attribute("y").as_int();

	textFont = TTF_OpenFont(configParameters.child("dialoguePanel").child("text").attribute("font").as_string(), configParameters.child("dialoguePanel").child("text").attribute("pointSize").as_int());
	textOffsetX = configParameters.child("dialoguePanel").child("text").attribute("offsetX").as_int();
	textOffsetY = configParameters.child("dialoguePanel").child("text").attribute("offsetY").as_int();
	textMaxW = configParameters.child("dialoguePanel").child("text").attribute("maxW").as_int();
	textMaxH = configParameters.child("dialoguePanel").child("text").attribute("maxH").as_int();

    images.clear();
    for (pugi::xml_node node : configParameters.child("dialoguePanel").child("images").children())
    {
        images[node.attribute("name").as_string()] = Engine::GetInstance().textures.get()->Load(node.attribute("path").as_string());
    }
    imageOffsetX = configParameters.child("dialoguePanel").child("images").attribute("offsetX").as_int();
    imageOffsetY = configParameters.child("dialoguePanel").child("images").attribute("offsetY").as_int();

	nextLetterTime = 0.05;

	return true;
}

bool DialoguesManager::Update(float dt)
{
    if (onDialogue)
    {
        textToShow = Engine::GetInstance().textManager.get()->GetText(vignetteParameters.attribute("textId").as_string());
        std::vector<std::string> lines;
        std::string currentLine = "";
        std::string wordBuffer = "";

        int letterNum = textTimer.ReadMSec() / (int)(nextLetterTime * 1000);
        letterNum = std::min(letterNum, (int)textToShow.size());

        if (skipLetterByLetter)
        {
            letterNum = textToShow.size();
        }
    
        if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
        {
            if (!skipLetterByLetter && letterNum < textToShow.size())
            {
                skipLetterByLetter = true;
                letterNum = textToShow.size();
            }
            else
            {
                NextVignette();
            }
        }

        for (int i = 0; i < letterNum; i++)
        {
            char c = textToShow[i];
            wordBuffer += c;

            int textW = 0, textH = 0;
            std::string tempLine = currentLine + wordBuffer;
            TTF_SizeText(textFont, tempLine.c_str(), &textW, &textH);

            if (textW > textMaxW - 20)
            {
                if (!currentLine.empty()) {
                    lines.push_back(currentLine);
                }
                currentLine = wordBuffer;
                wordBuffer = "";
            }

            if (c == ' ') {
                currentLine += wordBuffer;
                wordBuffer = "";
            }
        }

        // Añadir lo que queda
        currentLine += wordBuffer;
        if (!currentLine.empty()) {
            lines.push_back(currentLine);
        }

        // Posición del rectángulo
        Vector2D rectPos = {
            -(float)Engine::GetInstance().render.get()->camera.x + rectangleTextureX,
            -(float)Engine::GetInstance().render.get()->camera.y + rectangleTextureY
        };

        Engine::GetInstance().render.get()->DrawTextureBuffer(rectangleTexture, rectPos.getX(), rectPos.getY(), false, HUD);
        Engine::GetInstance().render.get()->DrawTextureBuffer(images[vignetteParameters.attribute("talker").as_string()], rectPos.getX() + imageOffsetX, rectPos.getY() + imageOffsetY, false, HUD);

        for (int i = 0; i < lines.size(); i++)
        {
            int lineW = 0, lineH = 0;
            TTF_SizeText(textFont, lines[i].c_str(), &lineW, &lineH);

            Engine::GetInstance().render.get()->DrawTextToBuffer(
                lines[i].c_str(),
                rectangleTextureX + textOffsetX,
                rectangleTextureY + textOffsetY + lineH * i,
                lineW,
                lineH,
                textFont,
                { 255, 255, 255, 255 }, HUD
            );
        }
    }
    return true;
}

void DialoguesManager::StartDialogue(std::string _dialogueKey)
{
    onDialogue = true;
    dialogueKey = _dialogueKey;
    skipLetterByLetter = false;
    vignetteParameters = configParameters.child(dialogueKey.c_str()).first_child();
    textTimer.Start();
    if (!vignetteParameters) onDialogue = false;
}

void DialoguesManager::NextVignette()
{
    vignetteParameters = vignetteParameters.next_sibling();
    textTimer.Start();
    skipLetterByLetter = false;
    if (!vignetteParameters) onDialogue = false;
}

bool DialoguesManager::CleanUp()
{
	return true;
}

bool DialoguesManager::GetOnDialogue()
{
    return onDialogue;
}
