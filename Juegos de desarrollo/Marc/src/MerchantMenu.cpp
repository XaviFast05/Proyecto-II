#include "GuiControl.h"
#include "GuiManager.h"
#include "GuiControlButton.h"
#include "GuiControlSlider.h"
#include "MerchantMenu.h"
#include "CurrencyManager.h"
#include "Engine.h"
#include "Textures.h"
#include "FadeToBlack.h"
#include "MainMenu.h"
#include "Scene.h"
#include "Window.h"


MerchantMenu::MerchantMenu(bool startEnabled) : Module(startEnabled)
{
	name = "merchantMenu";
}

// Destructor
MerchantMenu::~MerchantMenu()
{

}

// Called before render is available
bool MerchantMenu::Awake()
{
	LOG("Loading Settings Screen");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool MerchantMenu::Start()
{

	pugi::xml_parse_result result = configFile.load_file("config.xml");
	rootNode = configFile.child("config");

	testSound = Engine::GetInstance().audio.get()->LoadFx(rootNode.child("audio").child("fx").child("testSound").attribute("path").as_string());

	/*SDL_Texture* circleTex = Engine::GetInstance().textures.get()->Load(configParameters.child("sliders").attribute("texture").as_string());*/
	//SDL_Texture* barTex = Engine::GetInstance().textures.get()->Load(configParameters.child("sliders").attribute("barTexture").as_string());


	//musicSlider = (GuiControlSlider*)Engine::Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::SLIDER, "musicSlider", "", { 0,0,0,0 }, this, { 0,0,0,0 }, circleTex, barTex);

	//SetGuiParameters(musicSlider, "musicSlider", configParameters.child("sliders"));
	//settingsGUI.push_back(musicSlider);

	//sfxSlider = (GuiControlSlider*)Engine::Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::SLIDER, "sfxSlider", "", { 0,0,0,0 }, this, { 0,0,0,0 }, circleTex, barTex);
	//SetGuiParameters(sfxSlider, "sfxSlider", configParameters.child("sliders"));

	//settingsGUI.push_back(sfxSlider);


	//IMPORTANTE: Que siga este orden porque sino no coincide con el config

	firstUpgradeBt = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "firstUpgradeBt", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(firstUpgradeBt, "firstUpgradeBt", configParameters);

	merchantGUI.push_back(firstUpgradeBt);

	secondUpgradeBt = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "secondUpgradeBt", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(secondUpgradeBt, "secondUpgradeBt", configParameters);

	merchantGUI.push_back(secondUpgradeBt);

	thirdUpgradeBt = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "thirdUpgradeBt", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(thirdUpgradeBt, "thirdUpgradeBt", configParameters);

	merchantGUI.push_back(thirdUpgradeBt);

	fourthUpgradeBt = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "fourthUpgradeBt", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(fourthUpgradeBt, "fourthUpgradeBt", configParameters);

	merchantGUI.push_back(fourthUpgradeBt);

	fifthUpgradeBt = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "fifthUpgradeBt", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(fifthUpgradeBt, "fifthUpgradeBt", configParameters);

	merchantGUI.push_back(fifthUpgradeBt);

	sixthUpgradeBt = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "sixthUpgradeBt", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(sixthUpgradeBt, "sixthUpgradeBt", configParameters);

	merchantGUI.push_back(sixthUpgradeBt);

	seventhUpgradeBt = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "seventhUpgradeBt", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(seventhUpgradeBt, "seventhUpgradeBt", configParameters);

	merchantGUI.push_back(seventhUpgradeBt);

	eighthUpgradeBt = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "eighthUpgradeBt", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(eighthUpgradeBt, "eighthUpgradeBt", configParameters);

	merchantGUI.push_back(eighthUpgradeBt);

	backBt = (GuiControlButton*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::BUTTON, "backBt", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	SetGuiParameters(backBt, "backBt", configParameters);

	merchantGUI.push_back(backBt);




	//fullScreenBox = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "fullScreenBox", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	//SetGuiParameters(fullScreenBox, "fullScreenBox", configParameters);
	//settingsGUI.push_back(fullScreenBox);

	merchantPanel = Engine::GetInstance().textures.get()->Load(configParameters.child("merchantPanel").attribute("path").as_string());
	merchantPanelX = configParameters.child("merchantPanel").attribute("x").as_int();
	merchantPanelY = configParameters.child("merchantPanel").attribute("y").as_int();



	//musicSlider->sliderPosX = musicSlider->sliderBounds.x + musicSlider->sliderBounds.w / 2 - musicSlider->bounds.w / 2;
	//sfxSlider->sliderPosX = sfxSlider->sliderBounds.x + sfxSlider->sliderBounds.w / 2 - sfxSlider->bounds.w / 2;


	confirmClick = false;
	beforeId = 0;

	LoadPrefs();

	merchantPanelOpen = false;
	return true;
}

// Called each loop iteration
bool MerchantMenu::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool MerchantMenu::Update(float dt)
{
	SDL_Rect camera = Engine::GetInstance().render.get()->camera;
	int windowScale = Engine::GetInstance().window.get()->GetScale();
	
	if (merchantPanelOpen) {
		int screenWidth = rootNode.child("window").child("resolution").attribute("width").as_int();
		int screenHeight = rootNode.child("window").child("resolution").attribute("height").as_int();


		Engine::GetInstance().render.get()->DrawRectangle({ 0 , 0, screenWidth, screenHeight }, 0, 0, 0, 200, true, false);
		Engine::GetInstance().render.get()->DrawTextureBuffer(merchantPanel, -camera.x / windowScale + merchantPanelX, -camera.y / windowScale + merchantPanelY);


		for (GuiControl* gui : merchantGUI) {
			if (gui->active == false) {
				gui->active = true;
			}
		}

		firstUpgradeBt->Update(dt);
		/*OnGuiMouseClickEvent(firstUpgradeBt);*/

		secondUpgradeBt->Update(dt);
		/*OnGuiMouseClickEvent(secondUpgradeBt);*/

		thirdUpgradeBt->Update(dt);
		/*OnGuiMouseClickEvent(thirdUpgradeBt);*/

		fourthUpgradeBt->Update(dt);
		/*OnGuiMouseClickEvent(fourthUpgradeBt);*/

		fifthUpgradeBt->Update(dt);
		/*OnGuiMouseClickEvent(fifthUpgradeBt);*/

		sixthUpgradeBt->Update(dt);
		/*OnGuiMouseClickEvent(sixthUpgradeBt);*/

		seventhUpgradeBt->Update(dt);
		/*OnGuiMouseClickEvent(seventhUpgradeBt);*/

		eighthUpgradeBt->Update(dt);
		/*OnGuiMouseClickEvent(eighthUpgradeBt);*/

		backBt->Update(dt);
		/*OnGuiMouseClickEvent(backBt);*/

		//fullScreenBox->Update(dt);

		ShowInfo(beforeId);

		//musicSlider->Update(dt);
		//OnGuiMouseClickEvent(musicSlider);

		//sfxSlider->Update(dt);




		//if (fullScreenBox->isChecked) {
		//	SDL_SetWindowFullscreen(Engine::GetInstance().window.get()->window, SDL_WINDOW_FULLSCREEN);
		//}
		//else {
		//	int windowW, windowH;
		//	Engine::GetInstance().window.get()->GetWindowSize(windowW, windowH);
		//	SDL_SetWindowFullscreen(Engine::GetInstance().window.get()->window, 0);
		//	SDL_SetWindowSize(Engine::GetInstance().window.get()->window, windowW, windowH);
		//}
	}
	else {
		for (GuiControl* gui : merchantGUI) {
			gui->active = false;
		}

	}

	return true;
}

// Called each loop iteration
bool MerchantMenu::PostUpdate()
{

	return true;
}

// Called before quitting
bool MerchantMenu::CleanUp()
{
	LOG("Freeing MerchantMenu");

	for (GuiControl* gui : merchantGUI) {
		gui->active = false;
	}
	return true;
}

bool MerchantMenu::OnGuiMouseClickEvent(GuiControl* control) {

	saved = rootNode.child("scene").child("savedData").attribute("saved").as_bool();

	if (!confirmClick && (int)control->id == beforeId) confirmClick = true;
	else if (!confirmClick && (int)control->id != beforeId) beforeId = (int)control->id;
	else if (confirmClick) {
	
		switch (control->id) {


		case GuiControlId::FIRST_UPGRADE:
			if (Engine::GetInstance().scene.get()->player->currencyManager->GetCurrency() > 100) {
				Engine::GetInstance().scene.get()->player->currencyManager->SumCurrency(-100);
				// desbloquear mejora
				control->state == GuiControlState::DISABLED;

			}
			break;
		case GuiControlId::SECOND_UPGRADE:
			if (Engine::GetInstance().scene.get()->player->currencyManager->GetCurrency() > 200) {
				Engine::GetInstance().scene.get()->player->currencyManager->SumCurrency(-200);
				// desbloquear mejora
				control->state == GuiControlState::DISABLED;
			}
			break;
		case GuiControlId::THIRD_UPGRADE:
			if (Engine::GetInstance().scene.get()->player->currencyManager->GetCurrency() > 300) {
				Engine::GetInstance().scene.get()->player->currencyManager->SumCurrency(-300);
				// desbloquear mejora
				control->state == GuiControlState::DISABLED;
			}
			break;
		case GuiControlId::FOURTH_UPGRADE:
			if (Engine::GetInstance().scene.get()->player->currencyManager->GetCurrency() > 400) {
				Engine::GetInstance().scene.get()->player->currencyManager->SumCurrency(-400);
				// desbloquear mejora
				control->state == GuiControlState::DISABLED;
			}
			break;
		case GuiControlId::FIFTH_UPGRADE:
			if (Engine::GetInstance().scene.get()->player->currencyManager->GetCurrency() > 500) {
				Engine::GetInstance().scene.get()->player->currencyManager->SumCurrency(-500);
				// desbloquear mejora
				control->state == GuiControlState::DISABLED;
			}
			break;
		case GuiControlId::SIXTH_UPGRADE:
			if (Engine::GetInstance().scene.get()->player->currencyManager->GetCurrency() > 600) {
				Engine::GetInstance().scene.get()->player->currencyManager->SumCurrency(-600);
				// desbloquear mejora
				control->state == GuiControlState::DISABLED;
			}
			break;
		case GuiControlId::SEVENTH_UPGRADE:
			if (Engine::GetInstance().scene.get()->player->currencyManager->GetCurrency() > 700) {
				Engine::GetInstance().scene.get()->player->currencyManager->SumCurrency(-700);
				// desbloquear mejora
				control->state == GuiControlState::DISABLED;
			}
			break;
		case GuiControlId::EIGHTH_UPGRADE:
			if (Engine::GetInstance().scene.get()->player->currencyManager->GetCurrency() > 800) {
				Engine::GetInstance().scene.get()->player->currencyManager->SumCurrency(-800);
				// desbloquear mejora
				control->state == GuiControlState::DISABLED;
			}
			break;
		}
	
	}
	

	// BACK BUTTON
	if (control->id == GuiControlId::BACK && control->state == GuiControlState::PRESSED && merchantPanelOpen) {
		merchantPanelOpen = false;
		Engine::GetInstance().scene.get()->player->pbody->body->SetEnabled(true);
		SavePrefs();
	}



	//sfxVolume = SetVolume((GuiControlSlider*)control);

	return true;
}


void MerchantMenu::SetGuiParameters(GuiControl* bt, std::string btName, pugi::xml_node parameters) {

	bt->id = (GuiControlId)parameters.child(btName.c_str()).attribute("id").as_int();
	bt->bounds.x = parameters.child(btName.c_str()).attribute("x").as_int();
	bt->bounds.y = parameters.child(btName.c_str()).attribute("y").as_int();
	bt->bounds.w = parameters.child(btName.c_str()).attribute("w").as_int();
	bt->bounds.h = parameters.child(btName.c_str()).attribute("h").as_int();
	bt->texture = Engine::GetInstance().textures.get()->Load(parameters.child(btName.c_str()).attribute("texture").as_string());
	//unlocked = Engine::GetInstance().textures.get()->Load(parameters.child(btName.c_str()).attribute("texture").as_string());

}

void MerchantMenu::SavePrefs()
{
	pugi::xml_document saveFile;
	pugi::xml_parse_result result = saveFile.load_file("savedData.xml");

	if (result == NULL) {
		LOG("Error loading saveData.xml");
		return;
	}

	//pugi::xml_node playerPrefsNode = saveFile.child("savedData").child("playerPrefs");

	//playerPrefsNode.child("playerPrefs").child("fullscreen").attribute("toggle").set_value((int)fullScreenBox->isChecked);
	//playerPrefsNode.child("playerPrefs").child("musicVolume").attribute("value").set_value(musicVolume);
	//playerPrefsNode.child("playerPrefs").child("sfxVolume").attribute("value").set_value(sfxVolume);

	//Saves the modifications to the XML 
	saveFile.save_file("savedData.xml");
}

void MerchantMenu::LoadPrefs()
{
	pugi::xml_document saveFile;
	pugi::xml_parse_result result = saveFile.load_file("savedData.xml");

	pugi::xml_node playerPrefsNode = saveFile.child("savedData").child("playerPrefs");

	//fullScreen = playerPrefsNode.child("fullscreen").attribute("toggle").as_bool();
	//musicVolume = playerPrefsNode.child("musicVolume").attribute("value").as_int();
	//sfxVolume = playerPrefsNode.child("sfxVolume").attribute("value").as_int();

	//fullScreenBox->SetChecked(fullScreen);
	//if (fullScreenBox->isChecked) {
	//	SDL_SetWindowFullscreen(Engine::GetInstance().window.get()->window, SDL_WINDOW_FULLSCREEN);
	//}
	//musicSlider->SetVolumeValue(musicVolume);
	//sfxSlider->SetVolumeValue(sfxVolume);
}

void MerchantMenu::ShowInfo(int id)
{
	std::string nameText, valueText, descriptionText;


	switch (id) {
	case 14:
		nameText = "Upgrade 1";
		valueText = "Valor: 100";
		descriptionText = "Saltas más alto";
		break;

	case (int)GuiControlId::SECOND_UPGRADE:
		nameText = "Upgrade 2";
		valueText = "Valor: 200";
		descriptionText = "Dash más rapido";
		break;

	case (int)GuiControlId::THIRD_UPGRADE:
		nameText = "Upgrade 3";
		valueText = "Valor: 300";
		descriptionText = "Ataque cargado mas rapido";
		break;

	case (int)GuiControlId::FOURTH_UPGRADE:
		nameText = "Upgrade 4";
		valueText = "Valor: 400";
		descriptionText = "Recuperacion de piquetas mas rapida";
		break;

	case (int)GuiControlId::FIFTH_UPGRADE:
		nameText = "Upgrade 5";
		valueText = "Valor: 500";
		descriptionText = "Más velocidad de movimiento";
		break;

	case (int)GuiControlId::SIXTH_UPGRADE:
		nameText = "Upgrade 6";
		valueText = "Valor: 600";
		descriptionText = "Piquetas maximas aumentadas";
		break;

	case (int)GuiControlId::SEVENTH_UPGRADE:
		nameText = "Upgrade 7";
		valueText = "Valor: 700";
		descriptionText = "Haces mas dano con 1 corazon";
		break;

	case (int)GuiControlId::EIGHTH_UPGRADE:
		nameText = "Upgrade 8";
		valueText = "Valor: 800";
		descriptionText = "Desbloquea habilidad especial";
		break;

	}

	// Render
	Engine::GetInstance().render.get()->DrawText(nameText.c_str(), 450, 100, 64*6, 64);

	Engine::GetInstance().render.get()->DrawText(valueText.c_str(), 450, 200, 32*6, 32);

	Engine::GetInstance().render.get()->DrawText(descriptionText.c_str(), 450, 300, 24*14, 24);
}
