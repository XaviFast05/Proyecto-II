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
#include "CheckPoint.h"
#include "Physics.h"
#include "BatEnemy.h"
#include "GroundEnemy.h"
#include "ChildEnemy.h"
#include "JumpingEnemy.h"
#include "RunningEnemy.h"
#include "FinalBoss.h"
#include "SoulRock.h"
#include "Ally.h"
#include "Merchant.h"
#include <string>
#include "Particle.h"
#include "tracy/Tracy.hpp"
#include "GuiControl.h"
#include "GuiManager.h"
#include "GuiControlButton.h"
#include "MainMenu.h"
#include "FadeToBlack.h"
#include "Settings.h"
#include "DeathMenu.h"
#include "WinMenu.h"
#include "PickaxeManager.h"
#include "CurrencyManager.h"
#include "UpgradesMenu.h"
#include "MerchantMenu.h"

#include "Intro.h"

Scene::Scene(bool startEnabled) : Module(startEnabled)
{

	name = "scene";
	player = nullptr;
	/*active = false;*/
}

// Destructor
Scene::~Scene()
{
}

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

	for (pugi::xml_node checkPointNode : configParameters.child("entities").child("items").child("checkPoints").child("instances").child(GetCurrentLevelString().c_str()).children())
	{
		CheckPoint* checkPoint = (CheckPoint*)Engine::GetInstance().entityManager->CreateEntity((EntityType)checkPointNode.attribute("entityType").as_int());;
		LoadItem(checkPoint, checkPointNode);
	}

	for (pugi::xml_node soulRockNode : configParameters.child("entities").child("items").child("soulRock").child("instances").child(GetCurrentLevelString().c_str()).children())
	{
		SoulRock* soulRock = (SoulRock*)Engine::GetInstance().entityManager->CreateEntity((EntityType)soulRockNode.attribute("entityType").as_int());;
		LoadSoulRock(soulRock, soulRockNode);
	}

	for (pugi::xml_node alliesNode : configParameters.child("entities").child("allies").child("merchant").child("instances").child(GetCurrentLevelString().c_str()).children())
	{
		Merchant* ally = (Merchant*)Engine::GetInstance().entityManager->CreateEntity((EntityType)alliesNode.attribute("entityType").as_int());;
		LoadAlly(ally, alliesNode);
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
		bt->SetGuiParameters(buttonName, pauseBtNode);
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
	else if (level == LVL3) {
		Engine::GetInstance().audio.get()->PlayMusic(musicNode.child("lvl2Mus").attribute("path").as_string());
	}

	startBossFight = false;
	bossMusPlaying = false;
	bossKilled = false;


	//UI
	heartsTexture = Engine::GetInstance().textures.get()->Load(configParameters.child("ui").child("heartContainers").attribute("path").as_string());
	piquetaNormal = Engine::GetInstance().textures.get()->Load(configParameters.child("ui").child("piquetaNormal").attribute("path").as_string());
	piquetaGastada = Engine::GetInstance().textures.get()->Load(configParameters.child("ui").child("piquetaGastada").attribute("path").as_string());
	barraPiqueta = Engine::GetInstance().textures.get()->Load(configParameters.child("ui").child("barraPiqueta").attribute("path").as_string());
	barraRoja = Engine::GetInstance().textures.get()->Load(configParameters.child("ui").child("barraRoja").attribute("path").as_string());
	orbSoul = Engine::GetInstance().textures.get()->Load(configParameters.child("ui").child("orbSoul").attribute("path").as_string());
	bgTutorial = Engine::GetInstance().textures.get()->Load(configParameters.child("ui").child("bgTut").attribute("path").as_string());
	kimHead = Engine::GetInstance().textures.get()->Load(configParameters.child("ui").child("kimHead").attribute("path").as_string());

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

void Scene::LoadSoulRock(SoulRock* soulRock, pugi::xml_node instanceNode) {

	soulRock->SetParameters(configParameters.child("entities").child("items").child("soulRock"));
	soulRock->SetInstanceParameters(instanceNode);
	soulRocks.push_back(soulRock);

}

void Scene::LoadItem(CheckPoint* checkPoint, pugi::xml_node instanceNode) {

	checkPoint->SetPlayer(player);
	checkPoint->SetParameters(configParameters.child("entities").child("items").child("checkPoints"));
	checkPoint->SetInstanceParameters(instanceNode);
	checkPoints.push_back(checkPoint);
}

void Scene::LoadAlly(Merchant* merchant, pugi::xml_node instanceNode) {

	merchant->SetPlayer(player);
	merchant->SetParameters(configParameters.child("entities").child("allies").child("merchant"));
	merchant->SetInstanceParameters(instanceNode);
	allies.push_back(merchant);

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

	if (level != LVL3 && Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
	{
		level = LVL3;
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
		int playerPOSX = player->GetPosition().getX();
		int playerPOSY = player->GetPosition().getY();
		changeLevel = false;
		if (level == LVL1)
		{
			Engine::GetInstance().fade.get()->Fade((Module*)this, (Module*)this, 30);
			level = LVL3;
		}
		else if (level == LVL3)
		{
			if (playerPOSX < 6000) {
				Engine::GetInstance().fade.get()->Fade((Module*)this, (Module*)this, 30);
				level = LVL4;
			}
			else if (playerPOSX > 9000) {
				Engine::GetInstance().fade.get()->Fade((Module*)this, (Module*)this, 30);
				level = LVL5;
			}
		}
		else if (level == LVL4)
		{
			Engine::GetInstance().fade.get()->Fade((Module*)this, (Module*)this, 30);
			level = LVL3;
		}
		else if (level == LVL5)
		{
			if (playerPOSY < 1500)
			{
				Engine::GetInstance().fade.get()->Fade((Module*)this, (Module*)this, 30);
				level = LVL3;
			}
			else if (playerPOSY > 1600)
			{
				Engine::GetInstance().fade.get()->Fade((Module*)this, (Module*)this, 30);
				level = LVL3;
			}
		}
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

		//CAMERA X
		ChangeDirectionCameraX();

		//CAMERA Y
		Engine::GetInstance().render.get()->camera.y = (METERS_TO_PIXELS(player->pbody->body->GetPosition().y) + CAM_EXTRA_DISPLACEMENT_Y) * -Engine::GetInstance().window.get()->scale;

	
		if (level == LVL1)
		{
			//CAMERA LIMITS X
			if (Engine::GetInstance().render.get()->camera.x < -9980) Engine::GetInstance().render.get()->camera.x = -9980;
			else if (Engine::GetInstance().render.get()->camera.x > -35) Engine::GetInstance().render.get()->camera.x = -35;

			//CAMERA LIMITS Y
			if (Engine::GetInstance().render.get()->camera.y < -4088) Engine::GetInstance().render.get()->camera.y = -4088;
			else if (Engine::GetInstance().render.get()->camera.y > 0) Engine::GetInstance().render.get()->camera.y = 0;
		}
		else if (level == LVL2)
		{
			//CAMERA LIMITS X
			if (Engine::GetInstance().render.get()->camera.x < -9980) Engine::GetInstance().render.get()->camera.x = -9980;
			else if (Engine::GetInstance().render.get()->camera.x > -35) Engine::GetInstance().render.get()->camera.x = -35;

			//CAMERA LIMITS Y
			if (Engine::GetInstance().render.get()->camera.y < -4088) Engine::GetInstance().render.get()->camera.y = -4088;
			else if (Engine::GetInstance().render.get()->camera.y > 0) Engine::GetInstance().render.get()->camera.y = 0;
		}
		else if (level == LVL3)
		{

		}
		

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
	//Engine::GetInstance().render.get()->DrawText(timerText.c_str(), 1050, 29, 20, 20);
	//Engine::GetInstance().render.get()->DrawText(secondText.c_str(), 1073, 30, 15, 18);

	//pickaxeText = std::to_string((int)player->pickaxeManager->GetNumPickaxes()) + " pickaxes";
	//Engine::GetInstance().render.get()->DrawText(pickaxeText.c_str(), 800, 50, 200, 18);

	if (player->projectileManager->GetNumPickaxes() < player->maxPickaxes) {
		std::string number = std::to_string(player->projectileManager->pickaxeRecollectCount - player->projectileManager->pickaxeRecollectTimer.ReadSec());
		number.resize(3);
		//timeTilPickaxeText = "time until next pickage: " + number + "s";
		//Engine::GetInstance().render.get()->DrawText(timeTilPickaxeText.c_str(), 800, 70, 400, 18);
	}

	//livesText = "hits left: " + std::to_string((int)player->hits);
	//Engine::GetInstance().render.get()->DrawText(livesText.c_str(), 800, 90, 200, 18);

	//currencyText = "currency: " + std::to_string((int)player->currencyManager->GetCurrency());
	//Engine::GetInstance().render.get()->DrawText(currencyText.c_str(), 800, 110, 200, 18);

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
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_START) == KEY_DOWN) {
		paused = !paused;
		Engine::GetInstance().settings.get()->settingsOpen = false;
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN || Engine::GetInstance().input.get()->GetGamepadButton(SDL_CONTROLLER_BUTTON_BACK) == KEY_DOWN)
	{
		drawnMap = !drawnMap;
	}

	Render* render = Engine::GetInstance().render.get();
	Window* window = Engine::GetInstance().window.get();

	//UI
	if (!Engine::GetInstance().settings.get()->settingsOpen) {
		
		if (!paused) {
			DrawPlayerHitsUI();

			DrawPickaxesUI();

			DrawCurrencyUI();

			if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_TAB)) DrawMap();
		}

		if (paused && !Engine::GetInstance().settings.get()->settingsOpen) {

			if (!Engine::GetInstance().upgradesMenu.get()->upgradesOpen) {
				Engine::GetInstance().render.get()->DrawRectangle({ -render->camera.x / window->scale , -render->camera.y / window->scale, window->width, window->height }, 0, 0, 0, 200, true, true);
				Engine::GetInstance().render.get()->DrawTextureBuffer(pausePanel, -render->camera.x / window->scale + pausePos.getX(), -render->camera.y / window->scale + pausePos.getY(), false, MENUS);

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
		}
		else {
			for (const auto& bt : pauseButtons)
				bt.second->active = false;
		}

		if (help) render->DrawTextureBuffer(helpMenu, -render->camera.x / window->scale + helpPos.getX(), -render->camera.y / window->scale + helpPos.getY());
			
		if (drawnMap) DrawMap();

		if (quit) return false;

		return ret;
	}
}



