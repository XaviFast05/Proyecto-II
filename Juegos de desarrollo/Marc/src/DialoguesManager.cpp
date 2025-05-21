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

        int maxLetters = textTimer.ReadMSec() / (int)(nextLetterTime * 1000);
        int totalVisibleLetters = 0;

        if (skipLetterByLetter)
        {
            maxLetters = INT_MAX;
        }


        //CREAR LINEAS CARACTER A CARACTER
        for (size_t i = 0; i < textToShow.size(); )
        {
            if (totalVisibleLetters >= maxLetters)
                break;

            int charLen = GetUtf8CharLen((unsigned char)textToShow[i]);
            std::string utf8Char = textToShow.substr(i, charLen);
            wordBuffer += utf8Char;
            i += charLen;
            totalVisibleLetters++;

            int textW = 0, textH = 0;
            std::string tempLine = currentLine + wordBuffer;
            TTF_SizeUTF8(textFont, tempLine.c_str(), &textW, &textH);

            if (textW > textMaxW - 20)
            {
                if (!currentLine.empty())
                    lines.push_back(currentLine);

                currentLine = wordBuffer;
                wordBuffer = "";
            }

            if (utf8Char == " ")
            {
                currentLine += wordBuffer;
                wordBuffer = "";
            }
        }

        currentLine += wordBuffer;
        if (!currentLine.empty())
            lines.push_back(currentLine);

        //PASAR O COMPLETAR CON EL ENTER
        if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
        {
            int totalChars = CountUtf8Chars(textToShow);
            if (!skipLetterByLetter && totalVisibleLetters < totalChars)
            {
                skipLetterByLetter = true;
            }
            else
            {
                NextVignette();
            }
        }

        //DRAW DEL DIALOGO
        Vector2D rectPos = {
            -(float)Engine::GetInstance().render.get()->camera.x + rectangleTextureX,
            -(float)Engine::GetInstance().render.get()->camera.y + rectangleTextureY
        };

        Engine::GetInstance().render.get()->DrawTextureBuffer(rectangleTexture, rectPos.getX(), rectPos.getY(), false, HUD);
        Engine::GetInstance().render.get()->DrawTextureBuffer(images[vignetteParameters.attribute("talker").as_string()], rectPos.getX() + imageOffsetX, rectPos.getY() + imageOffsetY, false, HUD);

        for (int i = 0; i < lines.size(); i++)
        {
            int lineW = 0, lineH = 0;
            TTF_SizeUTF8(textFont, lines[i].c_str(), &lineW, &lineH);

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
