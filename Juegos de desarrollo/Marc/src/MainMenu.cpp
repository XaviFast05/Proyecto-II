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
#include "Input.h"




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
		bt->SetGuiParameters(buttonName, buttonNode);
		buttons.push_back(bt);
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
		buttons[1]->state = GuiControlState::DISABLED();

	creditsOpen = false;
	buttons[5]->active = false;


	return ret;
}

bool MainMenu::Update(float dt)
{
	_dt = dt;

	if (Engine::GetInstance().settings.get()->settingsOpen) return true;

	Engine::GetInstance().render.get()->DrawTextureBuffer(bgTex, 0, 0, false, MENUS);
	Vector2D currentMousePos = Engine::GetInstance().input.get()->GetMousePosition();
	mouseMoved = (currentMousePos.getX() != prevMousePos.getX() || currentMousePos.getY() != prevMousePos.getY());

	if (creditsOpen)
	{
		buttons[5]->active = true;
		buttons[5]->Update(dt);
		OnGuiMouseClickEvent(buttons[5]);

		if (!mouseMoved)
		{
			buttons[5]->state = GuiControlState::FOCUSED;
			if (Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)
			{
				buttons[5]->state = GuiControlState::PRESSED;
				OnGuiMouseClickEvent(buttons[5]);
			}
		}
		else
		{
			selectedIndex = 4;
		}

		prevMousePos = currentMousePos;
		return true;
	}

	//Mouse 
	if (mouseMoved)
	{
		for (auto& bt : buttons)
		{
			if (!bt->active) continue;
			bt->Update(dt);
			OnGuiMouseClickEvent(bt);
		}
	}
	//Gamepad
	else
	{
		if (Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_DPAD_DOWN) == KEY_DOWN)
		{
			do {
				selectedIndex = (selectedIndex + 1 >= buttons.size()) ? 0 : selectedIndex + 1;
			} while (!buttons[selectedIndex]->active || buttons[selectedIndex]->state == GuiControlState::DISABLED);
		}
		if (Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_DPAD_UP) == KEY_DOWN)
		{
			do {
				selectedIndex = (selectedIndex == 0) ? buttons.size() - 1 : selectedIndex - 1;
			} while (!buttons[selectedIndex]->active || buttons[selectedIndex]->state == GuiControlState::DISABLED);
		}

		for (int i = 0; i < buttons.size(); ++i)
		{
			if (!buttons[i]->active) continue;
			buttons[i]->state = (i == selectedIndex) ? GuiControlState::FOCUSED : GuiControlState::NORMAL;
			buttons[i]->Update(dt);
			OnGuiMouseClickEvent(buttons[i]);
		}

		if (Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)
		{
			if (buttons[selectedIndex]->active && buttons[selectedIndex]->state != GuiControlState::DISABLED)
			{
				buttons[selectedIndex]->state = GuiControlState::PRESSED;
				OnGuiMouseClickEvent(buttons[selectedIndex]);
			}
		}
	}

	prevMousePos = currentMousePos;

	if (quit) return false;
	
	
	return true;
}

// Update: draw background
bool MainMenu::PostUpdate()
{
	
	if (Engine::GetInstance().settings.get()->settingsOpen) {
		
		for (const auto& bt : buttons)
			bt->state = GuiControlState::DISABLED;
	}

	if (creditsOpen) {
		Engine::GetInstance().render.get()->DrawTextureBuffer(credits, 0, 0, false, MENUS);
		buttons[5]->Update(_dt);
		
	}
	else {
		buttons[5]->active = false;
		buttons[4]->active = true;		
	}

	return true;
}

bool MainMenu::CleanUp() {

	for (const auto& bt : buttons) {
		bt->active = false;
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
			buttons[4]->active = false;
		}
		break;
	case GuiControlId::BACK:
		if (control->state == GuiControlState::PRESSED && creditsOpen) {
			creditsOpen = false;
			buttons[4]->active = true;
			
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
