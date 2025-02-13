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

Scene::Scene() : Module(), bgMusic(0), checkP(nullptr), player(nullptr), areControlsVisible(false), hasReachedCheckpoint(false), controls(nullptr)
{
    name = "scene";
    img = nullptr;
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
    LOG("Loading Scene");
    bool ret = true;

    Engine::GetInstance().map->CleanUp();

    //L04: TODO 3b: Instantiate the player using the entity manager
    player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
    player->SetParameters(configParameters.child("entities").child("player"));

    // Crear un item
    for (pugi::xml_node itemNode = configParameters.child("entities").child("items").child("pickups").child("coin"); itemNode; itemNode = itemNode.next_sibling("coin"))
    {
        Item* coin = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
        coin->SetParameters(itemNode);
        itemList.push_back(coin);
    }
    for (pugi::xml_node itemNode = configParameters.child("entities").child("items").child("healths").child("health"); itemNode; itemNode = itemNode.next_sibling("health"))
    {
        Item* health = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::HEALTH);
        health->SetParameters(itemNode);
        itemList.push_back(health);
    }
    for (pugi::xml_node powerNode = configParameters.child("entities").child("items").child("powerups").child("powerupjump"); powerNode; powerNode = powerNode.next_sibling("powerupjump"))
    {
        Item* powerup = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::POWERUPJUMP);
        powerup->SetParameters(powerNode);
        itemList.push_back(powerup);
    }
    for (pugi::xml_node powerNode = configParameters.child("entities").child("items").child("powerups").child("powerupspeed"); powerNode; powerNode = powerNode.next_sibling("powerupspeed"))
    {
        Item* powerup = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::POWERUPSPEED);
        powerup->SetParameters(powerNode);
        itemList.push_back(powerup);
    }

	// Crear un checkpoint
    for (pugi::xml_node checkNode = configParameters.child("entities").child("checkpoints").child("checkpoint"); checkNode; checkNode = checkNode.next_sibling("checkpoint"))
    {
        Checkpoint* checkpoint = (Checkpoint*)Engine::GetInstance().entityManager->CreateEntity(EntityType::CHECKPOINT);
        checkpoint->SetParameters(checkNode);
        checkPList.push_back(checkpoint);
    }

    // Crear una meta
    for (pugi::xml_node flagNode = configParameters.child("entities").child("finish").child("flag"); flagNode; flagNode = flagNode.next_sibling("flag"))
    {
        Flag* flag = (Flag*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FLAGPOLE);
        flag->SetParameters(flagNode);
        flagList.push_back(flag);
    }
   
	// Crear enemigos
    for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
    {
        Enemy* enemy = (Enemy*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMY);
        enemy->SetParameters(enemyNode);
        enemyList.push_back(enemy);
    }
    for (pugi::xml_node enemyNode = configParameters.child("entities").child("enemies").child("enemyFloor"); enemyNode; enemyNode = enemyNode.next_sibling("enemyFloor"))
    {
        EnemyFloor* enemyF = (EnemyFloor*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ENEMYFLOOR);
        enemyF->SetParameters(enemyNode);
        enemyFList.push_back(enemyF);
    }

    for (pugi::xml_node bossNode = configParameters.child("entities").child("bosses").child("boss"); bossNode; bossNode = bossNode.next_sibling("boss"))
    {
        Boss* boss = (Boss*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BOSS);
        boss->SetParameters(bossNode);
        bossList.push_back(boss);
    }

    return ret;
}

