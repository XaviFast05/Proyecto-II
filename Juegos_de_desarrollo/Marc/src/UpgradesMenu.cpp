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
#include "Settings.h"


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

	u1 = Engine::GetInstance().textures.get()->Load(rootNode.child("upgradesMenu").child("upg1").attribute("texture").as_string());
	u2 = Engine::GetInstance().textures.get()->Load(rootNode.child("upgradesMenu").child("upg2").attribute("texture").as_string());
	u3 = Engine::GetInstance().textures.get()->Load(rootNode.child("upgradesMenu").child("upg3").attribute("texture").as_string());
	u4 = Engine::GetInstance().textures.get()->Load(rootNode.child("upgradesMenu").child("upg4").attribute("texture").as_string());
	u5 = Engine::GetInstance().textures.get()->Load(rootNode.child("upgradesMenu").child("upg5").attribute("texture").as_string());
	u6 = Engine::GetInstance().textures.get()->Load(rootNode.child("upgradesMenu").child("upg6").attribute("texture").as_string());
	u7 = Engine::GetInstance().textures.get()->Load(rootNode.child("upgradesMenu").child("upg7").attribute("texture").as_string());
	u8 = Engine::GetInstance().textures.get()->Load(rootNode.child("upgradesMenu").child("upg8").attribute("texture").as_string());

	circle1X = rootNode.child("upgradesMenu").child("circle1").attribute("x").as_int();
	circle1Y = rootNode.child("upgradesMenu").child("circle1").attribute("y").as_int();
	circle2X = rootNode.child("upgradesMenu").child("circle2").attribute("x").as_int();
	circle2Y = rootNode.child("upgradesMenu").child("circle2").attribute("y").as_int();

	circleRect.x = 0;
	circleRect.y = 0;
	circleRect.w = rootNode.child("upgradesMenu").child("circle2").attribute("w").as_int();
	circleRect.h = rootNode.child("upgradesMenu").child("circle2").attribute("h").as_int();

	backBt = (GuiControlButton*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::BUTTON, "backBt", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(backBt, "backBt", configParameters);
	changeMenuBt = (GuiControlButton*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::BUTTON, "changeMenuBt", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(changeMenuBt, "changeMenuBt", configParameters);
	upg1 = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "upg1", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(upg1, "upg1", configParameters);
	upg2 = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "upg2", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(upg2, "upg2", configParameters);
	upg3 = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "upg3", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(upg3, "upg3", configParameters);
	upg4 = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "upg4", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(upg4, "upg4", configParameters);
	upg5 = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "upg5", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(upg5, "upg5", configParameters);
	upg6 = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "upg6", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(upg6, "upg6", configParameters);
	upg7 = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "upg7", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(upg7, "upg7", configParameters);
	upg8 = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "upg8", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(upg8, "upg8", configParameters);

	player = Engine::GetInstance().scene.get()->player;

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
	ZoneScoped;
	SDL_Rect camera = Engine::GetInstance().render.get()->camera;
	int windowScale = Engine::GetInstance().window.get()->GetScale();
	if (upgradesOpen) {
		if (hasOpened) {
			if (!Engine::GetInstance().scene.get()->player->HaveUpgrade(0)) upg1->state = GuiControlState::DISABLED;
			if (!Engine::GetInstance().scene.get()->player->HaveUpgrade(1)) upg2->state = GuiControlState::DISABLED;
			if (!Engine::GetInstance().scene.get()->player->HaveUpgrade(2)) upg3->state = GuiControlState::DISABLED;
			if (!Engine::GetInstance().scene.get()->player->HaveUpgrade(3)) upg4->state = GuiControlState::DISABLED;
			if (!Engine::GetInstance().scene.get()->player->HaveUpgrade(4)) upg5->state = GuiControlState::DISABLED;
			if (!Engine::GetInstance().scene.get()->player->HaveUpgrade(5)) upg6->state = GuiControlState::DISABLED;
			if (!Engine::GetInstance().scene.get()->player->HaveUpgrade(6)) upg7->state = GuiControlState::DISABLED;
			if (!Engine::GetInstance().scene.get()->player->HaveUpgrade(7)) upg8->state = GuiControlState::DISABLED;

			if (Engine::GetInstance().scene.get()->player->upgrades.size() < Engine::GetInstance().scene.get()->player->maxUpgrades) {
				if (Engine::GetInstance().scene.get()->player->HaveUpgrade(0)) upg1->state = GuiControlState::NORMAL;
				if (Engine::GetInstance().scene.get()->player->HaveUpgrade(1)) upg2->state = GuiControlState::NORMAL;
				if (Engine::GetInstance().scene.get()->player->HaveUpgrade(2)) upg3->state = GuiControlState::NORMAL;
				if (Engine::GetInstance().scene.get()->player->HaveUpgrade(3)) upg4->state = GuiControlState::NORMAL;
				if (Engine::GetInstance().scene.get()->player->HaveUpgrade(4)) upg5->state = GuiControlState::NORMAL;
				if (Engine::GetInstance().scene.get()->player->HaveUpgrade(5)) upg6->state = GuiControlState::NORMAL;
				if (Engine::GetInstance().scene.get()->player->HaveUpgrade(6)) upg7->state = GuiControlState::NORMAL;
				if (Engine::GetInstance().scene.get()->player->HaveUpgrade(7)) upg8->state = GuiControlState::NORMAL;
			}
			else {
				if (Engine::GetInstance().scene.get()->player->HaveUpgrade(0) && !Engine::GetInstance().scene.get()->player->HaveActiveUpgrade(0)) upg1->state = GuiControlState::OVERLOADED;
				if (Engine::GetInstance().scene.get()->player->HaveUpgrade(1) && !Engine::GetInstance().scene.get()->player->HaveActiveUpgrade(1)) upg2->state = GuiControlState::OVERLOADED;
				if (Engine::GetInstance().scene.get()->player->HaveUpgrade(2) && !Engine::GetInstance().scene.get()->player->HaveActiveUpgrade(2)) upg3->state = GuiControlState::OVERLOADED;
				if (Engine::GetInstance().scene.get()->player->HaveUpgrade(3) && !Engine::GetInstance().scene.get()->player->HaveActiveUpgrade(3)) upg4->state = GuiControlState::OVERLOADED;
				if (Engine::GetInstance().scene.get()->player->HaveUpgrade(4) && !Engine::GetInstance().scene.get()->player->HaveActiveUpgrade(4)) upg5->state = GuiControlState::OVERLOADED;
				if (Engine::GetInstance().scene.get()->player->HaveUpgrade(5) && !Engine::GetInstance().scene.get()->player->HaveActiveUpgrade(5)) upg6->state = GuiControlState::OVERLOADED;
				if (Engine::GetInstance().scene.get()->player->HaveUpgrade(6) && !Engine::GetInstance().scene.get()->player->HaveActiveUpgrade(6)) upg7->state = GuiControlState::OVERLOADED;
				if (Engine::GetInstance().scene.get()->player->HaveUpgrade(7) && !Engine::GetInstance().scene.get()->player->HaveActiveUpgrade(7)) upg8->state = GuiControlState::OVERLOADED;
			}

			hasOpened = false;
		}
		int screenWidth = rootNode.child("window").child("resolution").attribute("width").as_int();
		int screenHeight = rootNode.child("window").child("resolution").attribute("height").as_int();

		Engine::GetInstance().render.get()->DrawRectangle({ 0 , 0, screenWidth, screenHeight }, 0, 0, 0, 200, true, false);
		Engine::GetInstance().render.get()->DrawTextureBuffer(upgPanel, -camera.x / windowScale + upgPanelX, -camera.y / windowScale + upgPanelY, false, MENUS);


		for (GuiControl* gui : upgradesGUI) {
			if (gui->active == false) {
				gui->active = true;
			}
		}

		upg1->Update(dt);
		upg2->Update(dt);
		upg3->Update(dt);
		upg4->Update(dt);
		upg5->Update(dt);
		upg6->Update(dt);
		upg7->Update(dt);
		upg8->Update(dt);

		int used = 0;

		if (upg1->isChecked) { Engine::GetInstance().render.get()->DrawTextureBuffer(u1, -camera.x / windowScale + (used == 0 ? circle1X : circle2X), -camera.y / windowScale + (used == 0 ? circle1Y : circle2Y), false, MENUS, &circleRect); used++; }
		if (upg2->isChecked) { Engine::GetInstance().render.get()->DrawTextureBuffer(u2, -camera.x / windowScale + (used == 0 ? circle1X : circle2X), -camera.y / windowScale + (used == 0 ? circle1Y : circle2Y), false, MENUS, &circleRect); used++; }
		if (upg3->isChecked) { Engine::GetInstance().render.get()->DrawTextureBuffer(u3, -camera.x / windowScale + (used == 0 ? circle1X : circle2X), -camera.y / windowScale + (used == 0 ? circle1Y : circle2Y), false, MENUS, &circleRect); used++; }
		if (upg4->isChecked) { Engine::GetInstance().render.get()->DrawTextureBuffer(u4, -camera.x / windowScale + (used == 0 ? circle1X : circle2X), -camera.y / windowScale + (used == 0 ? circle1Y : circle2Y), false, MENUS, &circleRect); used++; }
		if (upg5->isChecked) { Engine::GetInstance().render.get()->DrawTextureBuffer(u5, -camera.x / windowScale + (used == 0 ? circle1X : circle2X), -camera.y / windowScale + (used == 0 ? circle1Y : circle2Y), false, MENUS, &circleRect); used++; }
		if (upg6->isChecked) { Engine::GetInstance().render.get()->DrawTextureBuffer(u6, -camera.x / windowScale + (used == 0 ? circle1X : circle2X), -camera.y / windowScale + (used == 0 ? circle1Y : circle2Y), false, MENUS, &circleRect); used++; }
		if (upg7->isChecked) { Engine::GetInstance().render.get()->DrawTextureBuffer(u7, -camera.x / windowScale + (used == 0 ? circle1X : circle2X), -camera.y / windowScale + (used == 0 ? circle1Y : circle2Y), false, MENUS, &circleRect); used++; }
		if (upg8->isChecked) { Engine::GetInstance().render.get()->DrawTextureBuffer(u8, -camera.x / windowScale + (used == 0 ? circle1X : circle2X), -camera.y / windowScale + (used == 0 ? circle1Y : circle2Y), false, MENUS, &circleRect); used++; }

		backBt->Update(dt);
		OnGuiMouseClickEvent(backBt);
		changeMenuBt->Update(dt);
		OnGuiMouseClickEvent(changeMenuBt);

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
		case GuiControlId::CHANGE_MENU:
			if (control->state == GuiControlState::PRESSED && upgradesOpen) {
				if (Engine::GetInstance().upgradesMenu.get()->upgradesOpen) {
					Engine::GetInstance().upgradesMenu.get()->upgradesOpen = false;
				}
				if (Engine::GetInstance().settings.get()->settingsOpen) {
					Engine::GetInstance().settings.get()->settingsOpen = false;
				}
			}
			break;
		break;
		case GuiControlId::UPG1:
			if (upg1->isChecked) Engine::GetInstance().scene.get()->player->AddUpgrade(0);
			else Engine::GetInstance().scene.get()->player->RemoveUpgrade(0);
			hasOpened = true;
			break;

		case GuiControlId::UPG2:
			if (upg2->isChecked) Engine::GetInstance().scene.get()->player->AddUpgrade(1);
			else Engine::GetInstance().scene.get()->player->RemoveUpgrade(1);
			hasOpened = true;
			break;

		case GuiControlId::UPG3:
			if (upg3->isChecked) Engine::GetInstance().scene.get()->player->AddUpgrade(2);
			else Engine::GetInstance().scene.get()->player->RemoveUpgrade(2);
			hasOpened = true;
			break;

		case GuiControlId::UPG4:
			if (upg4->isChecked) Engine::GetInstance().scene.get()->player->AddUpgrade(3);
			else Engine::GetInstance().scene.get()->player->RemoveUpgrade(3);
			hasOpened = true;
			break;

		case GuiControlId::UPG5:
			if (upg5->isChecked) Engine::GetInstance().scene.get()->player->AddUpgrade(4);
			else Engine::GetInstance().scene.get()->player->RemoveUpgrade(4);
			hasOpened = true;
			break;

		case GuiControlId::UPG6:
			if (upg6->isChecked) Engine::GetInstance().scene.get()->player->AddUpgrade(5);
			else Engine::GetInstance().scene.get()->player->RemoveUpgrade(5);
			hasOpened = true;
			break;

		case GuiControlId::UPG7:
			if (upg7->isChecked) Engine::GetInstance().scene.get()->player->AddUpgrade(6);
			else Engine::GetInstance().scene.get()->player->RemoveUpgrade(6);
			hasOpened = true;
			break;

		case GuiControlId::UPG8:
			if (upg8->isChecked) Engine::GetInstance().scene.get()->player->AddUpgrade(7);
			else Engine::GetInstance().scene.get()->player->RemoveUpgrade(7);
			hasOpened = true;
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