// Called before quitting
bool Scene::CleanUp()
{
	Engine::GetInstance().textures.get()->UnLoad(heartsTexture);
	Engine::GetInstance().textures.get()->UnLoad(piquetaNormal);
	Engine::GetInstance().textures.get()->UnLoad(piquetaNormal);
	Engine::GetInstance().map.get()->CleanUp();
	Engine::GetInstance().physics.get()->DeleteAllPhysBody();
	Engine::GetInstance().entityManager.get()->Disable();


	enemies.clear();
	checkPoints.clear();
	soulRocks.clear();
	allies.clear();



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
	pugi::xml_parse_result result = saveFile.load_file("savedData.xml");

	if (result == NULL) {
		LOG("Error loading saveData.xml");
		return;
	}


	pugi::xml_node savedDataNode = saveFile.child("savedData").child(GetCurrentLevelString().c_str());
	pugi::xml_node upgradesNode = saveFile.child("savedData").child("upgrades");

	savedDataNode.attribute("saved").set_value(true);
	savedDataNode.attribute("level").set_value((int)level);
	savedDataNode.attribute("time").set_value(currentTime);
	savedDataNode.attribute("startBossFight").set_value(startBossFight);
	savedDataNode.attribute("bossKilled").set_value(bossKilled);

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}



	//Save info to XML 
	//Player 
	player->SaveData(savedDataNode.child("player"), upgradesNode);

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

	//CheckPoints
	for (int i = 0; i < checkPoints.size(); i++)
	{
		std::string nodeChar = "checkPoint" + std::to_string(i);
		pugi::xml_node parent = savedDataNode.child(nodeChar.c_str());

		if (!parent) {
			parent = savedDataNode.append_child(nodeChar.c_str());
			parent.append_attribute("alight");
			parent.append_attribute("x");
			parent.append_attribute("y");
		}

		checkPoints[i]->SaveData(parent);
	}

	////SoulRocks
	//for (int i = 0; i < checkPoints.size(); i++)
	//{
	//	std::string nodeChar = "soulRock" + std::to_string(i);
	//	pugi::xml_node parent = savedDataNode.child(nodeChar.c_str());

	//	if (!parent) {
	//		parent = savedDataNode.append_child(nodeChar.c_str());
	//		parent.append_attribute("alight");
	//		parent.append_attribute("x");
	//		parent.append_attribute("y");
	//	}

	//	checkPoints[i]->SaveData(parent);
	//}

	//Saves the modifications to the XML 
	saveFile.save_file("savedData.xml");
}

