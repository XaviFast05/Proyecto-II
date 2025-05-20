#include "DeathMenu.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Audio.h"
#include "Input.h"
#include "FadeToBlack.h"
#include "Log.h"
#include "GuiControl.h"
#include "GuiControlButton.h"
#include "GuiControlSlider.h"
#include "GuiManager.h"
#include "EntityManager.h"
#include "Scene.h"
#include "Settings.h"
#include "Player.h"
#include <string>
#include <format>


DeathMenu::DeathMenu(bool startEnabled) : Module(startEnabled)
{
	name = "deathMenu";
}

DeathMenu::~DeathMenu()
{

}

// Load assets
bool DeathMenu::Start()
{
	LOG("Loading background assets");

	Engine::GetInstance().entityManager.get()->Disable();

	bool ret = true;
	configFile.load_file("config.xml");

	pugi::xml_parse_result result = configFile.load_file("config.xml");
	rootNode = configFile.child("config");
	pugi::xml_node musicNode = rootNode.child("audio").child("music");


	halloweenPixels = TTF_OpenFont("Assets/Fonts/Halloweenpixels.ttf", 100);
	corvidConspirator = TTF_OpenFont("Assets/Fonts/Corvid Conspirator v1.1.ttf", 30);

	pugi::xml_node buttonNode = rootNode.child("scene").child("deathMenu").child("buttons");

	deathButtons.clear();
	for (pugi::xml_node child : buttonNode.children())
	{
		std::string buttonName = child.name();
		GuiControlButton* bt = (GuiControlButton*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::BUTTON, buttonName.c_str(), "", { 0, 0, 0, 0 }, this, { 0,0,0,0 });
		SetGuiParameters(bt, buttonName, buttonNode);
		deathButtons[buttonName] = bt;
	}

	Engine::GetInstance().render.get()->camera.x = 0;
	Engine::GetInstance().render.get()->camera.y = 0;


	Engine::GetInstance().audio.get()->PlayMusic(musicNode.child("loseMus").attribute("path").as_string(), 0.5f);

	candyCount.clear();

	screenTex = Engine::GetInstance().textures.get()->Load(rootNode.child("scene").child("deathMenu").child("bg").attribute("path").as_string());
	candyIcon = Engine::GetInstance().textures.get()->Load(rootNode.child("scene").child("deathMenu").child("icons").child("candy").attribute("path").as_string());
	candyRect.x = rootNode.child("scene").child("deathMenu").child("icons").child("candy").attribute("x").as_int();
	candyRect.y = rootNode.child("scene").child("deathMenu").child("icons").child("candy").attribute("y").as_int();
	candyRect.w = rootNode.child("scene").child("deathMenu").child("icons").child("candy").attribute("w").as_int();
	candyRect.h = rootNode.child("scene").child("deathMenu").child("icons").child("candy").attribute("h").as_int();
	textRect.x = rootNode.child("scene").child("deathMenu").child("icons").child("candy").attribute("textX").as_int();
	textRect.y = rootNode.child("scene").child("deathMenu").child("icons").child("candy").attribute("textY").as_int();
	textRect.w = rootNode.child("scene").child("deathMenu").child("icons").child("candy").attribute("textW").as_int();
	textRect.h = rootNode.child("scene").child("deathMenu").child("icons").child("candy").attribute("textH").as_int();


	return ret;
}

bool DeathMenu::Update(float dt)
{
	
	Engine::GetInstance().render.get()->DrawTextureBuffer(screenTex, 0, 0, false, MENUS);
	Engine::GetInstance().render.get()->DrawTextureBuffer(candyIcon, candyRect.x, candyRect.y, false, MENUS);

	candyCount = std::to_string(finalCandyNum);

	Engine::GetInstance().render.get()->DrawTextEx(candyCount.c_str(), textRect.x, textRect.y, textRect.w, textRect.h, halloweenPixels, {255, 255, 255, 255});

	for (const auto& bt : deathButtons)
	{
		OnGuiMouseClickEvent(bt.second);
		bt.second->Update(dt);
	}
	
	
	if (quit) return false;


	return true;
}

// Update: draw background
bool DeathMenu::PostUpdate()
{
	
	return true;
}

bool DeathMenu::CleanUp() {

	for (const auto& bt : deathButtons) {
		bt.second->active = false;
	}


	return true;
}

bool DeathMenu::OnGuiMouseClickEvent(GuiControl* control) {

	switch (control->id) {
	case GuiControlId::RETRY:
		if (control->state == GuiControlState::PRESSED) {
			Engine::GetInstance().fade.get()->Fade((Module*)this, (Module*)Engine::GetInstance().scene.get(), 30);
			Engine::GetInstance().scene.get()->SetLoadState(true);

		}

		break;
	case GuiControlId::BACKTOTITLE:
		if (control->state == GuiControlState::PRESSED) {
			Engine::GetInstance().fade.get()->Fade((Module*)this, (Module*)Engine::GetInstance().mainMenu.get(), 30);
		}

		break;
	case GuiControlId::QUIT:
		if (control->state == GuiControlState::PRESSED) {
			quit = true;
		}
		break;

	}

	return true;
}

void DeathMenu::SetGuiParameters(GuiControl* bt, std::string btName, pugi::xml_node parameters) {

	bt->id = (GuiControlId)parameters.child(btName.c_str()).attribute("id").as_int();

	bt->bounds.x = parameters.child(btName.c_str()).attribute("x").as_int();
	bt->bounds.y = parameters.child(btName.c_str()).attribute("y").as_int();
	bt->bounds.w = parameters.child(btName.c_str()).attribute("w").as_int();
	bt->bounds.h = parameters.child(btName.c_str()).attribute("h").as_int();

	bt->texture = Engine::GetInstance().textures.get()->Load(parameters.child(btName.c_str()).attribute("texture").as_string());
}
