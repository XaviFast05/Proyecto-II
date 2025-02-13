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
#include "Item.h"
#include "Enemy.h"
#include "Physics.h"
#include "CheckPointBF.h"
#include "GuiControl.h"
#include "GuiManager.h"
#include <functional>

Scene::Scene() : Module()
{
	name = "scene";
	bg = nullptr;
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	//L04: TODO 3b: Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	player->SetParameters(configParameters.child("entities").child("player"));

	pugi::xml_node checkpointbf = configParameters.child("entities").child("checkpointbf");
	checkpoint = (CheckPointBF*)Engine::GetInstance().entityManager->CreateEntity(EntityType::CHECKPOINTBF);
	checkpoint->SetParameters(checkpointbf);
	checkpoint2 = (CheckPointBF*)Engine::GetInstance().entityManager->CreateEntity(EntityType::CHECKPOINTBF);
	checkpoint2->SetParameters(checkpointbf);
	checkpoint3 = (CheckPointBF*)Engine::GetInstance().entityManager->CreateEntity(EntityType::CHECKPOINTBF);
	checkpoint3->SetParameters(checkpointbf);

	// L16: TODO 2: Instantiate a new GuiControlButton in the Scene
	int scale = Engine::GetInstance().window.get()->GetScale();

	introTimer.Start();

	//Create the buttons
	SDL_Rect btPos = { 75 , 150 , 50, 20 };
	SDL_Rect btPos2 = { 75 , 175 , 50, 20 };
	SDL_Rect btPos3 = { 75 , 200 , 50, 20 };
	SDL_Rect btPos4 = { 75 , 225 , 50, 20 };
	SDL_Rect btPos5 = { 75 , 250 , 50, 20 };
	SDL_Rect btPos6 = { 125 , 175 , 100, 20 };
	SDL_Rect btPos7 = { 125 , 200 , 100, 20 };
	SDL_Rect btPos8 = { 75 , 225 , 50, 20 };
	SDL_Rect btPos9 = { 125 , 150 , 100, 20 };
	SDL_Rect btPos10 = { 65 , 225 , 70, 20 };

	guiBt = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "PLAY", btPos, this);
	guiContinue = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "CONTINUE", btPos2, this);
	guiConfig = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "CONFIG", btPos3, this);
	guiCredits = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "CREDITS", btPos4, this);
	guiExit = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "EXIT", btPos5, this);
	guiMusicSlider = (GuiControlSlider*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::SLIDER, 6, "Music Volume", btPos6, this, { 0, 128, 128 });
	guiFxSlider = (GuiControlSlider*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::SLIDER, 7, "FX Volume", btPos7, this, { 0, 128, 128 });
	guiBack = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 8, "BACK", btPos8, this);
	guiCheckScreen = (GuiControlCheck*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 9, "Fullscreen", btPos9, this);
	guiResume = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 10, "RESUME", btPos2, this);
	guiBackToTitle = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 11, "BACK TO TITLE", btPos10, this);


	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	Engine::GetInstance().audio->active = true;
	CTtexture = Engine::GetInstance().textures->Load("Assets/Textures/CONTROLS.png");
	menuBg = Engine::GetInstance().textures->Load("Assets/Textures/Menu/menuBg.png");
	configBg = Engine::GetInstance().textures->Load("Assets/Textures/Menu/configBg.png");
	title = Engine::GetInstance().textures->Load("Assets/Textures/Menu/Title.png");
	endingImg = Engine::GetInstance().textures->Load("Assets/Textures/Menu/ending.png");
	endingBg = Engine::GetInstance().textures->Load("Assets/Textures/Menu/endingBg.png");
	credits = Engine::GetInstance().textures->Load("Assets/Textures/Menu/credits.png");
	fastTravel = Engine::GetInstance().textures.get()->Load("Assets/Textures/Menu/fastTravel.png");
	die = Engine::GetInstance().textures.get()->Load("Assets/Textures/Menu/died.png");
	introBg = Engine::GetInstance().textures.get()->Load("Assets/Textures/Menu/introBg.png");

	// Initialize UI textures
	lifeIcon = Engine::GetInstance().textures.get()->Load("Assets/Textures/player/item3.png");
	coinIcon = Engine::GetInstance().textures.get()->Load("Assets/Textures/player/item1.png");

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	changeLevel(sceneNode.child("entities").child("player").attribute("level").as_int(),
		sceneNode.child("entities").child("enemies").child("enemy").attribute("saved").as_bool());
	
	player->coins = sceneNode.child("entities").child("player").attribute("coins").as_int();
	player->lifes = sceneNode.child("entities").child("player").attribute("lifes").as_int();

	//enemies
	if (enemyList.size() > 0)
	{
		Vector2D enemyPos = Vector2D(sceneNode.child("entities").child("enemies").child("enemy").attribute("x").as_int(),
			sceneNode.child("entities").child("enemies").child("enemy").attribute("y").as_int());

		enemyList[0]->SetPosition(enemyPos);
	}

	takenItems.clear();

	checkpoint->CheckTaken = sceneNode.child("entities").child("checkpointbf").attribute("taken1").as_bool();
	checkpoint2->CheckTaken = sceneNode.child("entities").child("checkpointbf").attribute("taken2").as_bool();
	checkpoint3->CheckTaken = sceneNode.child("entities").child("checkpointbf").attribute("taken3").as_bool();

	// Create FX
	coinFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Items/plink.wav");
	oneUpId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Items/1up.wav");
	pauseFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/Menu/menu_open.wav");

	configMenu = true;

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	ButtonManager();

	volumeMusic = guiMusicSlider->GetVolume();
	Engine::GetInstance().audio->MusicVolume(volumeMusic);
	volumeFx = guiFxSlider->GetVolume();
	Engine::GetInstance().audio->FxVolume(volumeFx);
	if (active)
	{	
		// Set music
		if (playerInvincible == true)
		{
			if (x == 0)
			{
				Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/menu_loop.wav", 0);
				finalBossMusic = false;
				y = 0;
				menuMusic = false;
				lvl6_11Music = false;
				x = 1;
			}
		}

		else if (player->currentLevel <= 5 && player->currentLevel >= 1)
		{
			if (y == 0)
			{
				Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/lvl_1-5_music.wav", 0);
				x = 0;
				menuMusic = false;
				lvl6_11Music = false;
				finalBossMusic = false;
				y = 1;
			}

		}
		else if (player->currentLevel <= 11 && player->currentLevel >= 6)
		{
			if (lvl6_11Music == false)
			{
				lvl6_11MS = Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/lvl_6-11_music.wav", 0);
				finalBossMusic = false;
				x = 0;
				y = 0;
				menuMusic = false;
				lvl6_11Music = true;
			}
		}
		else if (player->currentLevel == 12)
		{
			if (finalBossMusic == false)
			{
				Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/final_boss.wav", 0);
				x = 0;
				y = 0;
				menuMusic = false;
				lvl6_11Music = false;
				finalBossMusic = true;
			}
		}

		//L03 TODO 3: Make the camera movement independent of framerate
		float camSpeed = 1;

		Engine::GetInstance().render.get()->DrawTexture(bg, 0, 0);

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN and player->currentLevel < player->maxLevel)
		{
			changeLevel(player->currentLevel + 1, true);
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN and player->currentLevel > 1)
		{
			changeLevel(player->currentLevel - 1, false);
		}

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
		{
			CTVisible = !CTVisible;
		}

		if ((enemyList.size() > 0 && enemyList[0]->position.getY() < -10) || (enemyList.size() > 0 && enemyList[0]->position.getY() > 340))
		{
			Engine::GetInstance().entityManager->DestroyEntity(enemyList[0]);
			enemyList.clear();
		}

		if (enemyList.size() > 0 && enemyList[0]->isDead == true)
		{
			Engine::GetInstance().entityManager->DestroyEntity(enemyList[0]);
			enemyList.clear();
		}

		if (enemyList.size() > 0 && player->checkDeath == true && player->isDead == false)
		{
			if (player->GetPosition().getY() + 7 > enemyList[0]->GetPosition().getY() && !player->invincible)
			{
				pugi::xml_document loadFile;
				pugi::xml_parse_result result = loadFile.load_file("config.xml");


				player->Die();
				player->checkDeath = false;

				player->loseLife();
				loadFile.child("config").child("scene").child("entities").child("player").attribute("lifes").set_value(player->lifes);
				loadFile.save_file("config.xml");

				if (player->lifes == 0)
				{
					player->isDead = false;

					player->isFalling = false;
					player->isJumping = false;
					player->pbody->body->SetLinearVelocity(b2Vec2_zero);

					Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/die.wav", 0);

					FadeInOut(Engine::GetInstance().render->renderer, 3000, true);

					player->currentAnimation = &player->idle;
					SpawnPoint();
					playTime.Start();
					initialSeconds = 0;
					SaveState();
					checkpoint->CheckTaken = false;
					dead = true;
					loadFile.child("config").child("scene").child("entities").child("player").attribute("played").set_value(false);
					loadFile.save_file("config.xml");
					Engine::GetInstance().entityManager->active = false;
					Engine::GetInstance().map->active = false;
					Engine::GetInstance().scene->active = false;
				}
			}
			else
			{
				bool killEnemy = false;
				if (enemyList[0]->isBoss == false)
				{
					player->JumpFX();
					if (enemyList[0]->isGrounded == true)
					{
						player->KillGroundedFX();
					}
					else if (enemyList[0]->isGrounded == false) player->KillFX();

					player->checkDeath = false;
					killEnemy = true;
				}
				if (!player->invincible)
				{
					player->pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -0.6), true);
				}
				if (enemyList[0]->isBoss == true)
				{
					player->JumpFX();

					enemyList[0]->lifes -= 1;
					if (enemyList[0]->lifes == 0)
					{
						Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/ending.wav", 0);
						enemyList[0]->unaliveHutao();

						FadeInOut(Engine::GetInstance().render->renderer, 2000, true);

						Engine::GetInstance().entityManager->active = false;
						Engine::GetInstance().map->active = false;
						Engine::GetInstance().scene->active = false;
						ending = true;
						FadeInOut(Engine::GetInstance().render->renderer, 1000, false);
					}
					else
					{
						enemyList[0]->HurtHutao();
					}
					player->pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -1.2), true);

					player->checkDeath = false;
				}
				if (enemyList[0]->lifes == 0)
				{
					player->JumpFX();
					if (enemyList[0]->isBoss == false)
					{
						if (enemyList[0]->isGrounded == true)
						{
							player->KillGroundedFX();
						}
						else if (enemyList[0]->isGrounded == false) player->KillFX();
					}

					player->checkDeath = false;
					killEnemy = true;
				}
				if (killEnemy)
				{
					Engine::GetInstance().entityManager->DestroyEntity(enemyList[0]);
					enemyList.clear();
				}
			}
		}
		
		if (itemList.size() > 0 && player->takeItem == true)
		{

			switch (itemList[0]->type)
			{
			case 1:
				// Coins
				player->addCoins(5);
				Engine::GetInstance().audio->PlayFx(coinFxId);
				break;
			case 2:
				// Invencibility
				playerInvencibility.Start();
				playerInvincible = true;
				break;
			case 3:
				// Life
				Engine::GetInstance().audio->PlayFx(oneUpId);
				player->lifes++;
				break;
			default:
				break;
			}

			Engine::GetInstance().entityManager->DestroyEntity(itemList[0]);
			itemList.clear();

			takenItems.push_back(player->currentLevel);
			player->takeItem = false;
		}

		if (playerInvincible)
		{
			if (playerInvencibility.ReadSec() < 5)
			{
				player->invincible = true;
			}
			else
			{
				player->invincible = false;
				playerInvincible = false;
			}
		}

		if (checkpoint->Saving == true)
		{
			SaveState();
			checkpoint->Saving = false;
		}
		if (checkpoint2->Saving == true)
		{
			SaveState();
			checkpoint2->Saving = false;
		}
		if (checkpoint3->Saving == true)
		{
			SaveState();
			checkpoint3->Saving = false;
		}

		if (player->Loading == true)
		{
			LoadState();
			player->Loading = false;
		}

		if (configMenu == true)
		{
			Engine::GetInstance().render.get()->DrawTexture(configBg, 9, 130);
		}
		else if (player->paused == true)
		{
			Engine::GetInstance().render.get()->DrawTexture(menuBg, 57, 160);
		}
	}
	else if (!active)
	{
		if (ending == false && dead == false)
		{
			if (isIntro == true)
			{
				Engine::GetInstance().render.get()->DrawTexture(introBg, 0, 0);

				if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) or introTimer.ReadSec() > 2)
				{
					FadeInOut(Engine::GetInstance().render->renderer, 2000, true);
					isIntro = false;
				}
			}
			else if (isIntro == false)
			{
				Engine::GetInstance().render.get()->DrawTexture(title, 0, 0);
				if (configMenu == true)
				{
					Engine::GetInstance().render.get()->DrawTexture(configBg, 9, 130);
				}
				else if (ifCredits == true)
				{
					Engine::GetInstance().render.get()->DrawTexture(credits, 0, 0);
				}
				else
				{
					Engine::GetInstance().render.get()->DrawTexture(menuBg, 57, 147);
				}
				if (menuMusic == false)
				{
					Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/menu_intro.wav", 0);
					x = 0;
					y = 0;
					lvl6_11Music = false;
					finalBossMusic = false;
					menuMusic = true;
				}
			}
		}
		else if (ending == true)
		{
			Engine::GetInstance().render.get()->DrawTexture(endingImg, 0, 0);
			Engine::GetInstance().render.get()->DrawTexture(endingBg, 50, 210);
		}
		else if (dead == true)
		{
			if (dieMusic == false)
			{
				Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/died.wav", 0);
				dieMusic = true;
			}
			Engine::GetInstance().render.get()->DrawTexture(die, 0, 0);
		}
	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if (fixFade == false)
	{
		configMenu = false;
		fixFade = true;
	}

	if (exitGame == true)
	{
		ret = false;
	}
	if (active)
	{
		// Activate or deactivate debug mode
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		{
			if (physics->debug == false)
			{
				for (int i = 0; i < enemyList.size(); i++)
				{
					enemyList[i]->DrawingPath = !enemyList[i]->DrawingPath;

				}
			}
		}

		if (counting)
		{
			DrawUI();
		}
		if ((Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN))
		{
			player->paused = !player->paused;
			for (int i = 0; i < enemyList.size(); i++)
			{
				enemyList[i]->paused = !enemyList[i]->paused;
			}

			if (configMenu == true)
			{
				configMenu = false;
			}

			Engine::GetInstance().audio->PlayFx(pauseFxId);
		}

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		{
			player->isDead = false;
			LoadState();
		}

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		{
			player->isDead = false;

			player->isFalling = false;
			player->isJumping = false;
			player->pbody->body->SetLinearVelocity(b2Vec2_zero);

			SpawnPoint();
			SaveState();
			checkpoint->CheckTaken = false;
			checkpoint2->CheckTaken = false;
			checkpoint3->CheckTaken = false;
		}

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		{
			player->isDead = false;

			player->isFalling = false;
			player->isJumping = false;
			player->pbody->body->SetLinearVelocity(b2Vec2_zero);

			SpawnPointLvl2();
			SaveState();
			checkpoint->CheckTaken = false;
			checkpoint2->CheckTaken = false;
			checkpoint3->CheckTaken = false;
		}

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN && player->isDead == false)
		{
			SaveState();
		}

		if (checkpoint->onPlayer == true || checkpoint2->onPlayer == true || checkpoint3->onPlayer == true)
		{
			Engine::GetInstance().render.get()->DrawTexture(fastTravel, 0, 0, nullptr);
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) == KEY_DOWN and checkpoint->onPlayer == true)
		{
			
			Vector2D newPos = Vector2D(0, 0);
			if (checkpoint2->CheckTaken)
			{
				newPos = Vector2D(332, 309);
				player->SetPosition(newPos);
				changeLevel(7, true);
			}
			else if (checkpoint3->CheckTaken)
			{
				newPos = Vector2D(435, 309);
				player->SetPosition(newPos);
				changeLevel(11, true);
			}
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) == KEY_DOWN and checkpoint2->onPlayer == true)
		{
			Vector2D newPos = Vector2D(0, 0);
			if (checkpoint3->CheckTaken)
			{
				newPos = Vector2D(435, 309);
				player->SetPosition(newPos);
				changeLevel(11, true);
			}
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN and checkpoint2->onPlayer == true)
		{
			Vector2D newPos = Vector2D(0, 0);
			if (checkpoint->CheckTaken)
			{
				newPos = Vector2D(51, 207);
				player->SetPosition(newPos);
				changeLevel(4, true);
			}
		}
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN and checkpoint3->onPlayer == true)
		{
			Vector2D newPos = Vector2D(0, 0);
			if (checkpoint2->CheckTaken)
			{
				newPos = Vector2D(332, 309);
				player->SetPosition(newPos);
				changeLevel(7, true);
			}
			else if (checkpoint->CheckTaken)
			{
				newPos = Vector2D(51, 207);
				player->SetPosition(newPos);
				changeLevel(4, true);
			}
		}

		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN && player->isDead == false)
		{
			if (enemyList.size() > 0)
			{
				changeLevel(player->currentLevel, true);
			}
			else
			{
				changeLevel(player->currentLevel, false);
			}

			Vector2D newPos = Vector2D(0, 0);

			player->isFalling = false;
			player->isJumping = false;
			player->pbody->body->SetLinearVelocity(b2Vec2_zero);

			switch (player->currentLevel)
			{
			case 1:
				newPos = Vector2D(251, 329);
				break;
			case 2:
				newPos = Vector2D(331, 269);
				break;
			case 3:
				newPos = Vector2D(221, 289);
				break;
			case 4:
				newPos = Vector2D(134, 309);
				break;
			case 5:
				newPos = Vector2D(134, 289);
				break;
			case 6:
				newPos = Vector2D(260, 309);
				break;
			case 7:
				newPos = Vector2D(270, 309);
				break;
			case 8:
				newPos = Vector2D(323, 269);
				break;
			case 9:
				newPos = Vector2D(241, 309);
				break;
			case 10:
				newPos = Vector2D(229, 329);
				break;
			case 11:
				newPos = Vector2D(434, 309);
				break;
			case 12:
				newPos = Vector2D(240, 329);
				break;
			default:
				break;
			}

			player->SetPosition(newPos);
		}

		Vector2D newCheck = Vector2D(-50, 0);
		checkpoint->SetPosition(newCheck);
		checkpoint2->SetPosition(newCheck);
		checkpoint3->SetPosition(newCheck);

		switch (player->currentLevel)
		{
		case 4:
			newCheck = Vector2D(51, 207);
			checkpoint->SetPosition(newCheck);
			break;
		case 7:
			newCheck = Vector2D(332, 309);
			checkpoint2->SetPosition(newCheck);
			break;
		case 11:
			newCheck = Vector2D(435, 309);
			checkpoint3->SetPosition(newCheck);
		default:
			break;
		}

		if (CTVisible && CTtexture != nullptr)
		{
			int WT, HG;
			Engine::GetInstance().textures->GetSize(CTtexture, WT, HG);
			int windowWT, windowHG;
			Engine::GetInstance().window->GetWindowSize(windowWT, windowHG);

			SDL_Rect Juan = { windowWT - WT - 100, 0, WT * 1.5, HG * 1.5f };
			SDL_RenderCopy(Engine::GetInstance().render->renderer, CTtexture, nullptr, &Juan);
		}
	}
	else if (!active)
	{
		if (ending == true)
		{
			DrawUI();
		}
	}

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	if (CTtexture != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(CTtexture);
		CTtexture = nullptr;
	}
	if (menuBg != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(menuBg);
		menuBg = nullptr;
	}
	if (configBg != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(configBg);
		configBg = nullptr;
	}
	if (title != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(title);
		title = nullptr;
	}
	if (lifeIcon != nullptr)
	{
		Engine::GetInstance().textures.get()->UnLoad(lifeIcon);
		lifeIcon = nullptr;
	}
	if (coinIcon != nullptr)
	{
		Engine::GetInstance().textures.get()->UnLoad(coinIcon);
		coinIcon = nullptr;
	}
	if (endingImg != nullptr)
	{
		Engine::GetInstance().textures->UnLoad(endingImg);
		endingImg = nullptr;
	}

	return true;
}