void Scene::LoadState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("savedData.xml");

	if (result == NULL) {
		LOG("Error loading saveData.xml");
		return;
	}

	pugi::xml_node savedDataNode = loadFile.child("savedData").child(GetCurrentLevelString().c_str());
	pugi::xml_node upgradesNode = loadFile.child("savedData").child("upgrades");

	currentTime = savedDataNode.attribute("time").as_float();
	startBossFight = savedDataNode.attribute("startBossFight").as_bool();
	bossKilled = savedDataNode.attribute("bossKilled").as_bool();

	player->LoadData(savedDataNode.child("player"), upgradesNode);

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

	for (int i = 0; i < checkPoints.size(); i++)
	{
		std::string nodeChar = "checkPoint" + std::to_string(i);
		pugi::xml_node parent = savedDataNode.child(nodeChar.c_str());
		if (parent)
		{
			checkPoints[i]->LoadData(parent);
		}
	}

	//for (int i = 0; i < soulRocks.size(); i++)
	//{
	//	std::string nodeChar = "soulRock" + std::to_string(i);
	//	pugi::xml_node parent = savedDataNode.child(nodeChar.c_str());
	//	if (parent)
	//	{
	//		soulRocks[i]->LoadData(parent);
	//	}
	//}

	loadFile.save_file("savedData.xml");
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

	case GuiControlId::CHANGE_MENU:
		if (control->state == GuiControlState::PRESSED) {
			if (!Engine::GetInstance().upgradesMenu.get()->upgradesOpen) {
				Engine::GetInstance().upgradesMenu.get()->hasOpened = true;
				Engine::GetInstance().upgradesMenu.get()->upgradesOpen = true;
				if (Engine::GetInstance().settings.get()->settingsOpen) {
					Engine::GetInstance().settings.get()->settingsOpen = false;
				}
			}
		}
		break;
	}

	return true;
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
	//if (Engine::GetInstance().render.get()->camera.x)
	if (cameraDirectionChangeActivation) {
		int currentDisplace = transitionDisplace;

		Engine::GetInstance().render.get()->camera.x = (METERS_TO_PIXELS(player->pbody->body->GetPosition().x) - (Engine::GetInstance().window.get()->width / 2) + currentDisplace) * -Engine::GetInstance().window.get()->scale;

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
		Engine::GetInstance().render.get()->camera.x = (METERS_TO_PIXELS(player->pbody->body->GetPosition().x) - (Engine::GetInstance().window.get()->width / 2) + finalDisplace) * -Engine::GetInstance().window.get()->scale;
	}

}

