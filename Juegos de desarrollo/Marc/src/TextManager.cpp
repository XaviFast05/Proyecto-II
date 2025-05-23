#include "TextManager.h"

TextManager::TextManager(bool startEnabled) : Module(startEnabled)
{
    name = "textmanager";
}
TextManager::~TextManager()
{

}

bool TextManager::Awake()
{

    pugi::xml_document textsFile;
    pugi::xml_parse_result result = textsFile.load_file("texts.xml");

    for (pugi::xml_node textNode : textsFile.child("Texts").children())
    {
        std::string key = textNode.attribute("id").as_string();
        std::vector<std::string> versions;

        for (pugi::xml_node versionNode : textNode.children())
        {
            versions.push_back(versionNode.text().as_string());
        }

        if (versions.size() < 3) LOG("Incorrect xml format in text %", key.c_str());

        idToText.emplace(key, versions);
    }
    return true;
}

bool TextManager::Update(float dt)
{
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_DOWN)
    {
        languageIndex--;
        if (languageIndex < 0) languageIndex = 2;
    }
    else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
    {
        languageIndex++;
        if (languageIndex > 2) languageIndex = 0;
    }
    return true;
}

void TextManager::ChangeIdiom(int idiom)
{
    languageIndex = idiom;
}

std::string TextManager::GetText(std::string id)
{
    if (idToText[id].empty())
        return "";
    return idToText[id][languageIndex];
}