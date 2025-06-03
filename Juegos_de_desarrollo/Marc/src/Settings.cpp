#include "GuiControl.h"
#include "GuiManager.h"
#include "GuiControlButton.h"
#include "GuiControlSlider.h"
#include "Settings.h"
#include "Engine.h"
#include "Textures.h"
#include "FadeToBlack.h"
#include "MainMenu.h"
#include "Scene.h"
#include "Window.h"
#include "TextManager.h"


Settings::Settings(bool startEnabled) : Module(startEnabled)
{
	name = "settings";
}

// Destructor
Settings::~Settings()
{

}

// Called before render is available
bool Settings::Awake()
{
	LOG("Loading Settings Screen");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Settings::Start()
{

	

	pugi::xml_parse_result result = configFile.load_file("config.xml");
	rootNode = configFile.child("config");

	testSound = Engine::GetInstance().audio.get()->LoadFx(rootNode.child("audio").child("fx").child("testSound").attribute("path").as_string());

	SDL_Texture* circleTex = Engine::GetInstance().textures.get()->Load(configParameters.child("sliders").attribute("texture").as_string());
	SDL_Texture* barTex = Engine::GetInstance().textures.get()->Load(configParameters.child("sliders").attribute("barTexture").as_string());


	musicSlider = (GuiControlSlider*)Engine::Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::SLIDER, "musicSlider", "", { 0,0,0,0 }, this, { 0,0,0,0 }, circleTex, barTex);

	musicSlider->SetGuiParameters("musicSlider", configParameters.child("sliders"));
	settingsGUI.push_back(musicSlider);

	sfxSlider = (GuiControlSlider*)Engine::Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::SLIDER, "sfxSlider", "", { 0,0,0,0 }, this, { 0,0,0,0 }, circleTex, barTex);
	sfxSlider->SetGuiParameters("sfxSlider", configParameters.child("sliders"));

	settingsGUI.push_back(sfxSlider);

	backBt = (GuiControlButton*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::BUTTON, "backBt", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	backBt->SetGuiParameters("backBt", configParameters);
	
	settingsGUI.push_back(backBt);

	// Language buttons
	espBt = (GuiControlButton*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::BUTTON, "espBt", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	espBt->SetGuiParameters("espBt", configParameters);

	controlsBt = (GuiControlButton*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::BUTTON, "controlsBt", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	controlsBt->SetGuiParameters("controlsBt", configParameters);

	settingsGUI.push_back(controlsBt);
	
	fullScreenBox = (GuiControlCheckBox*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::CHECKBOX, "fullScreenBox", "", { 0,0,0,0 }, this, { 0,0,0,0 });
	fullScreenBox->SetGuiParameters("fullScreenBox", configParameters);
	settingsGUI.push_back(fullScreenBox);

	optPanel = Engine::GetInstance().textures.get()->Load(configParameters.child("optPanel").attribute("path").as_string());
	optPanelX = configParameters.child("optPanel").attribute("x").as_int();
	optPanelY = configParameters.child("optPanel").attribute("y").as_int();
	optPanelW = configParameters.child("optPanel").attribute("w").as_int();
	optPanelH = configParameters.child("optPanel").attribute("h").as_int();
	titleFont = TTF_OpenFont(configParameters.child("optPanel").attribute("font").as_string(), configParameters.child("optPanel").attribute("fontSize").as_int());
	titleText = configParameters.child("optPanel").attribute("text").as_string();
	titleVerticalDisplacement = configParameters.child("optPanel").attribute("textVerticalDisplacement").as_int();

	controlsKeyboardPanel = Engine::GetInstance().textures.get()->Load(configParameters.child("controlsKeyboardPanel").attribute("path").as_string());
	controlsKeyboardPanelX = configParameters.child("controlsKeyboardPanel").attribute("x").as_int();
	controlsKeyboardPanelY = configParameters.child("controlsKeyboardPanel").attribute("y").as_int();
	controlsKeyboardPanelW = configParameters.child("controlsKeyboardPanel").attribute("w").as_int();
	controlsKeyboardPanelH = configParameters.child("controlsKeyboardPanel").attribute("h").as_int();
	controlsFont = TTF_OpenFont(configParameters.child("controlsKeyboardPanel").attribute("font").as_string(), configParameters.child("controlsKeyboardPanel").attribute("fontSize").as_int());
	controlsText = configParameters.child("controlsKeyboardPanel").attribute("text").as_string();
	controlsVerticalDisplacement = configParameters.child("controlsKeyboardPanel").attribute("textVerticalDisplacement").as_int();

	musicSlider->sliderPosX = musicSlider->sliderBounds.x + musicSlider->sliderBounds.w/2 - musicSlider->bounds.w/2;
	sfxSlider->sliderPosX = sfxSlider->sliderBounds.x + sfxSlider->sliderBounds.w/2 - sfxSlider->bounds.w/2;

	LoadPrefs();

	settingsOpen = false;
	return true;
}

// Called each loop iteration
bool Settings::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Settings::Update(float dt)
{
	SDL_Rect camera = Engine::GetInstance().render.get()->camera;
	int windowScale = Engine::GetInstance().window.get()->GetScale();
	if (settingsOpen){
		int screenWidth = rootNode.child("window").child("resolution").attribute("width").as_int();
		int screenHeight = rootNode.child("window").child("resolution").attribute("height").as_int();
		
		
		
		for (GuiControl* gui : settingsGUI) {
			if (gui->active == false) {
				gui->active = true;
			}
		}
		
		if (controlsOpen == false)
		{
			Engine::GetInstance().render.get()->DrawRectangle({ 0 , 0, screenWidth, screenHeight }, 0, 0, 0, 200, true, false);
			Engine::GetInstance().render.get()->DrawTextureBuffer(optPanel, -camera.x / windowScale + optPanelX, -camera.y / windowScale + optPanelY, false, MENUS);

			fullScreenBox->Update(dt);

			musicSlider->Update(dt);
			OnGuiMouseClickEvent(musicSlider);

			sfxSlider->Update(dt);
			OnGuiMouseClickEvent(sfxSlider);

			espBt->Update(dt);
			OnGuiMouseClickEvent(espBt);

			controlsBt->Update(dt);
			OnGuiMouseClickEvent(controlsBt);
			if (titleText != "")
			{
				int textW = 0, textH = 0;
				TTF_SizeUTF8(titleFont, Engine::GetInstance().textManager.get()->GetText(titleText).c_str(), &textW, &textH);

				Engine::GetInstance().render.get()->DrawTextToBuffer(
					Engine::GetInstance().textManager.get()->GetText(titleText).c_str(),
					optPanelX + (optPanelW / 2) - textW / 2,
					optPanelY + (optPanelH / 2) - textH / 2 + titleVerticalDisplacement,
					textW,
					textH,
					titleFont,
					{ 255, 255, 255, 255 }, MENUS
				);
			}
		}
		else
		{
			Engine::GetInstance().render.get()->DrawRectangle({ 0 , 0, screenWidth, screenHeight }, 0, 0, 0, 200, true, false);
			Engine::GetInstance().render.get()->DrawTextureBuffer(controlsKeyboardPanel, -camera.x / windowScale + controlsKeyboardPanelX, -camera.y / windowScale + controlsKeyboardPanelY, false, MENUS);
			if (controlsText != "")
			{
				int textW = 0, textH = 0;
				TTF_SizeUTF8(controlsFont, Engine::GetInstance().textManager.get()->GetText(controlsText).c_str(), &textW, &textH);

				Engine::GetInstance().render.get()->DrawTextToBuffer(
					Engine::GetInstance().textManager.get()->GetText(controlsText).c_str(),
					controlsKeyboardPanelX + (controlsKeyboardPanelW / 2) - textW / 2,
					controlsKeyboardPanelY + (controlsKeyboardPanelH / 2) - textH / 2 + titleVerticalDisplacement,
					textW,
					textH,
					controlsFont,
					{ 255, 255, 255, 255 }, MENUS
				);
			}
		}
		
		backBt->Update(dt);
		OnGuiMouseClickEvent(backBt);
		
		
	}
	else {
		for (GuiControl* gui : settingsGUI) {
			gui->active = false;
		}
		
	}
	
	return true;
}

// Called each loop iteration
bool Settings::PostUpdate()
{
	
	return true;
}

// Called before quitting
bool Settings::CleanUp()
{
	LOG("Freeing UI");

	for (GuiControl* gui : settingsGUI) {
		gui->active = false;
	}
	return true;
}

bool Settings::OnGuiMouseClickEvent(GuiControl* control) {

	saved = rootNode.child("scene").child("savedData").attribute("saved").as_bool();

	switch (control->id) {
	case GuiControlId::MUSIC:
		musicVolume = SetVolume((GuiControlSlider*)control);
		Mix_VolumeMusic(musicVolume);
		break;
	case GuiControlId::SFX:
		
		sfxVolume = SetVolume((GuiControlSlider*)control);
		Engine::GetInstance().audio.get()->PlayFx(testSound);
		break;
	case GuiControlId::FULLSCREEN:
		if (fullScreenBox->isChecked) {
			SDL_SetWindowFullscreen(Engine::GetInstance().window.get()->window, SDL_WINDOW_FULLSCREEN);
		}
		else {
			int windowW, windowH;
			Engine::GetInstance().window.get()->GetWindowSize(windowW, windowH);
			SDL_SetWindowFullscreen(Engine::GetInstance().window.get()->window, 0);
			SDL_SetWindowSize(Engine::GetInstance().window.get()->window, windowW, windowH);
		}
		break;
	case GuiControlId::BACK:
		if (control->state == GuiControlState::PRESSED && settingsOpen) {
			if (controlsOpen == true) {
				controlsOpen = false;
			}
			else {
				settingsOpen = false;
				for (const auto& bt : Engine::GetInstance().mainMenu.get()->buttons) {
					bt.second->state = GuiControlState::NORMAL;
					if (!saved) {
						Engine::GetInstance().mainMenu.get()->buttons["continueBt"]->state = GuiControlState::DISABLED;
					}
				}
				SavePrefs();
			}
		}
		break;
	case GuiControlId::CONTROLS:
		if (control->state == GuiControlState::PRESSED && settingsOpen) {
			controlsOpen = true;
		}
		
		break;

	case GuiControlId::ESP:
		if (control->state == GuiControlState::PRESSED) {
			Engine::GetInstance().textManager.get()->ChangeIdiom(currentLanguage++);
			if (currentLanguage > 2) { // Assuming 0 = ESP, 1 = CAT, 2 = ENG
				currentLanguage = 0;
			}
		}
		break;

	case GuiControlId::CAT:
		if (control->state == GuiControlState::PRESSED) {
			Engine::GetInstance().textManager.get()->ChangeIdiom(1);
		}
		break;

		case GuiControlId::ENG:
		if (control->state == GuiControlState::PRESSED) {
			Engine::GetInstance().textManager.get()->ChangeIdiom(2);
		}
		break;
	}


	return true;
}

int Settings::SetVolume(GuiControlSlider* slider) {

	return (slider->volumeValue);
}

void Settings::SavePrefs()
{
	pugi::xml_document saveFile;
	pugi::xml_parse_result result = saveFile.load_file("savedData.xml");

	if (result == NULL) {
		LOG("Error loading saveData.xml");
		return;
	}

	pugi::xml_node playerPrefsNode = saveFile.child("savedData").child("playerPrefs");

	playerPrefsNode.child("playerPrefs").child("fullscreen").attribute("toggle").set_value((int)fullScreenBox->isChecked);
	playerPrefsNode.child("playerPrefs").child("musicVolume").attribute("value").set_value(musicVolume);
	playerPrefsNode.child("playerPrefs").child("sfxVolume").attribute("value").set_value(sfxVolume);

	//Saves the modifications to the XML 
	saveFile.save_file("savedData.xml");
}

void Settings::LoadPrefs()
{
	pugi::xml_document saveFile;
	pugi::xml_parse_result result = saveFile.load_file("savedData.xml");

	pugi::xml_node playerPrefsNode = saveFile.child("savedData").child("playerPrefs");

	fullScreen = playerPrefsNode.child("fullscreen").attribute("toggle").as_bool();
	musicVolume = playerPrefsNode.child("musicVolume").attribute("value").as_int();
	sfxVolume = playerPrefsNode.child("sfxVolume").attribute("value").as_int();

	fullScreenBox->SetChecked(fullScreen);
	if (fullScreenBox->isChecked) {
		SDL_SetWindowFullscreen(Engine::GetInstance().window.get()->window, SDL_WINDOW_FULLSCREEN);
	}
	musicSlider->SetVolumeValue(musicVolume);
	sfxSlider->SetVolumeValue(sfxVolume);
}