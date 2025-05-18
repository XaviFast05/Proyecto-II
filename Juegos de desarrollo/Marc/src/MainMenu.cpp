#include "MainMenu.h"
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




MainMenu::MainMenu(bool startEnabled) : Module(startEnabled)
{
	name = "mainmenu";
}

MainMenu::~MainMenu()
{

}

// Load assets
bool MainMenu::Start()
{
	LOG("Loading background assets");
	
	bool ret = true;
	configFile.load_file("config.xml");
	
	pugi::xml_parse_result configResult = configFile.load_file("config.xml");
	if (configResult == NULL) {
		LOG("Error loading config.xml");
		return false;
	}
	rootNode = configFile.child("config");	
	pugi::xml_node musicNode = rootNode.child("audio").child("music");

	credits = Engine::GetInstance().textures.get()->Load(configParameters.child("credits").attribute("path").as_string());

	btTex = Engine::GetInstance().textures.get()->Load(configParameters.child("buttons").attribute("defaultTex").as_string());
	int texW, texH;
	Engine::GetInstance().textures.get()->GetSize(btTex, texW, texH);

	btFont = TTF_OpenFont("Assets/Fonts/Corvid Conspirator v1.1.ttf", 30);

	pugi::xml_node buttonNode = configFile.child("config").child("mainmenu").child("buttons");

	buttons.clear();
	for (pugi::xml_node child : buttonNode.children())
	{
		std::string buttonName = child.name();
		GuiControlButton* bt = (GuiControlButton*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::BUTTON, buttonName.c_str(), "", { 0, 0, 0, 0 }, this, { 0,0,0,0 });
		SetGuiParameters(bt, buttonName, buttonNode);
		buttons[buttonName] = bt;
	}
	
	Engine::GetInstance().render.get()->camera.x = 0;
	Engine::GetInstance().render.get()->camera.y = 0;

	Engine::GetInstance().audio.get()->PlayMusic(musicNode.child("menuMus").attribute("path").as_string(), 0.5f);

	bgTex = Engine::GetInstance().textures.get()->Load(configParameters.child("bg").attribute("path").as_string());	


	pugi::xml_document loadFile;
	pugi::xml_parse_result savedDataResult = loadFile.load_file("savedData.xml");

	if (savedDataResult == NULL) {
		LOG("Error loading saveData.xml");
		return false;
	}

	saved = loadFile.child("savedData").attribute("saved").as_bool();

	loadFile.save_file("savedData.xml");

	if (!saved)
		buttons["continueBt"]->state = GuiControlState::DISABLED();

	creditsOpen = false;
	buttons["backBt"]->active = false;

	return ret;
}

bool MainMenu::Update(float dt)
{
	_dt = dt;

	Engine::GetInstance().render.get()->DrawTexture(bgTex, 0, 0, NULL);
	if (!Engine::GetInstance().settings.get()->settingsOpen && !creditsOpen)
	{
		for (const auto& bt : buttons)
		{
			OnGuiMouseClickEvent(bt.second);
			if (bt.second->name != "backBt")
				bt.second->Update(dt);
		}
	}

	if (quit) return false;
	
	
	return true;
}

// Update: draw background
bool MainMenu::PostUpdate()
{
	
	if (Engine::GetInstance().settings.get()->settingsOpen) {
		
		for (const auto& bt : buttons)
			bt.second->state = GuiControlState::DISABLED;
	}

	if (creditsOpen) {
		Engine::GetInstance().render.get()->DrawTexture(credits, 0, 0, NULL);

		buttons["backBt"]->active = true;
		OnGuiMouseClickEvent(buttons["backBt"]);
		buttons["backBt"]->Update(_dt);

		
	}
	else {
		buttons["backBt"]->active = false;
		buttons["creditsBt"]->active = true;		
	}

	return true;
}

bool MainMenu::CleanUp() {

	for (const auto& bt : buttons) {
		bt.second->active = false;
	}

	return true;
}

bool MainMenu::OnGuiMouseClickEvent(GuiControl* control) {
	
	saved = rootNode.child("scene").child("savedData").attribute("saved").as_bool();
	switch (control->id) {
	case GuiControlId::NEW_GAME:
		if(control->state == GuiControlState::PRESSED){
			Engine::GetInstance().fade.get()->Fade((Module*)this, (Module*)Engine::GetInstance().scene.get(), 30);
			Engine::GetInstance().scene.get()->SetLoadState(false);
			Engine::GetInstance().scene.get()->SetLevel(LVL1);
			
		}

		break;
	case GuiControlId::CONTINUE:
		if (control->state == GuiControlState::PRESSED) {
			Engine::GetInstance().fade.get()->Fade((Module*)this, (Module*)Engine::GetInstance().scene.get(), 30);
			Engine::GetInstance().scene.get()->SetLoadState(true);
			
		}
		
		break;
	case GuiControlId::OPTIONS:
		if (control->state == GuiControlState::PRESSED) {
			if (!Engine::GetInstance().settings.get()->settingsOpen) 
				Engine::GetInstance().settings.get()->settingsOpen = true;
		}
		break;
	case GuiControlId::CREDITS:
		if (control->state == GuiControlState::PRESSED && !creditsOpen) {
			creditsOpen = true;
			buttons["creditsBt"]->active = false;
		}
		break;
	case GuiControlId::BACK:
		if (control->state == GuiControlState::PRESSED && creditsOpen) {
			creditsOpen = false;
			buttons["creditsBt"]->active = true;
			
		}
		break;
	case GuiControlId::QUIT:
		if (control->state == GuiControlState::PRESSED) {
			quit = true;
		}
		break;

	case GuiControlId::TEST_LEVEL:
		if (control->state == GuiControlState::PRESSED) {
			Engine::GetInstance().fade.get()->Fade((Module*)this, (Module*)Engine::GetInstance().scene.get(), 30);
			Engine::GetInstance().scene.get()->SetLoadState(false);
			Engine::GetInstance().scene.get()->SetLevel(LVL2);
		}

		break;
	}
	if (control->id == GuiControlId::FIRST_CAPE) {
		if (control->state == GuiControlState::PRESSED) {
			Engine::GetInstance().fade.get()->Fade((Module*)this, (Module*)Engine::GetInstance().scene.get(), 30);
			Engine::GetInstance().scene.get()->SetLoadState(false);
			Engine::GetInstance().scene.get()->SetLevel(LVL3);
		}
	}

	
	return true;
}

void MainMenu::SetGuiParameters(GuiControl* bt, std::string btName, pugi::xml_node parameters) {
	
	bt->id = (GuiControlId)parameters.child(btName.c_str()).attribute("id").as_int();
	if (bt->type == GuiControlType::SLIDER) {
		bt->bounds.x = parameters.child(btName.c_str()).attribute("circleX").as_int();
		bt->bounds.y = parameters.child(btName.c_str()).attribute("circleY").as_int();
	}

	bt->bounds.x = parameters.child(btName.c_str()).attribute("x").as_int();
	bt->bounds.y = parameters.child(btName.c_str()).attribute("y").as_int();
	bt->bounds.w = parameters.child(btName.c_str()).attribute("w").as_int();
	bt->bounds.h = parameters.child(btName.c_str()).attribute("h").as_int();

	bt->texture = Engine::GetInstance().textures.get()->Load(parameters.child(btName.c_str()).attribute("texture").as_string());
}