// Called before the first frame
bool Scene::Start()
{
    //L06 TODO 3: Call the function to load the map. 
    Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());
    controls = Engine::GetInstance().textures->Load("Assets/Textures/Help.png");

    gameOver = Engine::GetInstance().textures->Load("Assets/Textures/Screens/lossScreen.png");
	win = Engine::GetInstance().textures->Load("Assets/Textures/Screens/winScreen.png");
	title = Engine::GetInstance().textures->Load("Assets/Textures/Screens/titleScreen.png");
    credits = Engine::GetInstance().textures->Load("Assets/Textures/Screens/credits.png");

    pHealth3 = Engine::GetInstance().textures->Load("Assets/Textures/HUD/playerHealth3.png");
    pHealth2 = Engine::GetInstance().textures->Load("Assets/Textures/HUD/playerHealth2.png");
    pHealth1 = Engine::GetInstance().textures->Load("Assets/Textures/HUD/playerHealth1.png");
    pHealth0 = Engine::GetInstance().textures->Load("Assets/Textures/HUD/playerHealth0.png");

	bHealth4 = Engine::GetInstance().textures->Load("Assets/Textures/HUD/bossHealth4.png");
	bHealth3 = Engine::GetInstance().textures->Load("Assets/Textures/HUD/bossHealth3.png");
	bHealth2 = Engine::GetInstance().textures->Load("Assets/Textures/HUD/bossHealth2.png");
	bHealth1 = Engine::GetInstance().textures->Load("Assets/Textures/HUD/bossHealth1.png");
	bHealth0 = Engine::GetInstance().textures->Load("Assets/Textures/HUD/bossHealth0.png");

    Engine::GetInstance().entityManager->active = false;
    Engine::GetInstance().map->active = false;

    int musicVolume = 50;
    Mix_VolumeMusic(musicVolume);

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
	if (hasStarted == false)
	{
        Engine::GetInstance().entityManager->active = false;
        Engine::GetInstance().map->active = false;
        if (!hasShownCredits) {
            int width, height;
            Engine::GetInstance().textures->GetSize(credits, width, height);
            int windowWidth, windowHeight;
            Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);
            SDL_Rect dstRect = { 0, 0, width, height };
            SDL_RenderCopy(Engine::GetInstance().render->renderer, credits, nullptr, &dstRect);

            if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
                hasShownCredits = true;
            }
            if (!isPlayingMusic) {
                bgMusic = Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/menu.ogg", 0);
                isPlayingMusic = true;
            }
        }
        else {
            int width, height;
            Engine::GetInstance().textures->GetSize(title, width, height);
            int windowWidth, windowHeight;
            Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);
            SDL_Rect dstRect = { 0, 0, width, height };
            SDL_RenderCopy(Engine::GetInstance().render->renderer, title, nullptr, &dstRect);

            if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {
                Engine::GetInstance().entityManager->active = true;
                Engine::GetInstance().map->active = true;
                hasStarted = true;
                currentLevel = 1;
                Engine::GetInstance().render.get()->camera.x = 0;
                cameraNeedsUpdate = true;
                player->isFalling = false;
                player->SetPosition(Vector2D(175, 390));
                isPlayingMusic = false;
            }
            else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_Y) == KEY_DOWN) {
                Engine::GetInstance().entityManager->active = true;
                Engine::GetInstance().map->active = true;
                isPlayingMusic = false;
                hasStarted = true;
                LoadState();
            }
        }
	}
    
    if (hasStarted) {
        UpdatePlayerHUD();

        // Comprobar si el jugador está en la zona del jefe
        if (player->position.getX() <= 4500) {
            if (currentLevel != 1) {
                currentLevel = 1;
                isPlayingMusic = false; // Reiniciar la bandera de música
            }
        }
        else if (player->position.getX() >= 4500 && player->position.getX() <= 8435) {
            if (currentLevel != 2) {
                currentLevel = 2;
                isPlayingMusic = false; // Reiniciar la bandera de música
            }
        }
        else if (player->position.getX() >= 8435) {
            UpdateBossHUD();
            if (currentLevel != 3) {
                currentLevel = 3;
                isPlayingMusic = false; // Reiniciar la bandera de música
            }
        }

        // Control de la música según el nivel actual
        if (currentLevel == 1 && !isPlayingMusic) {
            bgMusic = Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/level1.ogg", 0);
            isPlayingMusic = true;
        }
        else if (currentLevel == 2 && !isPlayingMusic) {
            bgMusic = Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/level2.ogg", 0);
            isPlayingMusic = true;
        }
        else if (currentLevel == 3 && !isPlayingMusic) {
            bgMusic = Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/bossBattle.ogg", 0);
            isPlayingMusic = true;
        }


        if (currentLevel == 1) {
            if (player->position.getX() >= 525 && !player->isDead && player->position.getX() <= 3700) {
                Engine::GetInstance().render.get()->camera.x = 500 - player->position.getX();
            }
        }
        else if (currentLevel == 2 || currentLevel == 3) {
            if (player->position.getX() >= 5025 && !player->isDead && player->position.getX() <= 8835) {
                Engine::GetInstance().render.get()->camera.x = 500 - player->position.getX();
            }
        }

        if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F7) == KEY_DOWN) {
            currentCheckpoint += 1;
            if (currentCheckpoint > 3) {
                currentCheckpoint = 1;
            }
            switch (currentCheckpoint)
            {
            case 1:
                currentLevel = 1;
                player->SetPosition(Vector2D(1440, 300));
                player->isFalling = false;
                isPlayingMusic = false;
                break;
            case 2:
                currentLevel = 1;

                player->SetPosition(Vector2D(3640, 520));
                player->isFalling = false;
                isPlayingMusic = false;
                break;
            case 3:
                currentLevel = 2;
                player->SetPosition(Vector2D(7170, 550));
                player->isFalling = false;
                isPlayingMusic = false;
                break;
            default:
                break;
            }
        }

        if (cameraNeedsUpdate || player->hasToUpdateCam) {
            player->hasToUpdateCam = false;
            cameraNeedsUpdate = false;
            if (player->reachedCheckpoint && currentLevel == 1) {
                Engine::GetInstance().render.get()->camera.x = 2460;
            }
            else if (player->reachedCheckpoint && (currentLevel == 2 || currentLevel == 3)) {
                Engine::GetInstance().render.get()->camera.x = 7970;
            }

            else {
                Engine::GetInstance().render.get()->camera.x = 0;
            }

            LOG("Camera manually updated in Update to: %d", Engine::GetInstance().render.get()->camera.x);
        }

        if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {

            if (currentLevel = 1) {
                Engine::GetInstance().render.get()->camera.x = 0;
                player->isFalling = false;
                player->SetPosition(Vector2D(175, 395));
                isPlayingMusic = false;
            }
            else if (currentLevel = 2 || currentLevel == 3) {
                Engine::GetInstance().render.get()->camera.x = 4500;
                player->isFalling = false;
                player->SetPosition(Vector2D(5080, 350));
                isPlayingMusic = false;
            }
        }

        if (player->hasLost) {
            // Mostrar pantalla de Game Over
            for (Enemy* enemy : enemyList) {
                if (!enemy->isDying) {
                    enemy->SetActive(false);
                }
                break;

            }

            for (EnemyFloor* enemyF : enemyFList) {
                if (!enemyF->isDying) {
                    enemyF->SetActive(false);
                }
                break;
            }

            for (Boss* bossC : bossList) {
                if (!bossC->isDying) {
                    bossC->SetActive(false);
                }
                break;
            }
            Engine::GetInstance().entityManager->active = false;
            Engine::GetInstance().map->active = false;
            int width, height;
            Engine::GetInstance().textures->GetSize(gameOver, width, height);
            int windowWidth, windowHeight;
            Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);

            SDL_Rect dstRect = { 0, 0, width, height };
            SDL_RenderCopy(Engine::GetInstance().render->renderer, gameOver, nullptr, &dstRect);

            if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
                player->hasLost = false;
                player->lives = 3;
                LoadState();
                Engine::GetInstance().entityManager->active = true;
                Engine::GetInstance().map->active = true;
            }
        }

        // Mostrar el menú de controles
        if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN) {
            areControlsVisible = !areControlsVisible;
        }

        if (areControlsVisible && controls != nullptr) {
            int width, height;
            Engine::GetInstance().textures->GetSize(controls, width, height);
            int windowWidth, windowHeight;
            Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);

            SDL_Rect dstRect = { windowWidth - width - 10, 10, width, height };

            SDL_RenderCopy(Engine::GetInstance().render->renderer, controls, nullptr, &dstRect);
        }

        // Comprobar si el jugador ha llegado al checkpoint
        if (!hasReachedCheckpoint) {
            for (Checkpoint* checkpoint : checkPList) {
                if (checkpoint->hasSounded) {
                    SaveState();
                    hasReachedCheckpoint = true;
                    break; // Salir del bucle una vez que se ha alcanzado un punto de control
                }
                
            }
        }
        if (!hasReachedFlagpole) {
            for (Flag* flag : flagList) {
                if (flag->hasWon) {
                    Engine::GetInstance().render.get()->camera.x = 4500;
                    player->SetPosition(Vector2D(5025, 370));
                    SaveState();
                    hasReachedCheckpoint = false;
                    hasReachedFlagpole = true;
                    
                    break; // Salir del bucle una vez que se ha alcanzado un punto de control
                }
            }
        }

        if (hasReachedFlagpole) {
            if (currentLevel == 1) {
                currentLevel = 2;
                isPlayingMusic = false;
                hasReachedFlagpole = false;
            }
        }

        ActivateEnemies();
    }
   
    return true;
}


