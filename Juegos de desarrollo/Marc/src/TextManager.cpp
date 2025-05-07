#include "TextManager.h"

TextManager::TextManager(bool startEnabled) : Module(startEnabled)
{

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
        std::string key = textNode.attribute("ID").as_string();
        std::vector<std::string> versions;
        versions.push_back(textNode.attribute("ES").as_string());
        versions.push_back(textNode.attribute("CA").as_string());
        versions.push_back(textNode.attribute("EN").as_string());

        if (versions.size() < 3) LOG("Incorrect xml format in text %s", key.c_str());

        idToText.emplace(key, versions);
    }
    return true;
}

bool TextManager::Update()
{
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_K) == KEY_DOWN)
    {
        ChangeIdiom(0);
    }
    else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
    {
        ChangeIdiom(1);
    }
    return true;
}

void TextManager::ChangeIdiom(int idiom)
{
    languageIndex = idiom;
}

std::string TextManager::GetText(std::string id)
{
    return idToText[id][languageIndex];
}