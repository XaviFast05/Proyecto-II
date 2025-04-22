#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Pumpkin.h"
#include "Physics.h"
#include "BatEnemy.h"
#include "GroundEnemy.h"
#include <string>
#include "Particle.h"
#include "tracy/Tracy.hpp"
#include "GuiControl.h"
#include "GuiManager.h"
#include "GuiControlButton.h"
#include "Candy.h"
#include "MainMenu.h"
#include "FadeToBlack.h"
#include "Settings.h"
#include "Santa.h"
#include "DeathMenu.h"
#include "WinMenu.h"
#include "PickaxeManager.h"
#include "CurrencyManager.h"

#include "Intro.h"

Scene::Scene(bool startEnabled) : Module(startEnabled)
{
	
	name = "scene";
	player = nullptr;
	/*active = false;*/
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	bool ret = true;
	LOG("Loading Scene");

	level = LVL1;
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	paused = false;
	Engine::GetInstance().entityManager.get()->Enable();
	
	//Load HelpMenu
	help = false;
	helpPos.setX(configParameters.child("helpMenu").attribute("x").as_int());
	helpPos.setY(configParameters.child("helpMenu").attribute("y").as_int());
	helpMenu = Engine::GetInstance().textures.get()->Load(configParameters.child("helpMenu").attribute("path").as_string());

	if (loadScene) level = (Levels)configParameters.child("savedData").attribute("level").as_int();

	std::string path = configParameters.child("map").child("paths").child(GetCurrentLevelString().c_str()).attribute("path").as_string();
	std::string name = configParameters.child("map").child("paths").child(GetCurrentLevelString().c_str()).attribute("name").as_string();

	//Load Map
	Engine::GetInstance().map->Load(path, name);


	//Load Parallax
	Engine::GetInstance().map->LoadParalax(configParameters.child("map").child("parallax"));

	//Load Player
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	player->SetParameters(configParameters.child("entities").child("player"));

	//Load Enemies
	for (pugi::xml_node enemyNode : configParameters.child("entities").child("enemies").child("instances").child(GetCurrentLevelString().c_str()).children())
	{
		Enemy* enemy = (GroundEnemy*)Engine::GetInstance().entityManager->CreateEntity((EntityType)enemyNode.attribute("entityType").as_int());;
		LoadEnemy(enemy, enemyNode);
	}

	for (pugi::xml_node pumpkingNode : configParameters.child("entities").child("items").child("pumpkins").child("instances").child(GetCurrentLevelString().c_str()).children())
	{
		Pumpkin* pumpkin = (Pumpkin*)Engine::GetInstance().entityManager->CreateEntity((EntityType)pumpkingNode.attribute("entityType").as_int());;
		LoadItem(pumpkin, pumpkingNode);
	}

	for (pugi::xml_node candyNode : configParameters.child("entities").child("items").child("candies").child("instances").child(GetCurrentLevelString().c_str()).children())
	{
		Candy* candy = (Candy*)Engine::GetInstance().entityManager->CreateEntity((EntityType)candyNode.attribute("entityType").as_int());;
		LoadItem(candy, candyNode);
	}

	std::list<Entity*> entities = Engine::GetInstance().entityManager.get()->entities;
	for (const auto& entity : entities) {
		entity->Enable();
		entity->active = true;
		entity->renderable = true;
	}

	//Load PauseMenu
	pausePos.setX(configParameters.child("pauseMenu").child("pausePanel").attribute("x").as_int());
	pausePos.setY(configParameters.child("pauseMenu").child("pausePanel").attribute("y").as_int());
	pausePanel = Engine::GetInstance().textures.get()->Load(configParameters.child("pauseMenu").child("pausePanel").attribute("texture").as_string());
	
	pugi::xml_node pauseBtNode = configParameters.child("pauseMenu").child("buttons");
	
	pauseButtons.clear();
	for (pugi::xml_node child : pauseBtNode.children())
	{
		std::string buttonName = child.name();
		GuiControlButton* bt = (GuiControlButton*)Engine::GetInstance().guiManager.get()->CreateGuiControl(GuiControlType::BUTTON, buttonName.c_str(), "", { 0, 0, 0, 0 }, this, { 0,0,0,0 });
		this->SetGuiParameters(bt, buttonName, pauseBtNode);
		pauseButtons[buttonName] = bt;
		bt->active = false;
	}

	stoppedTimer = false;
	finalCandyNum = 0;
	currentTime = 0;

	transitionDisplace = 0;
	

	if (!loadScene)
	{
		if (level != LVL1)
		{
			LoadTimeLivesCandies();
		}
		else
		{
			currentTime = 0;
		}
		SaveState();
	}

	musicNode = Engine::GetInstance().GetConfig().child("audio").child("music");
	if (level == LVL1)Engine::GetInstance().audio.get()->PlayMusic(musicNode.child("lvl1Mus").attribute("path").as_string());
	else if (level == LVL2) {
		Engine::GetInstance().audio.get()->PlayMusic(musicNode.child("lvl2Mus").attribute("path").as_string());
		
	}
	
	startBossFight = false;
	bossMusPlaying = false;
	bossKilled = false;

	
	//UI
	heartsTexture = Engine::GetInstance().textures.get()->Load(configParameters.child("ui").child("heartContainers").attribute("path").as_string());
	caramelsTexture = Engine::GetInstance().textures.get()->Load(configParameters.child("ui").child("candyCounter").attribute("path").as_string());
	sandclockTexture = Engine::GetInstance().textures.get()->Load(configParameters.child("ui").child("sandClock").attribute("path").as_string());

	sand.LoadAnimations(configParameters.child("ui").child("sandClock").child("animations").child("sand"));


	return true;
}