// Esto antes iba para todos los enemigos pero por algun motivo ahora solo funciona para el boss
void Scene::ActivateEnemies() {
    // Actualizar el estado de los enemigos según el nivel actual
    
    // Esto aqui funciona a veces y otras veces se crashea el juego... no entendemos por qué
    /*for (Enemy* enemy : enemyList) {
        if (!enemy->isDying) {
            if (enemy->GetLevel() != currentLevel) {
                enemy->SetActive(false);
            }
            else {
                enemy->SetActive(true);
            }
        }
        else {
            break;
        }
        break;

    }

    for (EnemyFloor* enemyF : enemyFList) {
        if (!enemyF->isDying) {
            if (enemyF->GetLevel() != currentLevel) {
                enemyF->SetActive(false);
            }
            else {
                enemyF->SetActive(true);
            }
        }
        else {
            break;
        }
        break;
    }*/

    for (Boss* bossC : bossList) {
        if (!bossC->isDying) {
            if (bossC->GetLevel() != currentLevel) {
                bossC->SetActive(false);
            }
            else {
                bossC->SetActive(true);
            }
        }
        else {
            break;
        }
        break;
    }
}

// Called each loop iteration
bool Scene::PostUpdate()
{
    bool ret = true;

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
        ret = false;
    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
        LoadState();

    if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
        SaveState();

    return ret;
}


