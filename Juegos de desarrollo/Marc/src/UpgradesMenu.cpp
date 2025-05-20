#include "GuiControl.h"
#include "GuiManager.h"
#include "GuiControlButton.h"
#include "GuiControlSlider.h"
#include "UpgradesMenu.h"
#include "Engine.h"
#include "Textures.h"
#include "FadeToBlack.h"
#include "MainMenu.h"
#include "Scene.h"
#include "Window.h"


UpgradesMenu::UpgradesMenu(bool startEnabled) : Module(startEnabled)
{
	name = "upgradesMenu";
}

// Destructor
UpgradesMenu::~UpgradesMenu()
{

}

// Called before render is available
bool UpgradesMenu::Awake()
{
	LOG("Loading UpgradesMenu Screen");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool UpgradesMenu::Start()
{
	pugi::xml_parse_result result = configFile.load_file("config.xml");
	rootNode = configFile.child("config");

	//testSound = Engine::GetInstance().audio.get()->LoadFx(rootNode.child("audio").child("fx").child("testSound").attribute("path").as_string());

	SDL_Texture* circleTex = Engine::GetInstance().textures.get()->Load(configParameters.child("sliders").attribute("texture").as_string());
	SDL_Texture* barTex = Engine::GetInstance().textures.get()->Load(configParameters.child("sliders").attribute("barTexture").as_string());

	backBt = (GuiControlButton*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::BUTTON, "backBt", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(backBt, "backBt", configParameters);

	upgradesGUI.push_back(backBt);

	upgPanel = Engine::GetInstance().textures.get()->Load(configParameters.child("upgPanel").attribute("path").as_string());
	upgPanelX = configParameters.child("upgPanel").attribute("x").as_int();
	upgPanelY = configParameters.child("upgPanel").attribute("y").as_int();

	LoadPrefs();

	upgradesOpen = false;
	return true;
}

// Called each loop iteration
bool UpgradesMenu::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool UpgradesMenu::Update(float dt)
{
	SDL_Rect camera = Engine::GetInstance().render.get()->camera;
	int windowScale = Engine::GetInstance().window.get()->GetScale();
	if (upgradesOpen) {
		int screenWidth = rootNode.child("window").child("resolution").attribute("width").as_int();
		int screenHeight = rootNode.child("window").child("resolution").attribute("height").as_int();

		Engine::GetInstance().render.get()->DrawRectangle({ 0 , 0, screenWidth, screenHeight }, 0, 0, 0, 200, true, false);
		Engine::GetInstance().render.get()->DrawTexture(upgPanel, -camera.x / windowScale + upgPanelX, -camera.y / windowScale + upgPanelY);


		for (GuiControl* gui : upgradesGUI) {
			if (gui->active == false) {
				gui->active = true;
			}
		}

		backBt->Update(dt);
		OnGuiMouseClickEvent(backBt);
	}
	else {
		for (GuiControl* gui : upgradesGUI) {
			gui->active = false;
		}

	}

	return true;
}

// Called each loop iteration
bool UpgradesMenu::PostUpdate()
{

	return true;
}

// Called before quitting
bool UpgradesMenu::CleanUp()
{
	LOG("Freeing UI");

	for (GuiControl* gui : upgradesGUI) {
		gui->active = false;
	}
	return true;
}

bool UpgradesMenu::OnGuiMouseClickEvent(GuiControl* control) {

	saved = rootNode.child("scene").child("savedData").attribute("saved").as_bool();

	switch (control->id) {
		case GuiControlId::BACK:
		if (control->state == GuiControlState::PRESSED && upgradesOpen) {
			upgradesOpen = false;
			for (const auto& bt : Engine::GetInstance().mainMenu.get()->buttons) {
				bt.second->state = GuiControlState::NORMAL;
				if (!saved) {
					Engine::GetInstance().mainMenu.get()->buttons["continueBt"]->state = GuiControlState::DISABLED;
				}
			}
			SavePrefs();
		}

		break;
	}


	return true;
}


void UpgradesMenu::SetGuiParameters(GuiControl* bt, std::string btName, pugi::xml_node parameters) {

	bt->id = (GuiControlId)parameters.child(btName.c_str()).attribute("id").as_int();

	 if (bt->bullet_direction == GuiControlType::BUTTON || bt->bullet_direction == GuiControlType::CHECKBOX) {
		bt->bounds.x = parameters.child(btName.c_str()).attribute("x").as_int();
		bt->bounds.y = parameters.child(btName.c_str()).attribute("y").as_int();
		bt->bounds.w = parameters.child(btName.c_str()).attribute("w").as_int();
		bt->bounds.h = parameters.child(btName.c_str()).attribute("h").as_int();
		bt->texture = Engine::GetInstance().textures.get()->Load(parameters.child(btName.c_str()).attribute("texture").as_string());
	}



}


void UpgradesMenu::SavePrefs()
{
	pugi::xml_document saveFile;
	pugi::xml_parse_result result = saveFile.load_file("savedData.xml");

	if (result == NULL) {
		LOG("Error loading saveData.xml");
		return;
	}

	pugi::xml_node playerPrefsNode = saveFile.child("savedData").child("playerPrefs");

	//playerPrefsNode.child("playerPrefs").child("fullscreen").attribute("toggle").set_value((int)fullScreenBox->isChecked);
	//playerPrefsNode.child("playerPrefs").child("musicVolume").attribute("value").set_value(musicVolume);
	//playerPrefsNode.child("playerPrefs").child("sfxVolume").attribute("value").set_value(sfxVolume);

	//Saves the modifications to the XML 
	saveFile.save_file("savedData.xml");
}

void UpgradesMenu::LoadPrefs()
{
	pugi::xml_document saveFile;
	pugi::xml_parse_result result = saveFile.load_file("savedData.xml");

	pugi::xml_node playerPrefsNode = saveFile.child("savedData").child("playerPrefs");

	//fullScreen = playerPrefsNode.child("fullscreen").attribute("toggle").as_bool();
	//musicVolume = playerPrefsNode.child("musicVolume").attribute("value").as_int();
	//sfxVolume = playerPrefsNode.child("sfxVolume").attribute("value").as_int();

	//fullScreenBox->SetChecked(fullScreen);
	//if (fullScreenBox->isChecked) {
		//SDL_SetWindowFullscreen(Engine::GetInstance().window.get()->window, SDL_WINDOW_FULLSCREEN);
	//}
	//musicSlider->SetVolumeValue(musicVolume);
	//sfxSlider->SetVolumeValue(sfxVolume);
}