// Return the player position
Vector2D Scene::GetPlayerPosition()
{
	return player->GetPosition();
}

// L15 TODO 1: Implement the Load function
void Scene::LoadState() {
	b2Vec2 Vdefault = b2Vec2(0, 0);

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
	player->pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, 0), true);
	takenItems.clear();

	changeLevel(sceneNode.child("entities").child("player").attribute("level").as_int(), 
		sceneNode.child("entities").child("enemies").child("enemy").attribute("saved").as_bool());

	player->coins = sceneNode.child("entities").child("player").attribute("coins").as_int();
	player->lifes = sceneNode.child("entities").child("player").attribute("lifes").as_int();

	checkpoint->CheckTaken = sceneNode.child("entities").child("checkpointbf").attribute("taken1").as_bool();
	checkpoint2->CheckTaken = sceneNode.child("entities").child("checkpointbf").attribute("taken2").as_bool();
	checkpoint3->CheckTaken = sceneNode.child("entities").child("checkpointbf").attribute("taken3").as_bool();

	player->invincible = false;
	playerInvincible = false;

	//enemies
	if (enemyList.size() > 0)
	{
		Vector2D enemyPos = Vector2D(sceneNode.child("entities").child("enemies").child("enemy").attribute("x").as_int(),
			sceneNode.child("entities").child("enemies").child("enemy").attribute("y").as_int());

		enemyList[0]->SetPosition(enemyPos);
	}

	player->pbody->body->SetLinearVelocity(Vdefault);
	player->splatted.Reset();
}

