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
#include "TextManager.h"


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

	upgradeFont = TTF_OpenFont(configParameters.child("merchantPanel").attribute("font").as_string(), configParameters.child("merchantPanel").attribute("upgradeTextSize").as_int());
	valueFont = TTF_OpenFont(configParameters.child("merchantPanel").attribute("font").as_string(), configParameters.child("merchantPanel").attribute("valueTextSize").as_int());
	textFont = TTF_OpenFont(configParameters.child("merchantPanel").attribute("font").as_string(), configParameters.child("merchantPanel").attribute("textSize").as_int());

	u1 = Engine::GetInstance().GetInstance().textures.get()->Load(configParameters.child("firstUpgradeBt").attribute("texture").as_string());
	u2 = Engine::GetInstance().GetInstance().textures.get()->Load(configParameters.child("secondUpgradeBt").attribute("texture").as_string());
	u3 = Engine::GetInstance().GetInstance().textures.get()->Load(configParameters.child("thirdUpgradeBt").attribute("texture").as_string());
	u4 = Engine::GetInstance().GetInstance().textures.get()->Load(configParameters.child("fourthUpgradeBt").attribute("texture").as_string());
	u5 = Engine::GetInstance().GetInstance().textures.get()->Load(configParameters.child("fifthUpgradeBt").attribute("texture").as_string());
	u6 = Engine::GetInstance().GetInstance().textures.get()->Load(configParameters.child("sixthUpgradeBt").attribute("texture").as_string());
	u7 = Engine::GetInstance().GetInstance().textures.get()->Load(configParameters.child("seventhUpgradeBt").attribute("texture").as_string());
	u8 = Engine::GetInstance().GetInstance().textures.get()->Load(configParameters.child("eighthUpgradeBt").attribute("texture").as_string());
	upgradeImageX = configParameters.child("merchantPanel").attribute("upgradeImageX").as_int();
	upgradeImageY = configParameters.child("merchantPanel").attribute("upgradeImageY").as_int();
	upgradeImageW = configParameters.child("merchantPanel").attribute("upgradeImageW").as_int();
	upgradeImageH = configParameters.child("merchantPanel").attribute("upgradeImageH").as_int();
	upgradeTextRect = { 0,0, upgradeImageW, upgradeImageH };

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
	leaveMenu = false;
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
	ZoneScoped;
	SDL_Rect camera = Engine::GetInstance().render.get()->camera;
	int windowScale = Engine::GetInstance().window.get()->GetScale();
	
	if (merchantPanelOpen) {

		if (hasOpened) {
			if (Engine::GetInstance().scene.get()->player->HaveUpgrade(0)) firstUpgradeBt->state = GuiControlState::DISABLED;
			else firstUpgradeBt->state = GuiControlState::NORMAL;

			if (Engine::GetInstance().scene.get()->player->HaveUpgrade(1)) secondUpgradeBt->state = GuiControlState::DISABLED;
			else secondUpgradeBt->state = GuiControlState::NORMAL;

			if (Engine::GetInstance().scene.get()->player->HaveUpgrade(2)) thirdUpgradeBt->state = GuiControlState::DISABLED;
			else thirdUpgradeBt->state = GuiControlState::NORMAL;

			if (Engine::GetInstance().scene.get()->player->HaveUpgrade(3)) fourthUpgradeBt->state = GuiControlState::DISABLED;
			else fourthUpgradeBt->state = GuiControlState::NORMAL;

			if (Engine::GetInstance().scene.get()->player->HaveUpgrade(4)) fifthUpgradeBt->state = GuiControlState::DISABLED;
			else fifthUpgradeBt->state = GuiControlState::NORMAL;

			if (Engine::GetInstance().scene.get()->player->HaveUpgrade(5)) sixthUpgradeBt->state = GuiControlState::DISABLED;
			else sixthUpgradeBt->state = GuiControlState::NORMAL;

			if (Engine::GetInstance().scene.get()->player->HaveUpgrade(6)) seventhUpgradeBt->state = GuiControlState::DISABLED;
			else seventhUpgradeBt->state = GuiControlState::NORMAL;

			if (Engine::GetInstance().scene.get()->player->HaveUpgrade(7)) eighthUpgradeBt->state = GuiControlState::DISABLED;
			else eighthUpgradeBt->state = GuiControlState::NORMAL;

			hasOpened = false;
		}

		int screenWidth = rootNode.child("window").child("resolution").attribute("width").as_int();
		int screenHeight = rootNode.child("window").child("resolution").attribute("height").as_int();


		Engine::GetInstance().render.get()->DrawRectangle({ 0 , 0, screenWidth, screenHeight }, 0, 0, 0, 200, true, false);
		Engine::GetInstance().render.get()->DrawTextureBuffer(merchantPanel, -camera.x / windowScale + merchantPanelX, -camera.y / windowScale + merchantPanelY, false, MENUS);


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
		OnGuiMouseClickEvent(backBt);

		//fullScreenBox->Update(dt);

		

		//musicSlider->Update(dt);
		//OnGuiMouseClickEvent(musicSlider);

		//sfxSlider->Update(dt);

		GuiControlCheckBox* upgrades[8] = {
	   firstUpgradeBt, secondUpgradeBt, thirdUpgradeBt, fourthUpgradeBt,
	   fifthUpgradeBt, sixthUpgradeBt, seventhUpgradeBt, eighthUpgradeBt
		};

		const char* upgradeNames[8] = {
			"First", "Second", "Third", "Fourth", "Fifth", "Sixth", "Seventh", "Eighth"
		};

		for (int i = 0; i < 8; ++i) {
			if (upgrades[i]->state == GuiControlState::FOCUSED) {
				ShowInfo((int)upgrades[i]->id);
			}
			else if (upgrades[i]->state == GuiControlState::NORMAL) {
			}
		}
		


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
		if (!gui) gui->active = false;
	}
	return true;
}

