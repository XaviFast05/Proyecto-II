#pragma once
#include "vector"
#include "map"
#include "pugixml.hpp"
#include "Engine.h"
#include "Input.h"
#include "Module.h"

class TextManager : Module
{
private:

    int languageIndex = 0;
    std::map<std::string, std::vector<std::string>> idToText;

    TextManager(bool startEnabled);
    virtual ~TextManager();

    bool Awake();
    bool Update();

public:

    void ChangeIdiom(int idiom);
    std::string GetText(std::string id);
};