// Called before quitting
bool Scene::CleanUp()
{
    LOG("Freeing scene");

    SDL_DestroyTexture(img);
    if (controls != nullptr) {
        Engine::GetInstance().textures->UnLoad(controls);
        controls = nullptr;
    }
    return true;
}

void Scene::UpdatePlayerHUD()
{
    if (player->lives >= 3) {
        player->lives = 3;
        int width, height;
        Engine::GetInstance().textures->GetSize(pHealth3, width, height);
        int windowWidth, windowHeight;
        Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);

        SDL_Rect dstRect = { 10, 10, width, height };

        SDL_RenderCopy(Engine::GetInstance().render->renderer, pHealth3, nullptr, &dstRect);
    }
    else if (player->lives == 2) {
        int width, height;
        Engine::GetInstance().textures->GetSize(pHealth2, width, height);
        int windowWidth, windowHeight;
        Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);

        SDL_Rect dstRect = { 10, 10, width, height };

        SDL_RenderCopy(Engine::GetInstance().render->renderer, pHealth2, nullptr, &dstRect);
    }
    else if (player->lives == 1) {
        int width, height;
        Engine::GetInstance().textures->GetSize(pHealth1, width, height);
        int windowWidth, windowHeight;
        Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);

        SDL_Rect dstRect = { 10, 10, width, height };

        SDL_RenderCopy(Engine::GetInstance().render->renderer, pHealth1, nullptr, &dstRect);
    }
    else if (player->lives == 0) {
        int width, height;
        Engine::GetInstance().textures->GetSize(pHealth0, width, height);
        int windowWidth, windowHeight;
        Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);

        SDL_Rect dstRect = { 10, 10, width, height };

        SDL_RenderCopy(Engine::GetInstance().render->renderer, pHealth0, nullptr, &dstRect);
    }
}