bool MerchantMenu::OnGuiMouseClickEvent(GuiControl* control) {

	saved = rootNode.child("scene").child("savedData").attribute("saved").as_bool();

	switch (control->id) {
	case GuiControlId::FIRST_UPGRADE:
		if (firstUpgradeBt->isChecked) {
			if (Engine::GetInstance().scene.get()->player->currencyManager->GetCurrency() < cost1)
			{
				leaveMenu = true;
				firstUpgradeBt->SetChecked(false);
			}
			else {
				Engine::GetInstance().scene.get()->player->currencyManager->SumCurrency(-cost1);
				Engine::GetInstance().scene.get()->player->UnlockUpgrade(0);
				firstUpgradeBt->state == GuiControlState::DISABLED;
				hasOpened = true;
			}

		}

	case GuiControlId::SECOND_UPGRADE:
		if (secondUpgradeBt->isChecked) {
			if (Engine::GetInstance().scene.get()->player->currencyManager->GetCurrency() < cost2)
			{
				leaveMenu = true;
				secondUpgradeBt->SetChecked(false);
			}
			else {
				Engine::GetInstance().scene.get()->player->currencyManager->SumCurrency(-cost2);
				Engine::GetInstance().scene.get()->player->UnlockUpgrade(1);
				secondUpgradeBt->state == GuiControlState::DISABLED;
				hasOpened = true;
			}

		}
		break;

	case GuiControlId::THIRD_UPGRADE:
		if (thirdUpgradeBt->isChecked) {
			if (Engine::GetInstance().scene.get()->player->currencyManager->GetCurrency() < cost3)
			{
				leaveMenu = true;
				thirdUpgradeBt->SetChecked(false);
			}
			else {
				Engine::GetInstance().scene.get()->player->currencyManager->SumCurrency(-cost3);
				Engine::GetInstance().scene.get()->player->UnlockUpgrade(2);
				thirdUpgradeBt->state == GuiControlState::DISABLED;
				hasOpened = true;
			}

		}
		break;

	case GuiControlId::FOURTH_UPGRADE:
		if (fourthUpgradeBt->isChecked) {
			if (Engine::GetInstance().scene.get()->player->currencyManager->GetCurrency() < cost4)
			{
				leaveMenu = true;
				fourthUpgradeBt->SetChecked(false);
			}
			else {
				Engine::GetInstance().scene.get()->player->currencyManager->SumCurrency(-cost4);
				Engine::GetInstance().scene.get()->player->UnlockUpgrade(3);
				fourthUpgradeBt->state == GuiControlState::DISABLED;
				hasOpened = true;
			}
		}
		break;

	case GuiControlId::FIFTH_UPGRADE:
		if (fifthUpgradeBt->isChecked) {
			if (Engine::GetInstance().scene.get()->player->currencyManager->GetCurrency() < cost5)
			{
				leaveMenu = true;
				fifthUpgradeBt->SetChecked(false);
			}
			else {
				Engine::GetInstance().scene.get()->player->currencyManager->SumCurrency(-cost5);
				Engine::GetInstance().scene.get()->player->UnlockUpgrade(4);
				fifthUpgradeBt->state == GuiControlState::DISABLED;
				hasOpened = true;
			}
		}
		break;

	case GuiControlId::SIXTH_UPGRADE:
		if (sixthUpgradeBt->isChecked) {
			if (Engine::GetInstance().scene.get()->player->currencyManager->GetCurrency() < cost6)
			{
				leaveMenu = true;
				sixthUpgradeBt->SetChecked(false);
			}
			else {
				Engine::GetInstance().scene.get()->player->currencyManager->SumCurrency(-cost6);
				Engine::GetInstance().scene.get()->player->UnlockUpgrade(5);
				sixthUpgradeBt->state == GuiControlState::DISABLED;
				hasOpened = true;
			}
		}
		break;

	case GuiControlId::SEVENTH_UPGRADE:
		if (seventhUpgradeBt->isChecked) {
			if (Engine::GetInstance().scene.get()->player->currencyManager->GetCurrency() < cost7)
			{
				leaveMenu = true;
				seventhUpgradeBt->SetChecked(false);
			}
			else {
				Engine::GetInstance().scene.get()->player->currencyManager->SumCurrency(-cost7);
				Engine::GetInstance().scene.get()->player->UnlockUpgrade(6);
				seventhUpgradeBt->state == GuiControlState::DISABLED;
				hasOpened = true;
			}
		}
		break;

	case GuiControlId::EIGHTH_UPGRADE:
		if (eighthUpgradeBt->isChecked) {
			if (Engine::GetInstance().scene.get()->player->currencyManager->GetCurrency() < cost8)
			{
				leaveMenu = true;
				eighthUpgradeBt->SetChecked(false);
			}
			else {
				Engine::GetInstance().scene.get()->player->currencyManager->SumCurrency(-cost8);
				Engine::GetInstance().scene.get()->player->UnlockUpgrade(7);
				eighthUpgradeBt->state == GuiControlState::DISABLED;
				hasOpened = true;
			}
		}
		break;
	
	case GuiControlId::BACK:
		if (backBt->state == GuiControlState::PRESSED) {
			merchantPanelOpen = false;
			Engine::GetInstance().scene.get()->player->pbody->body->SetEnabled(true);
			SavePrefs();
		}
		break;
	}

	if (leaveMenu)
	{
		Engine::GetInstance().scene.get()->player->StartDialog("DIALOG05");
		merchantPanelOpen = false;
		leaveMenu = false;
		Engine::GetInstance().scene.get()->player->pbody->body->SetEnabled(true);
		SavePrefs();
	}

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
	nameText = Engine::GetInstance().textManager.get()->GetText("UPGRADE");

	currentTexture = nullptr;

	switch (id) {
	case (int)GuiControlId::FIRST_UPGRADE:
		currentTexture = u1;
		nameText += " 1";
		valueText += "100";
		descriptionText = Engine::GetInstance().textManager.get()->GetText("UPGRADE1_DESCRIPCION");
		break;

	case (int)GuiControlId::SECOND_UPGRADE:
		currentTexture = u2;
		nameText += " 2";
		valueText += "200";
		descriptionText = Engine::GetInstance().textManager.get()->GetText("UPGRADE2_DESCRIPCION");
		break;

	case (int)GuiControlId::THIRD_UPGRADE:
		currentTexture = u3;
		nameText += " 3";
		valueText += "300";
		descriptionText = Engine::GetInstance().textManager.get()->GetText("UPGRADE3_DESCRIPCION");
		break;

	case (int)GuiControlId::FOURTH_UPGRADE:
		currentTexture = u4;
		nameText += " 4";
		valueText += "400";
		descriptionText = Engine::GetInstance().textManager.get()->GetText("UPGRADE4_DESCRIPCION");
		break;

	case (int)GuiControlId::FIFTH_UPGRADE:
		currentTexture = u5;
		nameText += " 5";
		valueText += "500";
		descriptionText = Engine::GetInstance().textManager.get()->GetText("UPGRADE5_DESCRIPCION");
		break;

	case (int)GuiControlId::SIXTH_UPGRADE:
		currentTexture = u6;
		nameText += " 6";
		valueText += "600";
		descriptionText = Engine::GetInstance().textManager.get()->GetText("UPGRADE6_DESCRIPCION");
		break;

	case (int)GuiControlId::SEVENTH_UPGRADE:
		currentTexture = u7;
		nameText += " 7";
		valueText += "700";
		descriptionText = Engine::GetInstance().textManager.get()->GetText("UPGRADE7_DESCRIPCION");
		break;

	case (int)GuiControlId::EIGHTH_UPGRADE:
		currentTexture = u8;
		nameText += " 8";
		valueText += "800";
		descriptionText = Engine::GetInstance().textManager.get()->GetText("UPGRADE8_DESCRIPCION");
		break;
	}

	int nameTextW = 0, nameTextH = 0;
	int valueTextW = 0, valueTextH = 0;
	int descriptionTextW = 0, descriptionTextH = 0;
	TTF_SizeUTF8(upgradeFont, nameText.c_str(), &nameTextW, &nameTextH);
	TTF_SizeUTF8(valueFont, valueText.c_str(), &valueTextW, &valueTextH);
	TTF_SizeUTF8(textFont, descriptionText.c_str(), &descriptionTextW, &descriptionTextH);

	// Render
	if (currentTexture)
	{
		Engine::GetInstance().render.get()->DrawTextureBuffer(currentTexture, 
		(-Engine::GetInstance().render.get()->camera.x / Engine::GetInstance().window.get()->GetScale()) + upgradeImageX,
		(-Engine::GetInstance().render.get()->camera.y / Engine::GetInstance().window.get()->GetScale()) + upgradeImageY,
		false,
		MENUS,
		&upgradeTextRect);
	}
	
	Engine::GetInstance().render.get()->DrawTextToBuffer(nameText.c_str(), 789, 210, nameTextW, nameTextH, upgradeFont, { 255,255,255,255 }, MENUS);

	Engine::GetInstance().render.get()->DrawTextToBuffer(valueText.c_str(), 918, 423, valueTextW, valueTextH, valueFont, { 255,255,255,255 }, MENUS);

	Engine::GetInstance().render.get()->DrawTextToBuffer(descriptionText.c_str(), 672, 304, descriptionTextW, descriptionTextH, textFont, { 255,255,255,255 }, MENUS);

}
