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
#include "Bullet.h"
#include "AmmoCharger.h"
#include "LifeCharger.h"
#include "PointsContainer.h"
#include "MetallC15.h"
#include "BattonBone.h"
#include "GuiControl.h"
#include "GuiManager.h"

Scene::Scene() : Module()
{
	name = "scene";

}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	stateSaved = configParameters.child("currentGame").attribute("gameSaved").as_bool();
	minutesTimer = configParameters.child("currentGame").attribute("minute").as_int();
	secondsTimer = configParameters.child("currentGame").attribute("second").as_int();
	scorePoints = configParameters.child("currentGame").attribute("score").as_int();

	printf("STATE: %i", stateSaved); 

	//L04: TODO 3b: Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	player->SetParameters(configParameters.child("entities").child("player"));
	

	//L08 Create a new item using the entity manager and set the position to (200, 672) to test
	for (pugi::xml_node itemNode = configParameters.child("entities").child("items").child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		if (std::string(itemNode.attribute("name").as_string()) == "AmmoCharger") {

			AmmoCharger* ammoCharger = (AmmoCharger*)Engine::GetInstance().entityManager->CreateEntity(EntityType::AMMO_CHARGER);
			ammoCharger->SetParameters(itemNode);
			ammoChargerList.push_back(ammoCharger);

		}

		if (std::string(itemNode.attribute("name").as_string()) == "LifeCharger") {

			LifeCharger* lifeCharger = (LifeCharger*)Engine::GetInstance().entityManager->CreateEntity(EntityType::LIFE_CHARGER);
			lifeCharger->SetParameters(itemNode);
			lifeChargerList.push_back(lifeCharger);

		}

		if (std::string(itemNode.attribute("name").as_string()) == "PointsContainer") {

			PointsContainer* pointsContainer = (PointsContainer*)Engine::GetInstance().entityManager->CreateEntity(EntityType::POINTS_CONTAINER);
			pointsContainer->SetParameters(itemNode);
			pointsContainerList.push_back(pointsContainer);

		}
		
	}

	// Create a enemy using the entity manager 
	for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
	{
		// MetallC15 enemy creation
		if (std::string(enemyNode.attribute("name").as_string()) == "MetallC15") {

			MetallC15* metallC15 = (MetallC15*)Engine::GetInstance().entityManager->CreateEntity(EntityType::METALLC15);
			metallC15->SetParameters(enemyNode);
			metallC15List.push_back(metallC15);

		}
		
		// BattonBone enemy creation
		if (std::string(enemyNode.attribute("name").as_string()) == "BattonBone") {

			BattonBone* battonBone = (BattonBone*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BATTONBONE);
			battonBone->SetParameters(enemyNode);
			battonBoneList.push_back(battonBone);

		}




	}

	for (pugi::xml_node bulletNode = configParameters.child("entities").child("projectiles").child("bullet"); bulletNode; bulletNode = bulletNode.next_sibling("bullet"))
	{

		bulletConfig = bulletNode;

	}




	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());

	// Texture to highligh mouse position 
	mouseTileTex = Engine::GetInstance().textures.get()->Load("Assets/Maps/MapMetadata.png");

	// Initial Respawn activated
	initialRespawn = true;

	// Help menu initialization
	helpMenu = Engine::GetInstance().textures.get()->Load("Assets/Textures/HelpMenu.png");
	help = false;

	preparationTimer = 1.0f;
	activationPreparationTimer = false;

	introScreen = Engine::GetInstance().textures.get()->Load("Assets/Textures/IntroScreen.png");
	menuScreen = Engine::GetInstance().textures.get()->Load("Assets/Textures/MenuScreen.png");
	
	winScreen = Engine::GetInstance().textures.get()->Load("Assets/Textures/WinScreen.png");
	loseScreen = Engine::GetInstance().textures.get()->Load("Assets/Textures/LoseScreen.png");

	pauseMenu = Engine::GetInstance().textures.get()->Load("Assets/Textures/PauseMenu.png");
	activationPauseMenu = false;

	settingsMenu = Engine::GetInstance().textures.get()->Load("Assets/Textures/SettingsMenu.png");
	activationSettings = false;

	creditsScreen = Engine::GetInstance().textures.get()->Load("Assets/Textures/CreditsScreen.png");
	activationCredits = false;

	// Finished level detection desactivated
	finishLevel = false;

	changedButtons1 = false;
	changedButtons2 = false;
	changedButtons3 = false;

	selectedOption = false;

	exit = false;

	stateSaved = false;

	state = INTRO;

	deltaTime = 0;

	minutesTimer = 0;
	secondsTimer = 0;

	scorePoints = 0;

	// Music played
	startButton = Engine::GetInstance().textures.get()->Load("Assets/Textures/StartButton.png");
	Engine::GetInstance().textures->GetSize(startButton, width, height);

	SDL_Rect btPos1 = { 550, 450, width, height / 4 };
	guiBt1 = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "START", startButton, btPos1, this);
	guiBt1->state = GuiControlState::DISABLED;

	continueButton = Engine::GetInstance().textures.get()->Load("Assets/Textures/continueButton.png");
	Engine::GetInstance().textures->GetSize(continueButton, width, height);

	SDL_Rect btPos2 = { 550, 500, width, (height+4) / 4 };
	guiBt2 = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "CONTINUE", continueButton, btPos2, this);
	guiBt2->state = GuiControlState::DISABLED;

	settingsButton = Engine::GetInstance().textures.get()->Load("Assets/Textures/settingsButton.png");
	Engine::GetInstance().textures->GetSize(settingsButton, width, height);

	SDL_Rect btPos3 = { 550, 550, width, height / 4 };
	guiBt3 = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "SETTINGS", settingsButton, btPos3, this);
	guiBt3->state = GuiControlState::DISABLED;

	SDL_Rect btPos7 = { 504, 367, width, height / 4 };
	guiBt7 = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, "SETTINGS", settingsButton, btPos7, this);
	guiBt7->state = GuiControlState::DISABLED;

	creditsButton = Engine::GetInstance().textures.get()->Load("Assets/Textures/creditsButton.png");
	Engine::GetInstance().textures->GetSize(creditsButton, width, height);

	SDL_Rect btPos4 = { 550, 600, width, height / 4 };
	guiBt4 = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "CREDITS", creditsButton, btPos4, this);
	guiBt4->state = GuiControlState::DISABLED;

	exitButton = Engine::GetInstance().textures.get()->Load("Assets/Textures/exitButton.png");
	Engine::GetInstance().textures->GetSize(exitButton, width, height);

	SDL_Rect btPos5 = { 550, 650, width, height / 4 };
	guiBt5 = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "EXIT", exitButton, btPos5, this);
	guiBt5->state = GuiControlState::DISABLED;

	SDL_Rect btPos9 = { 578, 527, width, height / 4 };
	guiBt9 = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 9, "EXIT", exitButton, btPos9, this);
	guiBt9->state = GuiControlState::DISABLED;

	resumeButton = Engine::GetInstance().textures.get()->Load("Assets/Textures/resumeButton.png");
	Engine::GetInstance().textures->GetSize(resumeButton, width, height);

	SDL_Rect btPos6 = { 539, 287, width, height / 4 };
	guiBt6 = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, "RESUME", resumeButton, btPos6, this);
	guiBt6->state = GuiControlState::DISABLED;

	backToTitleButton = Engine::GetInstance().textures.get()->Load("Assets/Textures/backToTitleButton.png");
	Engine::GetInstance().textures->GetSize(backToTitleButton, width, height);

	SDL_Rect btPos8 = { 417, 447, width, height / 4 };
	guiBt8 = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 8, "BACK TO TITLE", backToTitleButton, btPos8, this);
	guiBt8->state = GuiControlState::DISABLED;

	checkboxButton = Engine::GetInstance().textures.get()->Load("Assets/Textures/CheckboxButton.png");
	Engine::GetInstance().textures->GetSize(checkboxButton, width, height);

	SDL_Rect chPos = { 804, 464, width, height / 8 };
	guiCh = (GuiControlCheckBox*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 10, "TOGGLE FULLSCREEN", checkboxButton, chPos, this);
	guiCh->state = GuiControlState::DISABLED;

	sliderButton = Engine::GetInstance().textures.get()->Load("Assets/Textures/SliderButton.png");
	Engine::GetInstance().textures->GetSize(sliderButton, width, height);

	SDL_Rect slPos1 = { 700, 289, width, height / 4 };
	SDL_Rect barlPos1 = { 700 - 64 - (width/2), 289 + (height / 4) / 2 - 3,  128, 6};
	guiSl1 = (GuiControlSlider*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::SLIDER, 11, "MUSIC", sliderButton, slPos1, this, barlPos1);
	guiSl1->state = GuiControlState::DISABLED;

	SDL_Rect slPos2 = { 700, 350, width, height / 4 };
	SDL_Rect barlPos2 = { 700 - 64 - (width / 2), 350 + (height / 4) / 2 - 3,  128, 6 };
	guiSl2 = (GuiControlSlider*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::SLIDER, 12, "FX", sliderButton, slPos2, this, barlPos2);
	guiSl2->state = GuiControlState::DISABLED;


	createBullet = false;

	pauseEntities = false;

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{

	// Respawns always on the initial point after the first step
	if (initialRespawn) {
		Engine::GetInstance().scene.get()->player->Respawn();
		initialRespawn = false;
	}

	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	Uint32 currentTick = SDL_GetTicks();
	deltaTime = (currentTick - lastTick) / 1000.0f; // Convert milliseconds to seconds
	lastTick = currentTick;

	switch (state) {

	case INTRO:


		Engine::GetInstance().textures->GetSize(introScreen, width, height);
		dstRect = { 0, 0, width, height };
		SDL_RenderCopy(Engine::GetInstance().render->renderer, introScreen, nullptr, &dstRect);

		if (activationPreparationTimer) {
			preparationTimer -= deltaTime;

		}

		if (preparationTimer <= 0.0f) {

			state = MENU;
			preparationTimer = 1.0f;
			activationPreparationTimer = false;
			guiBt1->state = GuiControlState::NORMAL;
			if (stateSaved) {
				guiBt2->state = GuiControlState::NORMAL;
			}
			else {
				guiBt2->state = GuiControlState::DISABLED;
				guiBt2->textureOnDisabled = true;
			}

			guiBt3->state = GuiControlState::NORMAL;
			guiBt4->state = GuiControlState::NORMAL;
			guiBt5->state = GuiControlState::NORMAL;
			Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/MenuMusic.wav", 0.0f);

		}

		break;
	case MENU:

		Engine::GetInstance().textures->GetSize(menuScreen, width, height);
		dstRect = { 0, 0, width, height };
		SDL_RenderCopy(Engine::GetInstance().render->renderer, menuScreen, nullptr, &dstRect);
		Engine::GetInstance().entityManager.get()->entities.begin();

		// SETTINGS
		if (activationSettings && !changedButtons1 && !activationCredits) {

			guiBt1->state = GuiControlState::DISABLED;
			guiBt2->state = GuiControlState::DISABLED;
			guiBt2->textureOnDisabled = false;
			guiBt3->state = GuiControlState::DISABLED;
			guiBt4->state = GuiControlState::DISABLED;
			guiBt5->state = GuiControlState::DISABLED;

			guiCh->state = GuiControlState::NORMAL;
			guiSl1->state = GuiControlState::NORMAL;
			guiSl2->state = GuiControlState::NORMAL;

			changedButtons1 = true;
		}
		else if (activationSettings && changedButtons1 && !activationCredits) {
			Engine::GetInstance().textures->GetSize(settingsMenu, width, height);
			SDL_Rect dstRect = { 0, 0, width, height };
			SDL_RenderCopy(Engine::GetInstance().render->renderer, settingsMenu, nullptr, &dstRect);
		}
		else if (!activationSettings && changedButtons1 && !activationCredits) {

			guiBt1->state = GuiControlState::NORMAL;
			if (stateSaved) {
				guiBt2->state = GuiControlState::NORMAL;
			}
			else {
				guiBt2->state = GuiControlState::DISABLED;
				guiBt2->textureOnDisabled = true;
			}
			guiBt3->state = GuiControlState::NORMAL;
			guiBt4->state = GuiControlState::NORMAL;
			guiBt5->state = GuiControlState::NORMAL;

			guiCh->state = GuiControlState::DISABLED;
			guiSl1->state = GuiControlState::DISABLED;
			guiSl2->state = GuiControlState::DISABLED;

			changedButtons1 = false;
		}

		// CREDITS
		if (activationCredits && !changedButtons1 && !activationSettings) {

			guiBt1->state = GuiControlState::DISABLED;
			guiBt2->state = GuiControlState::DISABLED;
			guiBt2->textureOnDisabled = false;
			guiBt3->state = GuiControlState::DISABLED;
			guiBt4->state = GuiControlState::DISABLED;
			guiBt5->state = GuiControlState::DISABLED;


			changedButtons1 = true;
		}
		else if (activationCredits && changedButtons1 && !activationSettings) {
			Engine::GetInstance().textures->GetSize(creditsScreen, width, height);
			SDL_Rect dstRect = { 0, 0, width, height };
			SDL_RenderCopy(Engine::GetInstance().render->renderer, creditsScreen, nullptr, &dstRect);
		}
		else if (!activationCredits && changedButtons1 && !activationSettings) {


			guiBt1->state = GuiControlState::NORMAL;
			if (stateSaved) {
				guiBt2->state = GuiControlState::NORMAL;
			}
			else {
				guiBt2->state = GuiControlState::DISABLED;
				guiBt2->textureOnDisabled = true;
			}
			guiBt3->state = GuiControlState::NORMAL;
			guiBt4->state = GuiControlState::NORMAL;
			guiBt5->state = GuiControlState::NORMAL;
			changedButtons1 = false;
		}


		// START / CONTINUE
		if (activationPreparationTimer) {

			preparationTimer -= deltaTime;

		}

		if (preparationTimer <= 0.0f) {

			Engine::GetInstance().map.get()->mapLevel = M_LEVEL1;
			state = LEVEL1;
			preparationTimer = 1.0f;
			activationPreparationTimer = false;
			Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/Level1Music.wav", 0.0f);
			guiBt1->state = GuiControlState::DISABLED;
			guiBt2->state = GuiControlState::DISABLED;
			guiBt2->textureOnDisabled = false;
			guiBt3->state = GuiControlState::DISABLED;
			guiBt4->state = GuiControlState::DISABLED;
			guiBt5->state = GuiControlState::DISABLED;
			minutesTimer = 0;
			secondsTimer = 0;
			scorePoints = 0;

		}

		break;
	case LEVEL1:

		if (!activationPauseMenu) {

			HelpMenu();

			if (!help) {
				Score();
				LevelTimer();
			}

		}

		HandleCamera();


		if (activationPauseMenu && !changedButtons1 && !activationSettings) {

			guiBt6->state = GuiControlState::NORMAL;
			guiBt7->state = GuiControlState::NORMAL;
			guiBt8->state = GuiControlState::NORMAL;
			guiBt9->state = GuiControlState::NORMAL;

			changedButtons1 = true;

		}
		else if (activationPauseMenu && changedButtons1 && !activationSettings) {

			Engine::GetInstance().textures->GetSize(pauseMenu, width, height);
			SDL_Rect dstRect = { 0, 0, width, height };
			SDL_RenderCopy(Engine::GetInstance().render->renderer, pauseMenu, nullptr, &dstRect);

			pauseEntities = true;
			Mix_PauseMusic();

		}
		else if (!activationPauseMenu && changedButtons1 && !activationSettings) {

			guiBt6->state = GuiControlState::DISABLED;
			guiBt7->state = GuiControlState::DISABLED;
			guiBt8->state = GuiControlState::DISABLED;
			guiBt9->state = GuiControlState::DISABLED;

			Mix_ResumeMusic();
			pauseEntities = false;
			changedButtons1 = false;
		}

		if (activationSettings && !changedButtons2 && changedButtons1) {

			guiBt6->state = GuiControlState::DISABLED;
			guiBt7->state = GuiControlState::DISABLED;
			guiBt8->state = GuiControlState::DISABLED;
			guiBt9->state = GuiControlState::DISABLED;

			guiCh->state = GuiControlState::NORMAL;
			guiSl1->state = GuiControlState::NORMAL;
			guiSl2->state = GuiControlState::NORMAL;

			changedButtons2 = true;
		}
		else if (activationSettings && changedButtons2 && changedButtons1) {
			Engine::GetInstance().textures->GetSize(settingsMenu, width, height);
			SDL_Rect dstRect = { 0, 0, width, height };
			SDL_RenderCopy(Engine::GetInstance().render->renderer, settingsMenu, nullptr, &dstRect);
		}
		else if (!activationSettings && changedButtons2 && changedButtons1) {

			guiBt6->state = GuiControlState::NORMAL;
			guiBt7->state = GuiControlState::NORMAL;
			guiBt8->state = GuiControlState::NORMAL;
			guiBt9->state = GuiControlState::NORMAL;

			guiCh->state = GuiControlState::DISABLED;
			guiSl1->state = GuiControlState::DISABLED;
			guiSl2->state = GuiControlState::DISABLED;

			changedButtons2 = false;
		}


		if (activationPreparationTimer) {

			preparationTimer -= deltaTime;
			lastState = LEVEL1;
		}

		if (finishLevel) {

			activationPreparationTimer = true;

			if (preparationTimer <= 0.0f) {

				state = WIN;
				preparationTimer = 1.0f;
				activationPreparationTimer = false;
				finishLevel = false;

			}

		}


		if (preparationTimer <= 0.0f && activationPauseMenu) {

			pauseEntities = false;
			guiBt6->state = GuiControlState::DISABLED;
			guiBt7->state = GuiControlState::DISABLED;
			guiBt8->state = GuiControlState::DISABLED;
			guiBt9->state = GuiControlState::DISABLED;
			activationPauseMenu = false;
			Engine::GetInstance().map.get()->mapLevel = M_NO_LEVEL;
			state = MENU;
			preparationTimer = 1.0f;
			activationPreparationTimer = false;
			Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/MenuMusic.wav", 0.0f);

		}


		if (player->dead) {

			if (preparationTimer <= 0.0f) {


				Engine::GetInstance().map.get()->mapLevel = M_NO_LEVEL;
				state = LOSE;
				preparationTimer = 1.0f;
				activationPreparationTimer = false;
				Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/LoseMusic.ogg", 0.0f);
			}
		}

		break;

	case WIN:

		Engine::GetInstance().textures->GetSize(winScreen, width, height);
		dstRect = { 0, 0, width, height };
		SDL_RenderCopy(Engine::GetInstance().render->renderer, winScreen, nullptr, &dstRect);

		if (activationPreparationTimer) {

			preparationTimer -= deltaTime;

		}

		if (preparationTimer <= 0.0f) {

			player->life = 16;
			player->ammo = 0;
			player->lookRight = true;
			scorePoints = 0;
			secondsTimer = 0;
			minutesTimer = 0;
			player->Respawn();
			Engine::GetInstance().map.get()->mapLevel = M_LEVEL1;
			state = LEVEL1;
			preparationTimer = 1.0f;
			activationPreparationTimer = false;
			Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/Level1Music.wav", 0.0f);

		}

		break;
	case LOSE:

		Engine::GetInstance().textures->GetSize(loseScreen, width, height);
		dstRect = { 0, 0, width, height };
		SDL_RenderCopy(Engine::GetInstance().render->renderer, loseScreen, nullptr, &dstRect);

		if (activationPreparationTimer) {

			preparationTimer -= deltaTime;

		}

		if (preparationTimer <= 0.0f) {

			player->lookRight = true;
			player->dead = false;
			LoadState();

			state = lastState;

			if (state == LEVEL1) {
				Engine::GetInstance().map.get()->mapLevel = M_LEVEL1;
				Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/Level1Music.wav", 0.0f);
			}

			preparationTimer = 1.0f;
			activationPreparationTimer = false;

		}

		break;
	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	// F1 to respawn at the beginning of the first level
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		Engine::GetInstance().scene.get()->player->Respawn();
	
	// F3 to respawn at the beginning of the current level
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		Engine::GetInstance().scene.get()->player->Respawn();

	// ESC or finished the level to close the window
	if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)) {
		
		if (state == MENU) {
			
			if (activationCredits) activationCredits = false;
			else if (activationSettings) activationSettings = false;
		}
		else if (state == LEVEL1 || state == LEVEL2) {

			if (!activationPauseMenu && !activationSettings) activationPauseMenu = true;
			else if (activationPauseMenu && !activationSettings) activationPauseMenu = false;
			else if (activationPauseMenu && activationSettings) activationSettings = false;
		}

	}


	if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_P) == KEY_DOWN) || exit)
		ret = false;

	// F5 to Save
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		SaveState();

	// F6 to Load
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		LoadState();

	// F7 to teleport to next checkponit
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
		Engine::GetInstance().scene.get()->player->MoveToNextCheckpoint();

	// F8 to show GUI debug
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) {
		auto& guiControlsList = Engine::GetInstance().guiManager.get()->guiControlsList;

		for (auto it = guiControlsList.begin(); it != guiControlsList.end(); ++it) {
			if (*it) {
				(*it)->debugMode = !(*it)->debugMode; 
			}
		}
	}

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN && (state == WIN || state == LOSE)) {
		activationPreparationTimer = true;
	}

	
	if (state == INTRO) {
		activationPreparationTimer = true;
	}



	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");


	Engine::GetInstance().textures->UnLoad(helpMenu);


	return true;
}