// L15 TODO 2: Implement the Save function
void Scene::SaveState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	sceneNode.child("entities").child("player").attribute("lifes").set_value(player->lifes);
	sceneNode.child("entities").child("player").attribute("coins").set_value(player->coins);

	if (canSave)
	{
		sceneNode.child("entities").child("player").attribute("played").set_value(true);
	}

	if (enemyList.size() > 0)
	{
		sceneNode.child("entities").child("enemies").child("enemy").attribute("x").set_value(enemyList[0]->GetPosition().getX());
		sceneNode.child("entities").child("enemies").child("enemy").attribute("y").set_value(enemyList[0]->GetPosition().getY());
		sceneNode.child("entities").child("enemies").child("enemy").attribute("saved") = true;
	}
	else
	{
		sceneNode.child("entities").child("enemies").child("enemy").attribute("saved") = false;
	}
	
	if (takenItems.size() > 0)
	{
		for (int i = 0; i < takenItems.size(); i++)
		{
			std::string lvl = "lvl" + std::to_string(takenItems[i]);
			sceneNode.child("entities").child("player").child("items").attribute(lvl.c_str()) = true;
		}
	}

	//Save info to XML 

	//Player position
	sceneNode.child("entities").child("player").attribute("x").set_value(player->GetPosition().getX());
	sceneNode.child("entities").child("player").attribute("y").set_value(player->GetPosition().getY());
	sceneNode.child("entities").child("player").attribute("level").set_value(player->currentLevel);

	sceneNode.child("entities").child("checkpointbf").attribute("taken1").set_value(checkpoint->CheckTaken);
	sceneNode.child("entities").child("checkpointbf").attribute("taken2").set_value(checkpoint2->CheckTaken);
	sceneNode.child("entities").child("checkpointbf").attribute("taken3").set_value(checkpoint3->CheckTaken);

	//Saves the modifications to the XML 
	loadFile.save_file("config.xml");
}

