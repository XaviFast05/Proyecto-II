#include "VideoPlayer.h"
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
#include <string>
#include <format>
#include "CutscenePlayer.h"


VideoPlayer::VideoPlayer(bool startEnabled) : Module(startEnabled)
{
	name = "deathMenu";
}

VideoPlayer::~VideoPlayer()
{

}

// Load assets
bool VideoPlayer::Start()
{
	LOG("Loading background assets");

	Engine::GetInstance().entityManager.get()->Disable();

	bool ret = true;
	configFile.load_file("config.xml");

	pugi::xml_parse_result result = configFile.load_file("config.xml");
	rootNode = configFile.child("config");

	Engine::GetInstance().render.get()->camera.x = 0;
	Engine::GetInstance().render.get()->camera.y = 0;


	return ret;
}

bool VideoPlayer::Update(float dt)
{
	ZoneScoped;
	if (!videoPlayed)
	{
		Engine::GetInstance().cutScene->ConvertPixels(videoNum, 1);
		videoPlayed = true;
	}
	else
	{
		Engine::GetInstance().fade.get()->Fade((Module*)this, moduleToGo, 30);
	}


	return true;
}

// Update: draw background
bool VideoPlayer::PostUpdate()
{
	
	return true;
}

bool VideoPlayer::CleanUp() {

	return true;
}

void VideoPlayer::SetGuiParameters(GuiControl* bt, std::string btName, pugi::xml_node parameters) {

	bt->id = (GuiControlId)parameters.child(btName.c_str()).attribute("id").as_int();

	bt->bounds.x = parameters.child(btName.c_str()).attribute("x").as_int();
	bt->bounds.y = parameters.child(btName.c_str()).attribute("y").as_int();
	bt->bounds.w = parameters.child(btName.c_str()).attribute("w").as_int();
	bt->bounds.h = parameters.child(btName.c_str()).attribute("h").as_int();

	bt->texture = Engine::GetInstance().textures.get()->Load(parameters.child(btName.c_str()).attribute("texture").as_string());
}

void VideoPlayer::SetVideoNum(int num)
{
	videoNum = num;
}

void VideoPlayer::SetVideoPlayed(bool played)
{
	videoPlayed = played;
}

void VideoPlayer::SetModuleToGo(Module* module)
{
	moduleToGo = module;
}