// Return the player position
Vector2D Scene::GetPlayerPosition()
{
	return player->GetPosition();
}

void Scene::LoadState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	//Read XML and restore information

	//Player position
	Vector2D playerPos = Vector2D(sceneNode.child("entities").child("player").attribute("x").as_int(),
		sceneNode.child("entities").child("player").attribute("y").as_int());
	player->SetPosition(playerPos);
	player->ammo = (sceneNode.child("entities").child("player").attribute("ammo").as_int());
	player->life = (sceneNode.child("entities").child("player").attribute("life").as_int());

	// Game
	scorePoints = (sceneNode.child("currentGame").attribute("score").as_int());
	minutesTimer = (sceneNode.child("currentGame").attribute("minute").as_int());
	secondsTimer = (sceneNode.child("currentGame").attribute("second").as_int());
	stateSaved = (sceneNode.child("currentGame").attribute("gameSaved").as_bool());

	//enemies



}

void Scene::SaveState() {

	if (state == LEVEL1) {
		stateSaved = true;
	}
	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	//Save info to XML 

	//Player position
	sceneNode.child("entities").child("player").attribute("x").set_value(player->GetPosition().getX());
	sceneNode.child("entities").child("player").attribute("y").set_value(player->GetPosition().getY() - 10);
	sceneNode.child("entities").child("player").attribute("ammo").set_value(player->ammo);
	sceneNode.child("entities").child("player").attribute("life").set_value(player->life);

	// Game
	sceneNode.child("currentGame").attribute("score").set_value(scorePoints);
	sceneNode.child("currentGame").attribute("minute").set_value(minutesTimer);
	sceneNode.child("currentGame").attribute("second").set_value(secondsTimer);
	sceneNode.child("currentGame").attribute("gameSaved").set_value(stateSaved);
	sceneNode.child("entities").child("player").attribute("ammo").set_value(player->ammo);


	//Saves the modifications to the XML 
	loadFile.save_file("config.xml");
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	switch (control->id) {
	case 1:

		activationPreparationTimer = true;
		break;

	case 2:

		activationPreparationTimer = true;
		LoadState();
		break;

	case 3:
		
		activationSettings = true;
		break;

	case 4:
		
		activationCredits = true;
		break;

	case 5:

		exit = true;
		break;

	case 6:

		activationPauseMenu = false;
		break;

	case 7:

		activationSettings = true;
		break;

	case 8:

		activationPreparationTimer = true;
		break;

	case 9:

		exit = true;
		break;

	case 10:
		GuiControlCheckBox* checkBox = dynamic_cast<GuiControlCheckBox*>(control);
		checkBox->activateCheckBox = !checkBox->activateCheckBox;
		Engine::GetInstance().window.get()->ToggleFullscreen();

		break;

	}

	return true;
}

