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
#include "Potion.h"
#include "Fire.h"
#include "Potion.h"
#include "GuiControl.h"
#include "GuiManager.h"
#include "tracy/Tracy.hpp"


Scene::Scene() : Module()
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
	
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	player->SetParameters(configParameters.child("entities").child("player"));

	slash = (Slash*)Engine::GetInstance().entityManager->CreateEntity(EntityType::SLASH);
	slash->SetParameters(configParameters.child("entities").child("slash"));

	bat1 = (Bat*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BAT);
	bat1->SetParameters(configParameters.child("entities").child("bat1"));

	bat2 = (Bat*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BAT);
	bat2->SetParameters(configParameters.child("entities").child("bat2"));

	golem1 = (Golem*)Engine::GetInstance().entityManager->CreateEntity(EntityType::GOLEM);
	golem1->SetParameters(configParameters.child("entities").child("golem1"));

	golem2 = (Golem*)Engine::GetInstance().entityManager->CreateEntity(EntityType::GOLEM);
	golem2->SetParameters(configParameters.child("entities").child("golem2"));

	boss = (Boss*)Engine::GetInstance().entityManager->CreateEntity(EntityType::BOSS);
	boss->SetParameters(configParameters.child("entities").child("boss"));
	
	Fire* fire1 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire1->SetParameters(configParameters.child("entities").child("fire"));
	fire1->position = Vector2D(192, 640);

	Fire* fire2 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire2->SetParameters(configParameters.child("entities").child("fire"));
	fire2->position = Vector2D(734, 352);

	Fire* fire3 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire3->SetParameters(configParameters.child("entities").child("fire"));
	fire3->position = Vector2D(1216, 576);
	
	Fire* fire4 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire4->SetParameters(configParameters.child("entities").child("fire"));
	fire4->position = Vector2D(896, 736);

	Fire* fire5 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire5->SetParameters(configParameters.child("entities").child("fire"));
	fire5->position = Vector2D(1312, 1216);

	Fire* fire6 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire6->SetParameters(configParameters.child("entities").child("fire"));
	fire6->position = Vector2D(704, 1248);

	Fire* fire7 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire7->SetParameters(configParameters.child("entities").child("fire"));
	fire7->position = Vector2D(800, 1056);

	Fire* fire8 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire8->SetParameters(configParameters.child("entities").child("fire"));
	fire8->position = Vector2D(384, 896);

	Fire* fire9 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire9->SetParameters(configParameters.child("entities").child("fire"));
	fire9->position = Vector2D(1536, 1312);

	Fire* fire10 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire10->SetParameters(configParameters.child("entities").child("fire"));
	fire10->position = Vector2D(2080, 1344);

	Fire* fire11 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire11->SetParameters(configParameters.child("entities").child("fire"));
	fire11->position = Vector2D(2272, 704);

	Fire* fire12 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire12->SetParameters(configParameters.child("entities").child("fire"));
	fire12->position = Vector2D(2528, 1088);

	Fire* fire13 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire13->SetParameters(configParameters.child("entities").child("fire"));
	fire13->position = Vector2D(2656, 1088);

	Fire* fire14 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire14->SetParameters(configParameters.child("entities").child("fire"));
	fire14->position = Vector2D(3328, 1184);

	Fire* fire15 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire15->SetParameters(configParameters.child("entities").child("fire"));
	fire15->position = Vector2D(3040, 928);

	Fire* fire16 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire16->SetParameters(configParameters.child("entities").child("fire"));
	fire16->position = Vector2D(3616, 928);

	Fire* fire17 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire17->SetParameters(configParameters.child("entities").child("fire"));
	fire17->position = Vector2D(2752, 928);

	Fire* fire18 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire18->SetParameters(configParameters.child("entities").child("fire"));
	fire18->position = Vector2D(2592, 864);

	Fire* fire19 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire19->SetParameters(configParameters.child("entities").child("fire"));
	fire19->position = Vector2D(2592, 1216);

	Fire* fire20 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire20->SetParameters(configParameters.child("entities").child("fire"));
	fire20->position = Vector2D(3232, 1024);

	Fire* fire21 = (Fire*)Engine::GetInstance().entityManager->CreateEntity(EntityType::FIRE);
	fire21->SetParameters(configParameters.child("entities").child("fire"));
	fire21->position = Vector2D(3360, 1024);

	Item* key1 = (Item*)Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
	key1->SetParameters(configParameters.child("entities").child("key"));

	Potion* potion = (Potion*)Engine::GetInstance().entityManager->CreateEntity(EntityType::POTION);
	potion->SetParameters(configParameters.child("entities").child("potion"));

	

	// L16: TODO 2: Instantiate a new GuiControlButton in the Scene
	//SDL_Rect btPos = { 520, 350, 120,20 };
	//guiBt = (GuiControlButton*)Engine::GetInstance().guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "MyButton", btPos, this);

	return ret;

}