void Scene::LoadEnemy(Enemy* enemy, pugi::xml_node instanceNode)
{
	enemy->SetPlayer(player);
	enemy->SetParameters(configParameters.child("entities").child("enemies").child(instanceNode.attribute("enemyType").as_string()));
	enemy->SetInstanceParameters(instanceNode);
	enemy->SetPath(instanceNode);
	enemies.push_back(enemy);
}

void Scene::LoadItem(Pumpkin* pumpkin, pugi::xml_node instanceNode) {

	pumpkin->SetPlayer(player);
	pumpkin->SetParameters(configParameters.child("entities").child("items").child("pumpkins"));
	pumpkin->SetInstanceParameters(instanceNode);
	pumpkins.push_back(pumpkin);
}

void Scene::LoadItem(Candy* candy, pugi::xml_node instanceNode) {

	candy->SetPlayer(player);
	candy->SetParameters(configParameters.child("entities").child("items").child("candies"));
	candy->SetInstanceParameters(instanceNode);
	candies.push_back(candy);
}


int Scene::GetLevel()
{
	return (int)level;
}


// Called each loop iteration
bool Scene::PreUpdate()
{
	
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{	
	ZoneScoped;

	_dt = dt;

	//if (player->lives <= 0 && player->respawnTimer.ReadSec() > player->respawnTime-1) {
	//	/*Engine::GetInstance().death.get()->Enable();*/

	//	Engine::GetInstance().death.get()->finalCandyNum = player->pickedCandies;
	//	Engine::GetInstance().fade.get()->Fade((Module*)this, Engine::GetInstance().death.get());
	//	

	//	
	//	return true;
	//}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
	{
		player->KillPlayer();
	}

	if (level != LVL1 && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		level = LVL1;
		loadScene = false;
		Engine::GetInstance().fade.get()->Fade(this, this);
	}

	if (level != LVL2 && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		level = LVL2;
		loadScene = false;
		Engine::GetInstance().fade.get()->Fade(this, this);
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		loadScene = false;
		Engine::GetInstance().fade.get()->Fade(this, this);
		
	}

	if (changeLevel || level == LVL1 && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
	{
		changeLevel = false;
		SaveState();
		level = LVL2;
		Engine::GetInstance().fade.get()->Fade(this, this);
		return true;
	}

	//if (player->won || level == LVL2 && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) {

	//	player->won = false;
	//	Engine::GetInstance().win.get()->finalCandyNum = player->pickedCandies;
	//	Engine::GetInstance().fade.get()->Fade(this, Engine::GetInstance().win.get());
	//	
	//	
	//	return true;
	//}

	if (loadScene)
	{
		//LOG("ENTRO");
		LoadState();
		loadScene = false;
	}
	
	if (!paused) {
		currentTime += dt / 1000.0f;

		/*if (player->position.getX() < POS_TO_START_MOVING_CAMX) {
			Engine::GetInstance().render.get()->camera.x = (POS_TO_START_MOVING_CAMX + CAM_EXTRA_DISPLACEMENT_X) * -Engine::GetInstance().window.get()->scale;
		}
		else if (player->position.getX() > POS_TO_STOP_MOVING_CAMX) Engine::GetInstance().render.get()->camera.x = (POS_TO_STOP_MOVING_CAMX + CAM_EXTRA_DISPLACEMENT_X) * -Engine::GetInstance().window.get()->scale;
		else Engine::GetInstance().render.get()->camera.x = (player->position.getX() + CAM_EXTRA_DISPLACEMENT_X) * -Engine::GetInstance().window.get()->scale;*/
		/*Engine::GetInstance().render.get()->camera.x = (player->position.getX() + CAM_EXTRA_DISPLACEMENT_X_LEFT) * -Engine::GetInstance().window.get()->scale;*/
		//camera y
		/*if (player->position.getY() > POS_TO_START_MOVING_CAMY) {
			Engine::GetInstance().render.get()->camera.y = (POS_TO_START_MOVING_CAMY + CAM_EXTRA_DISPLACEMENT_Y) * -Engine::GetInstance().window.get()->scale;
		}
		else if (player->position.getY() < POS_TO_STOP_MOVING_CAMY) Engine::GetInstance().render.get()->camera.y = (POS_TO_STOP_MOVING_CAMY + CAM_EXTRA_DISPLACEMENT_X) * -Engine::GetInstance().window.get()->scale;
		else Engine::GetInstance().render.get()->camera.y = (player->position.getY() + CAM_EXTRA_DISPLACEMENT_Y) * -Engine::GetInstance().window.get()->scale;*/
		Engine::GetInstance().render.get()->camera.y = (player->pbody->GetPhysBodyWorldPosition().getY() +CAM_EXTRA_DISPLACEMENT_Y) * -Engine::GetInstance().window.get()->scale;

		ChangeDirectionCameraX();
	}

	if (!bossKilled && startBossFight)
	{
		
		Engine::GetInstance().map.get()->ActivateBossFightCollider(true);
		if (!bossMusPlaying) {
			musicNode = Engine::GetInstance().GetConfig().child("audio").child("music");
			Engine::GetInstance().audio.get()->PlayMusic(musicNode.child("bossMus").attribute("path").as_string(), 0.0f);
			bossMusPlaying = true;
		}
		
		
	}
	else Engine::GetInstance().map.get()->ActivateBossFightCollider(false);

	
	timerText = std::to_string((int)currentTime);
	secondText = "s";
	Engine::GetInstance().render.get()->DrawText(timerText.c_str(), 1050, 29, 20, 20);
	Engine::GetInstance().render.get()->DrawText(secondText.c_str(), 1073, 30, 15, 18);
	
	pickaxeText = std::to_string((int)player->pickaxeManager->GetNumPickaxes()) + " pickaxes";
	Engine::GetInstance().render.get()->DrawText(pickaxeText.c_str(), 800, 50, 200, 18);

	if (player->pickaxeManager->GetNumPickaxes() < MAX_PICKAXES) {
		std::string number = std::to_string(player->pickaxeManager->pickaxeRecollectCount - player->pickaxeManager->pickaxeRecollectTimer.ReadSec());
		number.resize(3);
		timeTilPickaxeText = "time until next pickage: " + number + "s";
		Engine::GetInstance().render.get()->DrawText(timeTilPickaxeText.c_str(), 800, 70, 400, 18);
	}
	
	livesText = "hits left: " + std::to_string((int)player->hits);
	Engine::GetInstance().render.get()->DrawText(livesText.c_str(), 800, 90, 200, 18);

	currencyText = "currency: " + std::to_string((int)player->currencyManager->GetCurrency());
	Engine::GetInstance().render.get()->DrawText(currencyText.c_str(), 800, 110, 200, 18);

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{

	
	bool ret = true;

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		SaveState();
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		
		LoadState();
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN) {
		help = !help;
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		paused = !paused;
	}

	Render* render = Engine::GetInstance().render.get();
	Window* window = Engine::GetInstance().window.get();

	

	//UI
	if (!Engine::GetInstance().settings.get()->settingsOpen) {


		//life containers
		//int hearts = player->lives;

		//for (int i = 0; i < hearts; i++) {
		//	SDL_Rect rect = { 0, player->transformed ? 16 : 32, 16, 16 };
		//	Engine::GetInstance().render.get()->DrawTexture(
		//		heartsTexture,
		//		-render->camera.x / window->scale + i * 16 + 10,
		//		-render->camera.y / window->scale + 10,
		//		&rect
		//	);
		//}
	
		
		Engine::GetInstance().render.get()->DrawTexture(
			caramelsTexture, 
			-render->camera.x / window->scale + 580, 
			-render->camera.y / window->scale + 10);

		
		//timer
		sand.Update();
		Engine::GetInstance().render.get()->DrawTexture(
			sandclockTexture,
			-render->camera.x / window->scale + 505,
			-render->camera.y / window->scale + 9, 
			&sand.GetCurrentFrame());

		if (paused && !Engine::GetInstance().settings.get()->settingsOpen) {
			

			Engine::GetInstance().render.get()->DrawRectangle({ -render->camera.x / window->scale , - render->camera.y / window->scale, window->width, window->height}, 0, 0, 0, 200, true, true);
			Engine::GetInstance().render.get()->DrawTexture(pausePanel, -render->camera.x / window->scale + pausePos.getX(), -render->camera.y / window->scale + pausePos.getY());

			for (const auto& bt : pauseButtons) {
				if (bt.second->active == false) {
					bt.second->active = true;
				}
				else {
					bt.second->Update(_dt);
					OnGuiMouseClickEvent(bt.second);

				}

			}

			if (Engine::GetInstance().settings.get()->settingsOpen)
				for (const auto& bt : pauseButtons)
					bt.second->state = GuiControlState::DISABLED;
			else
				for (const auto& bt : pauseButtons)
					bt.second->state = GuiControlState::NORMAL;
		}
		else {
			for (const auto& bt : pauseButtons)
				bt.second->active = false;
		}

		if (help)
			render->DrawTexture(helpMenu, -render->camera.x / window->scale + helpPos.getX(), -render->camera.y / window->scale + helpPos.getY());
			
			

		if (quit) return false;

		return ret;
	}
}

// Called before quitting
bool Scene::CleanUp()
{
	Engine::GetInstance().textures.get()->UnLoad(heartsTexture);
	Engine::GetInstance().textures.get()->UnLoad(sandclockTexture);
	Engine::GetInstance().textures.get()->UnLoad(caramelsTexture);
	Engine::GetInstance().map.get()->CleanUp();
	Engine::GetInstance().physics.get()->DeleteAllPhysBody();
	Engine::GetInstance().entityManager.get()->Disable();
	

	enemies.clear();
	pumpkins.clear();
	candies.clear();

	
	
	LOG("Freeing scene");
	return true;
}

// Return the player position
Vector2D Scene::GetPlayerPosition()
{
	return player->GetPosition();
}

void Scene::SaveState()
{
	
	pugi::xml_document saveFile;
	pugi::xml_parse_result result = saveFile.load_file("config.xml");

	saveFile.child("config").child("scene").child("savedData").attribute("saved").set_value(true);
	saveFile.child("config").child("scene").child("savedData").attribute("level").set_value((int)level);
	saveFile.child("config").child("scene").child("savedData").attribute("time").set_value(currentTime);
	saveFile.child("config").child("scene").child("savedData").attribute("startBossFight").set_value(startBossFight);
	saveFile.child("config").child("scene").child("savedData").attribute("bossKilled").set_value(bossKilled);

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node savedDataNode = saveFile.child("config").child("scene").child("savedData").child(GetCurrentLevelString().c_str());

	//Save info to XML 
	//Player 
	player->SaveData(savedDataNode.child("player"));

	//Enemies
	for (int i = 0; i < enemies.size(); i++)
	{
		std::string nodeChar = "enemy" + std::to_string(i);
		pugi::xml_node parent = savedDataNode.child(nodeChar.c_str());

		if (!parent) {
			parent = savedDataNode.append_child(nodeChar.c_str());
			parent.append_attribute("dead");
			parent.append_attribute("x");
			parent.append_attribute("y");
		}

		enemies[i]->SaveData(parent);
	}

	//Pumpkins
	for (int i = 0; i < pumpkins.size(); i++)
	{
		std::string nodeChar = "pumpkin" + std::to_string(i);
		pugi::xml_node parent = savedDataNode.child(nodeChar.c_str());

		if (!parent) {
			parent = savedDataNode.append_child(nodeChar.c_str());
			parent.append_attribute("alight");
			parent.append_attribute("x");
			parent.append_attribute("y");
		}

		pumpkins[i]->SaveData(parent);
	}


	//Candies
	for (int i = 0; i < candies.size(); i++)
	{
		std::string nodeChar = "candy" + std::to_string(i);
		pugi::xml_node parent = savedDataNode.child(nodeChar.c_str());

		if (!parent) {
			parent = savedDataNode.append_child(nodeChar.c_str());
			parent.append_attribute("x");
			parent.append_attribute("y");
			parent.append_attribute("type");
			parent.append_attribute("picked");
		}

		candies[i]->SaveData(parent);
	}

	//Saves the modifications to the XML 
	saveFile.save_file("config.xml");
	Engine::GetInstance().ReloadConfig();
}

void Scene::LoadState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL) {
		LOG("Error loading config.xml");
		return;
	}

	currentTime = loadFile.child("config").child("scene").child("savedData").attribute("time").as_float();
	startBossFight = loadFile.child("config").child("scene").child("savedData").attribute("startBossFight").as_bool();
	bossKilled = loadFile.child("config").child("scene").child("savedData").attribute("bossKilled").as_bool();

	pugi::xml_node savedDataNode = loadFile.child("config").child("scene").child("savedData").child(GetCurrentLevelString().c_str());

	player->LoadData(savedDataNode.child("player"));


	//TODO: add an attribute to tell enemies from first and second level apart
	for (int i = 0; i < enemies.size(); i++)
	{
		std::string nodeChar = "enemy" + std::to_string(i);
		pugi::xml_node parent = savedDataNode.child(nodeChar.c_str());
		if (parent)
		{
			enemies[i]->LoadData(parent);
		}
	}

	for (int i = 0; i < candies.size(); i++)
	{
		std::string nodeChar = "candy" + std::to_string(i);
		pugi::xml_node parent = savedDataNode.child(nodeChar.c_str());
		if (parent)
		{
			candies[i]->LoadData(parent);
		}
	}

	for (int i = 0; i < pumpkins.size(); i++)
	{
		std::string nodeChar = "pumpkin" + std::to_string(i);
		pugi::xml_node parent = savedDataNode.child(nodeChar.c_str());
		if (parent)
		{
			pumpkins[i]->LoadData(parent);
		}
	}

	loadFile.save_file("config.xml");
}

