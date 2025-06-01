#include "UI.h"
#include "LOG.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Scene.h"
#include "MainMenu.h"
#include "FadeToBlack.h"

UI::UI(bool startEnabled) : Module(startEnabled)
{
	
}

// Destructor
UI::~UI()
{

}

// Called before render is available
bool UI::Awake()
{
	LOG("Loading UI");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool UI::Start()
{
	
	
	return true;
}

// Called each loop iteration
bool UI::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool UI::Update(float dt)
{
	

	return true;
}

// Called each loop iteration
bool UI::PostUpdate()
{
	return true;
}

// Called before quitting
bool UI::CleanUp()
{
	LOG("Freeing UI");
	


	return true;
}