void Scene::SpawnPoint()
{
	Vector2D playerPos = Vector2D(251, 329);
	player->SetPosition(playerPos);

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	sceneNode.child("entities").child("player").attribute("level") = 1;
	sceneNode.child("entities").child("player").attribute("lifes") = 3;
	sceneNode.child("entities").child("player").attribute("coins") = 0;
	player->resetCoins();
	player->lifes = 3;

	player->invincible = false;
	playerInvincible = false;

	sceneNode.child("entities").child("player").attribute("played").set_value(false);
	sceneNode.child("entities").child("player").attribute("seconds").set_value(0);

	sceneNode.child("entities").child("player").child("items").attribute("lvl1").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl2").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl3").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl4").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl5").set_value(false);
	takenItems.clear();

	sceneNode.child("entities").child("checkpointbf").attribute("taken1").set_value(false);
	sceneNode.child("entities").child("checkpointbf").attribute("taken2").set_value(false);
	sceneNode.child("entities").child("checkpointbf").attribute("taken3").set_value(false);

	checkpoint->CheckTaken = false;
	checkpoint2->CheckTaken = false;
	checkpoint3->CheckTaken = false;
	loadFile.save_file("config.xml");

	changeLevel(1, true);
}

void Scene::SpawnPointLvl2()
{
	Vector2D playerPos = Vector2D(321, 279);
	player->SetPosition(playerPos);

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	pugi::xml_node sceneNode = loadFile.child("config").child("scene");

	sceneNode.child("entities").child("player").attribute("level").set_value(2);
	sceneNode.child("entities").child("player").attribute("lifes").set_value(3);
	sceneNode.child("entities").child("player").attribute("coins").set_value(0);

	player->invincible = false;
	playerInvincible = false;

	sceneNode.child("entities").child("player").child("items").attribute("lvl1").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl2").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl3").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl4").set_value(false);
	sceneNode.child("entities").child("player").child("items").attribute("lvl5").set_value(false);
	takenItems.clear();

	sceneNode.child("entities").child("checkpointbf").attribute("taken1").set_value(false);
	sceneNode.child("entities").child("checkpointbf").attribute("taken2").set_value(false);
	sceneNode.child("entities").child("checkpointbf").attribute("taken3").set_value(false);

	checkpoint->CheckTaken = false;
	checkpoint2->CheckTaken = false;
	checkpoint3->CheckTaken = false;

	loadFile.save_file("config.xml");
	changeLevel(2, true);
}