void Scene::LoadTimeLivesCandies() {

	int previousLevel = (int)level - 1;
	std::string previousLevelString = GetLevelString(Levels(previousLevel));

	LOG("%s", previousLevelString);

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL) {
		LOG("Error loading config.xml");
		return;
	}



	pugi::xml_node savedDataNode = loadFile.child("config").child("scene").child("savedData");

	currentTime = savedDataNode.attribute("time").as_float();

	loadFile.save_file("config.xml");
}

void Scene::SetLoadState(bool b)
{
	loadScene = b;
}


bool Scene::OnGuiMouseClickEvent(GuiControl* control) {

	switch (control->id) {
	case GuiControlId::RESUME:
		if (control->state == GuiControlState::PRESSED) {
			paused = false;
		}

		break;
	case GuiControlId::OPTIONS:
		if (control->state == GuiControlState::PRESSED) {
			if (!Engine::GetInstance().settings.get()->settingsOpen) {
				Engine::GetInstance().settings.get()->settingsOpen = true;
			}
		}
		break;

	case GuiControlId::BACKTOTITLE:
		if (control->state == GuiControlState::PRESSED) {
			Engine::GetInstance().fade.get()->Fade((Module*)this, (Module*)Engine::GetInstance().mainMenu.get(), 30);

		}
		break;
	case GuiControlId::QUIT:
		if (control->state == GuiControlState::PRESSED) {
			quit = true;
		}
		break;
	}

	

	return true;
}