// Help Menu
void Scene::HelpMenu() {

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN) {
		help = !help;

	}
	if (help) {

		int width, height;
		Engine::GetInstance().textures->GetSize(helpMenu, width, height);
		SDL_Rect dstRect = { 0, 0, width, height };

		SDL_RenderCopy(Engine::GetInstance().render->renderer, helpMenu, nullptr, &dstRect);


	}
}

// Camera movement with limitations
void Scene::HandleCamera() {

	//L03 TODO 3: Make the camera movement independent of framerate
	float camSpeed = 1;

	if (Engine::GetInstance().map.get()->WorldToMap(player->position.getX(), player->position.getY()).getX() < 20) {

		Engine::GetInstance().render.get()->camera.x = 0;

	}
	else if (Engine::GetInstance().map.get()->WorldToMap(player->position.getX(), player->position.getY()).getX() >= 20
		&& Engine::GetInstance().map.get()->WorldToMap(player->position.getX(), player->position.getY()).getX() <= 179) {

		Engine::GetInstance().render.get()->camera.x = (Engine::GetInstance().window->width / 2 - player->position.getX());

	}
}

void Scene::LevelTimer() {

	char timer[30];

	 secondsTimer += deltaTime;
	
	if (secondsTimer >= 60) {
		secondsTimer = 0;
		minutesTimer++;
	}

	// "MM:SS"
	sprintf_s(timer, "TIME: %02i:%02i", minutesTimer, (int)secondsTimer);

	Engine::GetInstance().render.get()->DrawText(timer, 1000, 20, strlen(timer) * 20, 40);


}

void Scene::Score() {

	char score[20];

	sprintf_s(score, "SCORE: %i", scorePoints);

	Engine::GetInstance().render.get()->DrawText(score, 600, 20, strlen(score) * 20, 40);

}

