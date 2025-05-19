#pragma once
#include "vector"
#include "map"
#include "pugixml.hpp"
#include "Engine.h"
#include "Input.h"
#include "Module.h"

class TextManager : public Module
{
private:

    int languageIndex = 0;
    std::map<std::string, std::vector<std::string>> idToText;

    

public:

    TextManager(bool startEnabled);
    ~TextManager();

    bool Awake();
    bool Update(float dt);

    void ChangeIdiom(int idiom);
    std::string GetText(std::string id);
};