void Scene::SetGuiParameters(GuiControl* bt, std::string btName, pugi::xml_node parameters) {

	bt->id = (GuiControlId)parameters.child(btName.c_str()).attribute("id").as_int();

	bt->bounds.x = parameters.child(btName.c_str()).attribute("x").as_int();
	bt->bounds.y = parameters.child(btName.c_str()).attribute("y").as_int();
	bt->bounds.w = parameters.child(btName.c_str()).attribute("w").as_int();
	bt->bounds.h = parameters.child(btName.c_str()).attribute("h").as_int();

	bt->texture = Engine::GetInstance().textures.get()->Load(parameters.child(btName.c_str()).attribute("texture").as_string());
}

void Scene::ChangeLevel()
{
	changeLevel = true;
}


std::string Scene::GetCurrentLevelString()
{
	return "lvl" + std::to_string((int)level);
}

std::string Scene::GetLevelString(Levels lvl)
{
	return "lvl" + std::to_string((int)lvl);
}

void Scene::SetLevel(Levels lvl)
{
	level = lvl;
}

bool Scene::ReloadParameters(pugi::xml_node parameters)
{
	LoadParameters(parameters);
	if (player)
	{
		player->SetParameters(configParameters.child("entities").child("player"));
	}
	return true;
}

bool Scene::GetStartBossFight()
{
	return startBossFight;
}

