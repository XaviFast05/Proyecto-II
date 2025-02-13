#include "GuiControl.h"
#include "GuiManager.h"
#include "GuiControlButton.h"
#include "GuiControlSlider.h"
#include "Intro.h"
#include "Engine.h"
#include "Textures.h"
#include "FadeToBlack.h"
#include "MainMenu.h"
#include "Scene.h"
#include "Window.h"


Intro::Intro(bool startEnabled) : Module(startEnabled)
{
	name = "intro";
}

// Destructor
Intro::~Intro()
{

}

// Called before render is available
bool Intro::Awake()
{
	LOG("Loading Intro Screen");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Intro::Start()
{

	intro = Engine::GetInstance().textures.get()->Load(configParameters.attribute("path").as_string());
	maxIntroTime = 5.0f;
	introTimer.Start();



	return true;
}

// Called each loop iteration
bool Intro::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Intro::Update(float dt)
{
	Engine::GetInstance().render.get()->DrawTexture(intro, 0, 0, NULL);
	if (introTimer.ReadSec() > maxIntroTime || (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)) {
		Engine::GetInstance().fade.get()->Fade(this, Engine::GetInstance().mainMenu.get(), 60);
	}

	return true;
}

// Called each loop iteration
bool Intro::PostUpdate()
{

	return true;
}

// Called before quitting
bool Intro::CleanUp()
{
	LOG("Freeing Intro");
	
	return true;
}