void Scene::UpdateBossHUD()
{
    for (Boss* bossC : bossList) {
        if (!bossC->isDying) {
            if (bossC->health <= 50 && bossC->health > 40) {
                int width, height;
                Engine::GetInstance().textures->GetSize(bHealth4, width, height);
                int windowWidth, windowHeight;
                Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);
                SDL_Rect dstRect = { windowWidth / 3 + width / 4, 10, width, height };
                SDL_RenderCopy(Engine::GetInstance().render->renderer, bHealth4, nullptr, &dstRect);
            }
            else if (bossC->health <= 40 && bossC->health > 25) {
                int width, height;
                Engine::GetInstance().textures->GetSize(bHealth3, width, height);
                int windowWidth, windowHeight;
                Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);
                SDL_Rect dstRect = { windowWidth / 3 + width / 4 , 10, width, height };
                SDL_RenderCopy(Engine::GetInstance().render->renderer, bHealth3, nullptr, &dstRect);
            }
            else if (bossC->health <= 25 && bossC->health > 10) {
                int width, height;
                Engine::GetInstance().textures->GetSize(bHealth2, width, height);
                int windowWidth, windowHeight;
                Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);
                SDL_Rect dstRect = { windowWidth / 3 + width / 4, 10, width, height };
                SDL_RenderCopy(Engine::GetInstance().render->renderer, bHealth2, nullptr, &dstRect);
            }
            else if (bossC->health <= 10 && bossC->health > 0) {
                int width, height;
                Engine::GetInstance().textures->GetSize(bHealth1, width, height);
                int windowWidth, windowHeight;
                Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);
                SDL_Rect dstRect = { windowWidth / 3 + width / 4, 10, width, height };
                SDL_RenderCopy(Engine::GetInstance().render->renderer, bHealth1, nullptr, &dstRect);
            }
            else if (bossC->health == 0) {
                int width, height;
                Engine::GetInstance().textures->GetSize(bHealth0, width, height);
                int windowWidth, windowHeight;
                Engine::GetInstance().window->GetWindowSize(windowWidth, windowHeight);
                SDL_Rect dstRect = { windowWidth / 3 + width / 4, 10, width, height };
                SDL_RenderCopy(Engine::GetInstance().render->renderer, bHealth0, nullptr, &dstRect);
            }
        }
        break;
    }
}

Vector2D Scene::GetPlayerPosition()
{
    return player->GetPosition();
}