void Scene::changeLevel(int level, bool upordown)
{
	player->currentLevel = level;

	int windowW, windowH;
	Engine::GetInstance().window.get()->GetWindowSize(windowW, windowH);

	int texW, texH;
	std::string fileName = "Assets/Textures/fondo" + std::to_string(level) + ".png";

	bg = Engine::GetInstance().textures.get()->Load(fileName.c_str());
	Engine::GetInstance().textures.get()->GetSize(bg, texW, texH);

 	Engine::GetInstance().map->Load("Assets/Maps/", "Tilemap.tmx", level);

	pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy");
	pugi::xml_node itemNode = configParameters.child("entities").child("player").child("items");
	if (enemyList.size() > 0)
	{
		Engine::GetInstance().entityManager->DestroyEntity(enemyList[0]);
		enemyList.clear();
	}
	if (itemList.size() > 0)
	{
		Engine::GetInstance().entityManager->DestroyEntity(itemList[0]);
		itemList.clear();
	}
	
	bool canSpawnItem = true;
	if (takenItems.size() > 0)
	{
     	for (int i = 0; i < takenItems.size(); i++)
		{
			if (level == takenItems[i])
			{
				canSpawnItem = false;
			}
		}
	}
	if (level != player->maxLevel)
	{
		switch (level)
		{
		case 1:
			itemNode.attribute("x") = 240;
			itemNode.attribute("y") = 30;
			itemNode.attribute("type") = 1;
			break;
		case 2:
			itemNode.attribute("x") = 424;
			itemNode.attribute("y") = 155;
			itemNode.attribute("type") = 2;
			break;
		case 3:
			itemNode.attribute("x") = 309;
			itemNode.attribute("y") = 135;
			itemNode.attribute("type") = 3;
			break;
		case 4:
			itemNode.attribute("x") = 436;
			itemNode.attribute("y") = 95;
			itemNode.attribute("type") = 1;
			break;
		case 5:
			itemNode.attribute("x") = 45;
			itemNode.attribute("y") = 215;
			itemNode.attribute("type") = 1;
			break;
		case 6:
			itemNode.attribute("x") = 165;
			itemNode.attribute("y") = 163;
			itemNode.attribute("type") = 3;
			break;
		case 7:
			itemNode.attribute("x") = 193;
			itemNode.attribute("y") = 159;
			itemNode.attribute("type") = 2;
			break;
		case 8:
			itemNode.attribute("x") = 294;
			itemNode.attribute("y") = 139;
			itemNode.attribute("type") = 1;
			break;
		case 9:
			itemNode.attribute("x") = 152;
			itemNode.attribute("y") = 139;
			itemNode.attribute("type") = 2;
			break;
		case 10:
			itemNode.attribute("x") = 111;
			itemNode.attribute("y") = 279;
			itemNode.attribute("type") = 3;
			break;
		case 11:
			itemNode.attribute("x") = 443;
			itemNode.attribute("y") = 139;
			itemNode.attribute("type") = 1;
			break;
		default:
			break;
		}

		pugi::xml_document loadFile;
		pugi::xml_parse_result result = loadFile.load_file("config.xml");

		std::string lvl = "lvl" + std::to_string(level);
		if (loadFile.child("config").child("scene").child("entities").child("player").child("items").attribute(lvl.c_str()).as_bool() == false)
		{
			if (canSpawnItem == true)
			{
				Item* item = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
				item->SetParameters(itemNode);
				itemList.push_back(item);
			}
		}
	}
	
	if (upordown && level != 1 && level != player->maxLevel)
	{
		switch (level)
		{
		case 2:
			enemyNode.attribute("x") = 50;
			enemyNode.attribute("y") = 50;
			enemyNode.attribute("gravity") = true;
			grounded = true;
			break;
		case 3:
			enemyNode.attribute("x") = 50;
			enemyNode.attribute("y") = 50;
			enemyNode.attribute("gravity") = false;
			grounded = false;
			break;
		case 4:
			enemyNode.attribute("x") = 380;
			enemyNode.attribute("y") = 50;
			enemyNode.attribute("gravity") = false;
			grounded = false;
			break;
		case 5:
			enemyNode.attribute("x") = 160;
			enemyNode.attribute("y") = 50;
			enemyNode.attribute("gravity") = true;
			grounded = true;
			break;
		case 6:
			enemyNode.attribute("x") = 211;
			enemyNode.attribute("y") = 89;
			enemyNode.attribute("gravity") = true;
			grounded = false;
			break;
		case 7:
			enemyNode.attribute("x") = 92;
			enemyNode.attribute("y") = 149;
			enemyNode.attribute("gravity") = false;
			grounded = false;
			break;
		case 8:
			enemyNode.attribute("x") = 109;
			enemyNode.attribute("y") = 89;
			enemyNode.attribute("gravity") = true;
			grounded = true;
			break;
		case 9:
			enemyNode.attribute("x") = 441;
			enemyNode.attribute("y") = 109;
			enemyNode.attribute("gravity") = false;
			grounded = false;
			break;
		case 10:
			enemyNode.attribute("x") = 392;
			enemyNode.attribute("y") = 89;
			enemyNode.attribute("gravity") = true;
			grounded = true;
			break;
		case 11:
			enemyNode.attribute("x") = 292;
			enemyNode.attribute("y") = 69;
			enemyNode.attribute("gravity") = false;
			grounded = false;
			break;
		default:
			break;
		}

		Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
		enemy->SetParameters(enemyNode);
		enemyList.push_back(enemy);

		enemy->ChangeGrounded(grounded);
	}

	if (level == player->maxLevel)
	{
		enemyNode.attribute("x") = 160;
		enemyNode.attribute("y") = 50;
		enemyNode.attribute("gravity") = true;
		Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);

		enemy->isBoss = true;
		enemy->SetParameters(enemyNode);
		enemyList.push_back(enemy);
	}
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");
	pugi::xml_node sceneNode = loadFile.child("config").child("window");
	loadFile.child("fullscreen").attribute("value") = true;
	loadFile.save_file("config.xml");
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);
	if (control->id == 1)
	{
		Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/opening_theme.wav", 0);


		guiBt->showBounds = false;
		guiConfig->showBounds = false;
		guiCredits->showBounds = false;
		guiExit->showBounds = false;
		guiContinue->showBounds = false;
		guiMusicSlider->showBounds = false;
		guiFxSlider->showBounds = false;
		guiCheckScreen->showBounds = false;
		guiBackToTitle->showBounds = false;
		guiResume->showBounds = false;
		guiBack->showBounds = false;

		FadeInOut(Engine::GetInstance().render->renderer, 2000, true);

		SpawnPoint();
		canSave = false;
		SaveState();
		canSave = true;
		playTime.Start();
		initialSeconds = 0;
		counting = true;

		player->paused = false;
		for (int i = 0; i < enemyList.size(); i++)
		{
				enemyList[i]->paused = false;
		}
		Engine::GetInstance().entityManager->active = true;
		Engine::GetInstance().map->active = true;
		Engine::GetInstance().scene->active = true;

		player->currentAnimation = &player->idle;

		FadeInOut(Engine::GetInstance().render->renderer, 1500, false);
	}
	if (control->id == 2)
	{
		Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/opening_theme.wav", 0);


		guiBt->showBounds = false;
		guiConfig->showBounds = false;
		guiCredits->showBounds = false;
		guiExit->showBounds = false;
		guiContinue->showBounds = false;
		guiMusicSlider->showBounds = false;
		guiFxSlider->showBounds = false;
		guiCheckScreen->showBounds = false;
		guiBackToTitle->showBounds = false;
		guiResume->showBounds = false;
		guiBack->showBounds = false;

		FadeInOut(Engine::GetInstance().render->renderer, 2000, true);
		LoadState();

		pugi::xml_document configFile;
		pugi::xml_parse_result result = configFile.load_file("config.xml");

		initialSeconds = configFile.child("config").child("scene").child("entities").child("player").attribute("seconds").as_int();
		playTime.Start();
		counting = true;

		player->paused = false;
		for (int i = 0; i < enemyList.size(); i++)
		{
			enemyList[i]->paused = false;
		}
		Engine::GetInstance().entityManager->active = true;
		Engine::GetInstance().map->active = true;
		Engine::GetInstance().scene->active = true;
		player->currentAnimation = &player->idle;
		FadeInOut(Engine::GetInstance().render->renderer, 1500, false);

	}
	if (control->id == 3)
	{

		guiBt->showBounds = false;
		guiConfig->showBounds = false;
		guiCredits->showBounds = false;
		guiExit->showBounds = false;
		guiContinue->showBounds = false;
		guiMusicSlider->showBounds = false;
		guiFxSlider->showBounds = false;
		guiCheckScreen->showBounds = false;
		guiBackToTitle->showBounds = false;
		guiResume->showBounds = false;
		guiBack->showBounds = false;
		configMenu = true;
	}
	if (control->id == 4)
	{

		guiBt->showBounds = false;
		guiConfig->showBounds = false;
		guiCredits->showBounds = false;
		guiExit->showBounds = false;
		guiContinue->showBounds = false;
		guiMusicSlider->showBounds = false;
		guiFxSlider->showBounds = false;
		guiCheckScreen->showBounds = false;
		guiBackToTitle->showBounds = false;
		guiResume->showBounds = false;
		guiBack->showBounds = false;
		ifCredits = true;
	}
	if (control->id == 5)
	{
		if (counting)
		{
			pugi::xml_document configFile;
			pugi::xml_parse_result result = configFile.load_file("config.xml");

			int seconds = configFile.child("config").child("scene").child("entities").child("player").attribute("seconds").as_int();
			seconds += (int)playTime.ReadSec();
			configFile.child("config").child("scene").child("entities").child("player").attribute("seconds").set_value(seconds);

			configFile.save_file("config.xml");
		}
		FadeInOut(Engine::GetInstance().render->renderer, 2000, true);
		exitGame = true;
	}
	if (control->id == 8)
	{

		guiBt->showBounds = false;
		guiConfig->showBounds = false;
		guiCredits->showBounds = false;
		guiExit->showBounds = false;
		guiContinue->showBounds = false;
		guiMusicSlider->showBounds = false;
		guiFxSlider->showBounds = false;
		guiCheckScreen->showBounds = false;
		guiBackToTitle->showBounds = false;
		guiResume->showBounds = false;
		guiBack->showBounds = false;
		configMenu = false;
		ifCredits = false;
	}
	if (control->id == 9)
	{
		bool fullscreen = guiCheckScreen->isChecked();
		if (fullscreen == true)
		{
			// Load the config.xml file
			pugi::xml_document configFile;
			pugi::xml_parse_result result = configFile.load_file("config.xml");

			// Set the fullscreen attribute to true
			configFile.child("config").child("window").child("fullscreen").attribute("value").set_value(true);

			// Save the changes to the config.xml file
			configFile.save_file("config.xml");

			// Apply fullscreen mode
			Engine::GetInstance().window->ToggleFullscreen();
			
		}
		else
		{
			// Load the config.xml file
			pugi::xml_document configFile;
			pugi::xml_parse_result result = configFile.load_file("config.xml");

			// Set the fullscreen attribute to true
			configFile.child("config").child("window").child("fullscreen").attribute("value").set_value(false);

			// Save the changes to the config.xml file
			configFile.save_file("config.xml");
			Engine::GetInstance().window->UnToggleFullscreen();
		}
	}
	if (control->id == 10)
	{

		guiBt->showBounds = false;
		guiConfig->showBounds = false;
		guiCredits->showBounds = false;
		guiExit->showBounds = false;
		guiContinue->showBounds = false;
		guiMusicSlider->showBounds = false;
		guiFxSlider->showBounds = false;
		guiCheckScreen->showBounds = false;
		guiBackToTitle->showBounds = false;
		guiResume->showBounds = false;
		guiBack->showBounds = false;

		player->paused = false;
		for (int i = 0; i < enemyList.size(); i++)
		{
			enemyList[i]->paused = false;
		}
	}
	if (control->id == 11)
	{
		pugi::xml_document configFile;
		pugi::xml_parse_result result = configFile.load_file("config.xml");

		int seconds = configFile.child("config").child("scene").child("entities").child("player").attribute("seconds").as_int();
		seconds += (int)playTime.ReadSec();
		configFile.child("config").child("scene").child("entities").child("player").attribute("seconds").set_value(seconds);

		if (ending || dead)
		{
			configFile.child("config").child("scene").child("entities").child("player").attribute("played").set_value(false);
		}

		configFile.save_file("config.xml");
		counting = false;

		guiBt->showBounds = false;
		guiConfig->showBounds = false;
		guiCredits->showBounds = false;
		guiExit->showBounds = false;
		guiContinue->showBounds = false;
		guiMusicSlider->showBounds = false;
		guiFxSlider->showBounds = false;
		guiCheckScreen->showBounds = false;
		guiBackToTitle->showBounds = false;
		guiResume->showBounds = false;
		guiBack->showBounds = false;

		FadeInOut(Engine::GetInstance().render->renderer, 2000, true);
		Engine::GetInstance().entityManager->active = false;
		Engine::GetInstance().map->active = false;
		Engine::GetInstance().scene->active = false;
		ending = false;
		dead = false;
		dieMusic = false;
		FadeInOut(Engine::GetInstance().render->renderer, 2000, false);

	}
	return true;
}