void Scene::DrawPlayerHitsUI()
{
	// Tama�o de cada secci�n de la textura (224x224)
	const int sectionWidth = 224;
	const int sectionHeight = 224;

	// Calcular la secci�n de la textura a dibujar seg�n los hits restantes
	SDL_Rect section;
	section.x = (3 - player->hits) * sectionWidth; // Mover a la siguiente secci�n por cada golpe
	section.y = 0;
	section.w = sectionWidth;
	section.h = sectionHeight;

	// Dibujar la textura en la posici�n deseada
	Engine::GetInstance().render.get()->DrawTextureBuffer(
		heartsTexture, // Textura de los corazones
		-Engine::GetInstance().render.get()->camera.x / Engine::GetInstance().window.get()->scale + 10, // Posici�n X
		-Engine::GetInstance().render.get()->camera.y / Engine::GetInstance().window.get()->scale - 50,    // Posici�n Y
		false, HUD, &section // Secci�n de la textura a dibujar
	);
}

void Scene::DrawPickaxesUI()
{
	int numPickaxes = player->projectileManager->GetNumPickaxes(); // Piquetas disponibles
	int scale = Engine::GetInstance().window.get()->GetScale(); // Escala de la ventana

	// Posici�n inicial para dibujar las piquetas
	int startX = 100; // Coordenada X inicial
	int startY = 100;  // Coordenada Y inicial
	int spacing = 50; // Espaciado entre las piquetas
	int spacingRed = 8; // Espaciado entre las piquetas

	for (int i = 0; i < player->maxPickaxes; ++i) {
		// Si el �ndice es menor que el n�mero de piquetas disponibles, dibuja una piqueta normal
		if (i < numPickaxes) {
			Engine::GetInstance().render.get()->DrawTextureBuffer(
				piquetaNormal,
				-Engine::GetInstance().render.get()->camera.x / Engine::GetInstance().window.get()->scale + 200 + (i * spacing), // Posici�n X
				-Engine::GetInstance().render.get()->camera.y / Engine::GetInstance().window.get()->scale + 10, false, HUD    // Posici�n Y
			);
		}
		// Si no, dibuja una piqueta gastada
		else {
			Engine::GetInstance().render.get()->DrawTextureBuffer(
				piquetaGastada,
				-Engine::GetInstance().render.get()->camera.x / Engine::GetInstance().window.get()->scale + 200 + (i * spacing), // Posici�n X
				-Engine::GetInstance().render.get()->camera.y / Engine::GetInstance().window.get()->scale + 10 , false, HUD  // Posici�n Y
			);
			Engine::GetInstance().render.get()->DrawTextureBuffer(
				barraPiqueta,
				-Engine::GetInstance().render.get()->camera.x / Engine::GetInstance().window.get()->scale + 200 + (numPickaxes * spacing), // Posici�n X
				-Engine::GetInstance().render.get()->camera.y / Engine::GetInstance().window.get()->scale + 80 , false , HUD   // Posici�n Y
			);
			int redBars = player->projectileManager->GetNumRed();
			int drawRedSpacing = 0;
			for (int i = 0; i < redBars; i++) {
				Engine::GetInstance().render.get()->DrawTextureBuffer(
					barraRoja,
					-Engine::GetInstance().render.get()->camera.x / Engine::GetInstance().window.get()->scale + 200 + (numPickaxes * spacing) + drawRedSpacing, // Posici�n X
					-Engine::GetInstance().render.get()->camera.y / Engine::GetInstance().window.get()->scale + 80 , false , HUD   // Posici�n Y
				);
				drawRedSpacing += spacingRed;
			}
		}
	}
}