// Called before the first frame
bool Scene::Start()
{

	Engine::GetInstance().map->Load(configParameters.child("map").attribute("path").as_string(), configParameters.child("map").attribute("name").as_string());

	Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/EnterHallownest.ogg", 0);

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
	ZoneScoped;
	float camSpeed = 0;

	Engine::GetInstance().render.get()->camera.x =- player->position.getX()*2+450;
	Engine::GetInstance().render.get()->camera.y =- player->position.getY()*2+350;

	if (Engine::GetInstance().render.get()->camera.x > 0) {
		Engine::GetInstance().render.get()->camera.x = 0;
	}
	if (Engine::GetInstance().render.get()->camera.y > 0) {
		Engine::GetInstance().render.get()->camera.y = 0;
	}
	if (Engine::GetInstance().render.get()->camera.y < -2160) {
		Engine::GetInstance().render.get()->camera.y = -2160;
	}

	if (player->position.getX() >= 5426 && player->position.getY() >= 655 && boss->dead == false) {
		Engine::GetInstance().audio.get()->PlayMusic("Assets/Audio/Music/Radiance.ogg", 0);
	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	ZoneScoped;
	bool ret = true;

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {

		LoadState();
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {

		SaveState();
	}

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	SDL_DestroyTexture(img);

	return true;
}

Vector2D Scene::GetPlayerPosition()
{
	return player->GetPosition();
}

void Scene::LoadState() {

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");
	Engine::GetInstance().audio.get()->PlayFx(player->loadGame);

	if (result == NULL) {
		LOG("Error loading config.xml: %s", result.description());
	}
	Vector2D pos;

	pos.setX(loadFile.child("config").child("scene").child("entities").child("player").attribute("x").as_int());
	pos.setY(loadFile.child("config").child("scene").child("entities").child("player").attribute("y").as_int());
	player->SetPosition(pos);

}
void Scene::SaveState() {
	pugi::xml_document saveFile;
	pugi::xml_parse_result result = saveFile.load_file("config.xml");
	Engine::GetInstance().audio.get()->PlayFx(player->saveGame);
	
	if (result == NULL) {
		LOG("Error....");
		return;
	}
	//Read the player pos and set the value in the XML
	Vector2D pos = player->GetPosition();
	saveFile.child("config").child("scene").child("entities").child("player").attribute("x").set_value(pos.getX());
	saveFile.child("config").child("scene").child("entities").child("player").attribute("y").set_value(pos.getY());
	//pos = bat1->GetPosition();
	//saveFile.child("config").child("scene").child("entities").child("bat1").attribute("x").set_value(pos.getX());
	//saveFile.child("config").child("scene").child("entities").child("bat1").attribute("y").set_value(pos.getY());
	//pos = bat2->GetPosition();
	//saveFile.child("config").child("scene").child("entities").child("bat2").attribute("x").set_value(pos.getX());
	//saveFile.child("config").child("scene").child("entities").child("bat2").attribute("y").set_value(pos.getY());
	//pos = golem1->GetPosition();
	//saveFile.child("config").child("scene").child("entities").child("golem1").attribute("x").set_value(pos.getX());
	//saveFile.child("config").child("scene").child("entities").child("golem1").attribute("y").set_value(pos.getY());
	//pos = golem2->GetPosition();
	//saveFile.child("config").child("scene").child("entities").child("golem2").attribute("x").set_value(pos.getX());
	//saveFile.child("config").child("scene").child("entities").child("golem2").attribute("y").set_value(pos.getY());
	pos = boss->GetPosition();
	saveFile.child("config").child("scene").child("entities").child("boss").attribute("x").set_value(pos.getX());
	saveFile.child("config").child("scene").child("entities").child("boss").attribute("y").set_value(pos.getY());


	//save the XML modification to disk
	saveFile.save_file("config.xml");
}

bool Scene::isAttacking() {
	return player->isAttacking;
}

bool Scene::isLookingRight() {
	return player->isLookingRight;
}

void Scene::takeDamagePlayer() {
	player->takeDamage();
}

void Scene::takePoints() {
	player->points += 10;
}

void Scene::playerHeal() {
	player->lives++;
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{
	// L15: DONE 5: Implement the OnGuiMouseClickEvent method
	LOG("Press Gui Control: %d", control->id);

	return true;
}

void Scene::CreateCheckpoint(Vector2D pos) {
	Checkpoint* campfire = (Checkpoint*)Engine::GetInstance().entityManager->CreateEntity(EntityType::CHECKPOINT);
	campfire->SetParameters(configParameters.child("entities").child("campfire"));
	//Vector2D campPos = { pos.getX() / 0.02f, pos.getY() / 0.02f };
	campfire->position = pos;
	//checkpointList.push_back(&pos);
}