void Scene::ButtonManager()
{
	pugi::xml_document configFile;
	pugi::xml_parse_result result = configFile.load_file("config.xml");
	if (active)
	{
		if (player->paused)
		{
			if (configMenu == false)
			{
				guiBt->state = GuiControlState::DISABLED;
				guiContinue->state = GuiControlState::DISABLED;
				guiConfig->state = GuiControlState::NORMAL;
				guiCredits->state = GuiControlState::DISABLED;
				guiExit->state = GuiControlState::NORMAL;
				guiMusicSlider->state = GuiControlState::DISABLED;
				guiFxSlider->state = GuiControlState::DISABLED;
				guiBack->state = GuiControlState::DISABLED;
				guiCheckScreen->state = GuiControlState::DISABLED;
				guiResume->state = GuiControlState::NORMAL;
				guiBackToTitle->state = GuiControlState::NORMAL;
			}
			else if (configMenu == true)
			{
				guiBt->state = GuiControlState::DISABLED;
				guiContinue->state = GuiControlState::DISABLED;
				guiConfig->state = GuiControlState::DISABLED;
				guiCredits->state = GuiControlState::DISABLED;
				guiExit->state = GuiControlState::DISABLED;
				guiMusicSlider->state = GuiControlState::NORMAL;
				guiFxSlider->state = GuiControlState::NORMAL;
				guiBack->state = GuiControlState::NORMAL;
				guiCheckScreen->state = GuiControlState::NORMAL;
				guiResume->state = GuiControlState::DISABLED;
				guiBackToTitle->state = GuiControlState::DISABLED;
			}
			
		}
		else
		{
			guiBt->state = GuiControlState::DISABLED;
			guiContinue->state = GuiControlState::DISABLED;
			guiConfig->state = GuiControlState::DISABLED;
			guiCredits->state = GuiControlState::DISABLED;
			guiExit->state = GuiControlState::DISABLED;
			guiMusicSlider->state = GuiControlState::DISABLED;
			guiFxSlider->state = GuiControlState::DISABLED;
			guiBack->state = GuiControlState::DISABLED;
			guiCheckScreen->state = GuiControlState::DISABLED;
			guiResume->state = GuiControlState::DISABLED;
			guiBackToTitle->state = GuiControlState::DISABLED;
		}
	}
	else if (!active)
	{
		if (ending == false && dead == false)
		{
			if (isIntro == true)
			{
				guiBt->state = GuiControlState::DISABLED;
				guiContinue->state = GuiControlState::DISABLED;
				guiConfig->state = GuiControlState::DISABLED;
				guiCredits->state = GuiControlState::DISABLED;
				guiExit->state = GuiControlState::DISABLED;
				guiMusicSlider->state = GuiControlState::DISABLED;
				guiFxSlider->state = GuiControlState::DISABLED;
				guiBack->state = GuiControlState::DISABLED;
				guiCheckScreen->state = GuiControlState::DISABLED;
				guiResume->state = GuiControlState::DISABLED;
				guiBackToTitle->state = GuiControlState::DISABLED;

				if (configMenu == true)
				{
					guiBt->state = GuiControlState::DISABLED;
					guiContinue->state = GuiControlState::DISABLED;
					guiConfig->state = GuiControlState::DISABLED;
					guiCredits->state = GuiControlState::DISABLED;
					guiExit->state = GuiControlState::DISABLED;
					guiMusicSlider->state = GuiControlState::NORMAL;
					guiFxSlider->state = GuiControlState::NORMAL;
					guiBack->state = GuiControlState::NORMAL;
					guiCheckScreen->state = GuiControlState::NORMAL;
					guiResume->state = GuiControlState::DISABLED;
					guiBackToTitle->state = GuiControlState::DISABLED;
				}
			}
			else if (isIntro == false)
			{
				if (configMenu == true)
				{
					guiBt->state = GuiControlState::DISABLED;
					guiContinue->state = GuiControlState::DISABLED;
					guiConfig->state = GuiControlState::DISABLED;
					guiCredits->state = GuiControlState::DISABLED;
					guiExit->state = GuiControlState::DISABLED;
					guiMusicSlider->state = GuiControlState::NORMAL;
					guiFxSlider->state = GuiControlState::NORMAL;
					guiBack->state = GuiControlState::NORMAL;
					guiCheckScreen->state = GuiControlState::NORMAL;
					guiResume->state = GuiControlState::DISABLED;
					guiBackToTitle->state = GuiControlState::DISABLED;
				}
				else if (ifCredits == true)
				{
					guiBt->state = GuiControlState::DISABLED;
					guiContinue->state = GuiControlState::DISABLED;
					guiConfig->state = GuiControlState::DISABLED;
					guiCredits->state = GuiControlState::DISABLED;
					guiExit->state = GuiControlState::DISABLED;
					guiMusicSlider->state = GuiControlState::DISABLED;
					guiFxSlider->state = GuiControlState::DISABLED;
					guiBack->state = GuiControlState::NORMAL;
					guiCheckScreen->state = GuiControlState::DISABLED;
					guiResume->state = GuiControlState::DISABLED;
					guiBackToTitle->state = GuiControlState::DISABLED;
				}
				else
				{
					guiBt->state = GuiControlState::NORMAL;
					if (configFile.child("config").child("scene").child("entities").child("player").attribute("played").as_bool())
					{
						guiContinue->state = GuiControlState::NORMAL;
					}
					else if (!configFile.child("config").child("scene").child("entities").child("player").attribute("played").as_bool())
					{
						guiContinue->state = GuiControlState::DEACTIVATED;
					}
					guiConfig->state = GuiControlState::NORMAL;
					guiCredits->state = GuiControlState::NORMAL;
					guiExit->state = GuiControlState::NORMAL;
					guiMusicSlider->state = GuiControlState::DISABLED;
					guiFxSlider->state = GuiControlState::DISABLED;
					guiBack->state = GuiControlState::DISABLED;
					guiCheckScreen->state = GuiControlState::DISABLED;
					guiResume->state = GuiControlState::DISABLED;
					guiBackToTitle->state = GuiControlState::DISABLED;
				}
			}
		}
		else if (ending == true)
		{
			guiBt->state = GuiControlState::DISABLED;
			guiContinue->state = GuiControlState::DISABLED;
			guiConfig->state = GuiControlState::DISABLED;
			guiCredits->state = GuiControlState::DISABLED;
			guiExit->state = GuiControlState::DISABLED;
			guiMusicSlider->state = GuiControlState::DISABLED;
			guiFxSlider->state = GuiControlState::DISABLED;
			guiBack->state = GuiControlState::DISABLED;
			guiCheckScreen->state = GuiControlState::DISABLED;
			guiResume->state = GuiControlState::DISABLED;
			guiBackToTitle->state = GuiControlState::NORMAL;
		}
		else if (dead == true)
		{
			guiBt->state = GuiControlState::DISABLED;
			guiContinue->state = GuiControlState::DISABLED;
			guiConfig->state = GuiControlState::DISABLED;
			guiCredits->state = GuiControlState::DISABLED;
			guiExit->state = GuiControlState::DISABLED;
			guiMusicSlider->state = GuiControlState::DISABLED;
			guiFxSlider->state = GuiControlState::DISABLED;
			guiBack->state = GuiControlState::DISABLED;
			guiCheckScreen->state = GuiControlState::DISABLED;
			guiResume->state = GuiControlState::DISABLED;
			guiBackToTitle->state = GuiControlState::NORMAL;
		}
	}
}