void Scene::DrawCurrencyUI()
{
	int scale = Engine::GetInstance().window.get()->GetScale(); // Escala de la ventana

	// Dibujar la textura del soul orb
	Engine::GetInstance().render.get()->DrawTextureBuffer(
		orbSoul,
		-Engine::GetInstance().render.get()->camera.x / Engine::GetInstance().window.get()->scale + 1120, // Posici�n X
		-Engine::GetInstance().render.get()->camera.y / Engine::GetInstance().window.get()->scale + 10,  // Posici�n Y
		false, HUD
	);


	// Texto que muestra el n�mero de monedas
	std::string currencyText = std::to_string(player->currencyManager->GetCurrency());

	// Dibujar el texto del n�mero de monedas
	Engine::GetInstance().render.get()->DrawTextToBuffer(currencyText.c_str(), 1200, 32, 48, 32, Engine::GetInstance().render.get()->font, {255,255,255,255}, MENUS);
}

void Scene::DrawMap()
{
	// 3 x 68.465

	float posX = player->pbody->body->GetPosition().x;
	float posY = player->pbody->body->GetPosition().y;
	int centerX = -Engine::GetInstance().render.get()->camera.x / Engine::GetInstance().window.get()->scale + (Engine::GetInstance().window.get()->width / 2) - 360;
	int centerY = -Engine::GetInstance().render.get()->camera.y / Engine::GetInstance().window.get()->scale + (Engine::GetInstance().window.get()->height / 2) - 180;
	switch (level) {
	case LVL1:
		Engine::GetInstance().render.get()->DrawTextureBuffer(bgTutorial, centerX, centerY, false, MENUS);
		Engine::GetInstance().render.get()->DrawTextureBuffer(kimHead, centerX + (posX * 720 / 225 - 10), centerY + (posY * 360 / 96 - 25), false, MENUS);
		break;
	case LVL2:
		break;
	case LVL3:
		break;
	case LVL4:
		break;
	case LVL5:
		break;
	case LVL6:
		break;

	default:
		break;
	}
}