void Scene::SetStartBossFight(bool b)
{
	startBossFight = b;
}
void Scene::SetBossFightKilled(bool b)
{
	bossKilled = b;
}
void Scene::ChangeDirectionCameraX()
{
	if (cameraDirectionChangeActivation) {
		int currentDisplace = transitionDisplace;

		Engine::GetInstance().render.get()->camera.x = (player->pbody->GetPhysBodyWorldPosition().getX() - (Engine::GetInstance().window.get()->width / 2) + currentDisplace) * -Engine::GetInstance().window.get()->scale;

		if (player->dir == RIGHT) {
			if (transitionDisplace < CAM_EXTRA_DISPLACEMENT_X) transitionDisplace += 2; 
			else cameraDirectionChangeActivation = false;
		}
		else if (player->dir == LEFT) {
			if (transitionDisplace > 0) transitionDisplace -= 2;
			else cameraDirectionChangeActivation = false;
		}
	}
	else {
		int finalDisplace = (player->dir == RIGHT) ? CAM_EXTRA_DISPLACEMENT_X : 0;
		Engine::GetInstance().render.get()->camera.x = (player->pbody->GetPhysBodyWorldPosition().getX() - (Engine::GetInstance().window.get()->width / 2) + finalDisplace) * -Engine::GetInstance().window.get()->scale;
	}

}

