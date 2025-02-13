#include "UI.h"
#include "LOG.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Window.h"
#include "Physics.h"

UI::UI() : Module(){

}

UI::~UI(){

}

bool UI::Awake(){
	LOG("Loading UI");
	bool ret = true;

	return ret;
}

bool UI::Start(){
	help = false;
	bool ret = true;
	helpMenu = Engine::GetInstance().textures.get()->Load("Assets/UI/Help_menu/Help_menu.png");


	if (helpMenu == nullptr){
		LOG("Failed to load texture: Assets/UI/Help_menu/Help_menu.png");
		ret = false;
	}

	return true;
}


bool UI::Update(float dt){
	
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN){
		help = !help;
	}

	if (help){
		Render* render = Engine::GetInstance().render.get();
		Window* window = Engine::GetInstance().window.get();
		render->DrawTexture(helpMenu, -render->camera.x / window->scale + OFFSET_X, -render->camera.y / window->scale + OFFSET_Y);
	}


	return true;
}

bool UI::CleanUp(){
	LOG("Freeing UI");
	return true;
}