// L15 TODO 1: Implement the Load function
void Scene::LoadState() {
    pugi::xml_document loadFile;
    pugi::xml_parse_result result = loadFile.load_file("config.xml");

    if (!result) {
        LOG("Could not load file. Pugi error: %s", result.description());
        return;
    }

    pugi::xml_node sceneNode = loadFile.child("config").child("scene");

    // Cargar la posición del jugador
    pugi::xml_node playerNode = sceneNode.child("entities").child("player");
    if (playerNode) {
        Vector2D playerPos(
            static_cast<float>(playerNode.attribute("x").as_int() - 32),
            static_cast<float>(playerNode.attribute("y").as_int() - 40)
        );
        cameraNeedsUpdate = true;

        player->SetPosition(playerPos);
    }

	// AQUI HEMOS INTENTADO IMPLEMENTAR EL LOAD PARA LOS ENEMIGOS. COMO TAL FUNCIONA PERO CUANDO EL ENEMIGO SE MUERE Y SE VUELVE A CARGAR, NO FUNCIONA Y EL CODIGO PETA.

    //// Cargar enemigos
    //int i = 0;
    //for (pugi::xml_node enemyNode = sceneNode.child("entities").child("enemies").child("enemy");
    //    enemyNode;
    //    enemyNode = enemyNode.next_sibling("enemy")) {
    //    float x = static_cast<float>(enemyNode.attribute("x").as_int() - 32);
    //    float y = static_cast<float>(enemyNode.attribute("y").as_int() - 32);
    //    bool isAlive = enemyNode.attribute("alive").as_bool();

    //    if (isAlive && i < enemyList.size()) { // Actualizar enemigo existente solo si está vivo
    //        enemyList[i]->SetPosition(Vector2D(x, y));
    //        enemyList[i]->SetAlive();
    //        i++;
    //    }
    //}

    //// Cargar enemigos en el suelo
    //i = 0;
    //for (pugi::xml_node enemyFNode = sceneNode.child("entities").child("enemies").child("enemyFloor");
    //    enemyFNode;
    //    enemyFNode = enemyFNode.next_sibling("enemyFloor")) {
    //    float x = static_cast<float>(enemyFNode.attribute("x").as_int() - 32);
    //    float y = static_cast<float>(enemyFNode.attribute("y").as_int() - 32);
    //    bool isAlive = enemyFNode.attribute("alive").as_bool();

    //    if (isAlive && i < enemyFList.size()) { // Actualizar enemigo existente solo si está vivo
    //        enemyFList[i]->SetPosition(Vector2D(x, y));
    //        enemyFList[i]->SetAlive();
    //        i++;
    //    }
    //}
}


void Scene::SaveState() {
    pugi::xml_document saveFile;
    pugi::xml_parse_result result = saveFile.load_file("config.xml");

    if (!result) {
        LOG("Could not load file. Pugi error: %s", result.description());
        return;
    }

    pugi::xml_node sceneNode = saveFile.child("config").child("scene");

    // Guardar la posición del jugador
    pugi::xml_node playerNode = sceneNode.child("entities").child("player");
    if (playerNode) {
        playerNode.attribute("x").set_value(player->GetPosition().getX());
        playerNode.attribute("y").set_value(player->GetPosition().getY());
    }

    // AQUI HEMOS INTENTADO IMPLEMENTAR EL SAVE PARA LOS ENEMIGOS. COMO TAL FUNCIONA PERO COMO EL LOAD NO FUNCIONABA PUES AL FINAL LO HEMOS DESCARTADO.

    //// Guardar enemigos
    //pugi::xml_node enemiesNode = sceneNode.child("entities").child("enemies");
    //for (Enemy* enemy : enemyList) {
    //    pugi::xml_node enemyNode = enemiesNode.append_child("enemy");
    //    enemyNode.append_attribute("x").set_value(enemy->GetPosition().getX());
    //    enemyNode.append_attribute("y").set_value(enemy->GetPosition().getY());
    //    enemyNode.append_attribute("alive").set_value(enemy->isAlive());
    //}

    //// Guardar enemigos en el suelo
    //for (EnemyFloor* enemyF : enemyFList) {
    //    pugi::xml_node enemyFNode = enemiesNode.append_child("enemyFloor");
    //    enemyFNode.append_attribute("x").set_value(enemyF->GetPosition().getX());
    //    enemyFNode.append_attribute("y").set_value(enemyF->GetPosition().getY());
    //    enemyFNode.append_attribute("alive").set_value(enemyF->isAlive());
    //}

    // Guardar las modificaciones en el archivo XML
    saveFile.save_file("config.xml");
}