void Scene::FadeInOut(SDL_Renderer* renderer, int duration, bool fadeIn) {
	// Get time
	Uint32 startTime = SDL_GetTicks();

	while (SDL_GetTicks() - startTime < duration) {
		// Get transparent value
		float alpha = 0.f;
		if (fadeIn == true) 
		{
			alpha = (float)(SDL_GetTicks() - startTime) / duration;
		}
		else if (fadeIn == false)
		{
			alpha = 1.0f - (float)(SDL_GetTicks() - startTime) / duration;
		}

		// Get sure alpha is between 0 and 1
		if (alpha < 0.0f) alpha = 0.0f;
		if (alpha > 1.0f) alpha = 1.0f;

		// Set the the transparency to draw
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, (Uint8)(alpha * 255));

		// Get the whole screen
		SDL_Rect rect = { 0, 0, 960, 720 }; 
		SDL_RenderFillRect(renderer, &rect);

		// Update the screen
		SDL_RenderPresent(renderer);

		// Pause
		SDL_Delay(16); 
	}
}

void Scene::DrawUI()
{
	sceneSeconds = initialSeconds + (int)playTime.ReadSec();
	if (ending == false)
	{
		sceneFinalSeconds = sceneSeconds;
	}
	
	minutesOnScreen = (int)(sceneSeconds / 60);
	secondsOnScreen = (int)(sceneSeconds - 60 * minutesOnScreen);

	if (ending == false)
	{
		// Show time in "MM:SS"
		std::string minutesText = (minutesOnScreen < 10 ? "0" : "") + std::to_string(minutesOnScreen);
		std::string secondsText = (secondsOnScreen < 10 ? "0" : "") + std::to_string(secondsOnScreen);
		std::string timeText = "Time: " + minutesText + ":" + secondsText;

		Engine::GetInstance().render->DrawText(timeText.c_str(), 800, 20, 150, 40, white);
	}
	else if (ending == true)
	{
		int finalMinutesOnScreen = (int)(sceneFinalSeconds / 60);
		int finalSecondsOnScreen = (int)(sceneFinalSeconds - 60 * finalMinutesOnScreen);

		std::string finalMinutesText = (finalMinutesOnScreen < 10 ? "0" : "") + std::to_string(finalMinutesOnScreen);
		std::string finalSecondsText = (finalSecondsOnScreen < 10 ? "0" : "") + std::to_string(finalSecondsOnScreen);
		std::string timeText = "Time: " + finalMinutesText + ":" + finalSecondsText;

		
		Engine::GetInstance().render->DrawText(timeText.c_str(), 285, 450, 150, 40, white);
	}

	// Coins
	std::string coinsText = std::to_string(player->coins);

	// Lifes
	std::string lifesText = std::to_string(player->lifes);

	if (ending == false)
	{
		Engine::GetInstance().render.get()->DrawTexture(coinIcon, 0, 30, NULL, SDL_FLIP_NONE);
		Engine::GetInstance().render.get()->DrawText(coinsText.c_str(), 55, 75, 30, 50, white);

		Engine::GetInstance().render.get()->DrawTexture(lifeIcon, 0, 5, NULL, SDL_FLIP_NONE);
		Engine::GetInstance().render.get()->DrawText(lifesText.c_str(), 55, 15, 30, 50, white);
	}
	else if (ending == true)
	{
		Engine::GetInstance().render.get()->DrawTexture(coinIcon, 212, 215, NULL, SDL_FLIP_NONE);
		Engine::GetInstance().render.get()->DrawText(coinsText.c_str(), 478, 445, 30, 50, white);
	}
}