
#include "Engine.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Log.h"
#include "Physics.h"
#include "Window.h"
#include "tracy/Tracy.hpp"
#include <string>

#include <math.h>



Map::Map(bool startEnabled) : Module(startEnabled), mapLoaded(false)
{
    name = "map";
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake()
{
    name = "map";
    LOG("Loading Map Parser");

    return true;
}

bool Map::Start() {

    return true;
}

bool Map::Update(float dt)
{
    ZoneScoped;
    bool ret = true;

    for (const auto& paralax : mapData.paralaxs) {
        //Check if the property Draw exist get the value, if it's true draw the layer
        if (paralax->loaded)
        {
            int cameraX = Engine::GetInstance().render.get()->camera.x / Engine::GetInstance().window.get()->GetScale();
            int cameraY = Engine::GetInstance().render.get()->camera.y / Engine::GetInstance().window.get()->GetScale();


            for (int numRep = 0; numRep < paralax->repeatNum; numRep++)
            {
                if (numRep % 2 != 0) Engine::GetInstance().render->DrawTexture(paralax->texture, -cameraX / paralax->slowX + paralax->marginX + paralax->width * (numRep), -cameraY / paralax->slowY + paralax->marginY);
                else Engine::GetInstance().render->DrawTextureFlipped(paralax->texture, -cameraX / paralax->slowX + paralax->marginX + paralax->width * (numRep), -cameraY / paralax->slowY + paralax->marginY);
            }
        }
    }


    if (mapLoaded) {

        // L07 TODO 5: Prepare the loop to draw all tiles in a layer + DrawTexture()
        // iterate all tiles in a layer
        for (const auto& mapLayer : mapData.layers) {
            //Check if the property Draw exist get the value, if it's true draw the lawyer
            if (mapLayer->properties.GetProperty("Draw") != NULL && mapLayer->properties.GetProperty("Draw")->value == true) {
                for (int i = 0; i < mapData.width; i++) {
                    for (int j = 0; j < mapData.height; j++) {

                        // L07 TODO 9: Complete the draw function

                        Vector2D mapInWorld = MapToWorld(i,j);
                        if (Engine::GetInstance().render.get()->InCameraView(mapInWorld.getX(), mapInWorld.getY(), mapData.tileWidth, mapData.tileHeight))
                        {
                            //Get the gid from tile
                            int gid = mapLayer->Get(i, j);
                            //Check if the gid is different from 0 - some tiles are empty
                            if (gid != 0) {
                                //L09: TODO 3: Obtain the tile set using GetTilesetFromTileId
                                TileSet* tileSet = GetTilesetFromTileId(gid);
                                if (tileSet != nullptr) {
                                    //Get the Rect from the tileSetTexture;
                                    SDL_Rect tileRect = tileSet->GetRect(gid);
                                    //Get the screen coordinates from the tile coordinates
                                        Vector2D mapCoord = MapToWorld(i, j);
                                    //Draw the texture
                                    Engine::GetInstance().render->DrawTexture(tileSet->texture, mapCoord.getX(), mapCoord.getY(), &tileRect);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}

// L09: TODO 2: Implement function to the Tileset based on a tile id
TileSet* Map::GetTilesetFromTileId(int gid) const
{
    TileSet* set = nullptr;

    for (const auto& tileset : mapData.tilesets) {
        if (gid >= tileset->firstGid && gid < (tileset->firstGid + tileset->tileCount)) {
            set = tileset;
            break;
        }
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    // L06: TODO 2: Make sure you clean up any memory allocated from tilesets/map
    for (const auto& tileset : mapData.tilesets) {
        delete tileset;
    }
    mapData.tilesets.clear();

    for (const auto& layer : mapData.layers)
    {
        delete layer;
    }
    mapData.layers.clear();

    for (const auto& objectGroup : mapData.objectsGroups) {
        for (const auto& object : objectGroup->object) {
            delete object;  
        }
        objectGroup->object.clear();  
        delete objectGroup;  
    }
    mapData.objectsGroups.clear();

    for (const auto& paralax : mapData.paralaxs)
    {
        delete paralax;
    }
    mapData.paralaxs.clear();


    return true;
}

// Load new map
bool Map::Load(std::string path, std::string fileName)
{
    bool ret = false;

    // Assigns the name of the map file and the path
    mapFileName = fileName;
    mapPath = path;
    std::string mapPathName = path + fileName;


    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapPathName.c_str());

    bossFightColliders.clear();

    if (result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapPathName.c_str(), result.description());
        ret = false;
    }
    else {

        // L06: TODO 3: Implement LoadMap to load the map properties
        // retrieve the paremeters of the <map> node and store the into the mapData struct
        mapData.width = mapFileXML.child("map").attribute("width").as_int();
        mapData.height = mapFileXML.child("map").attribute("height").as_int();
        mapData.tileWidth = mapFileXML.child("map").attribute("tilewidth").as_int();
        mapData.tileHeight = mapFileXML.child("map").attribute("tileheight").as_int();
        mapData.orientation = ORTOGRAPHIC;

        // L06: TODO 4: Implement the LoadTileSet function to load the tileset properties
       


        //Iterate the Tileset
        for (pugi::xml_node tilesetNode = mapFileXML.child("map").child("tileset"); tilesetNode != NULL; tilesetNode = tilesetNode.next_sibling("tileset"))
        {
            //Load Tileset attributes
            TileSet* tileSet = new TileSet();
            tileSet->firstGid = tilesetNode.attribute("firstgid").as_int();
            tileSet->name = tilesetNode.attribute("name").as_string();
            tileSet->tileWidth = tilesetNode.attribute("tilewidth").as_int();
            tileSet->tileHeight = tilesetNode.attribute("tileheight").as_int();
            tileSet->spacing = tilesetNode.attribute("spacing").as_int();
            tileSet->margin = tilesetNode.attribute("margin").as_int();
            tileSet->tileCount = tilesetNode.attribute("tilecount").as_int();
            tileSet->columns = tilesetNode.attribute("columns").as_int();

            //Load the tileset image
            std::string imgName = tilesetNode.child("image").attribute("source").as_string();
            tileSet->texture = Engine::GetInstance().textures->Load((mapPath + imgName).c_str());

            mapData.tilesets.push_back(tileSet);
        }

        // L07: TODO 3: Iterate all layers in the TMX and load each of them
        for (pugi::xml_node layerNode = mapFileXML.child("map").child("layer"); layerNode != NULL; layerNode = layerNode.next_sibling("layer")) {

            // L07: TODO 4: Implement the load of a single layer 
            //Load the attributes and saved in a new MapLayer
            MapLayer* mapLayer = new MapLayer();
            mapLayer->id = layerNode.attribute("id").as_int();
            mapLayer->name = layerNode.attribute("name").as_string();
            mapLayer->width = layerNode.attribute("width").as_int();
            mapLayer->height = layerNode.attribute("height").as_int();

            //L09: TODO 6 Call Load Layer Properties
            LoadProperties(layerNode, mapLayer->properties);

            //Iterate over all the tiles and assign the values in the data array
            for (pugi::xml_node tileNode = layerNode.child("data").child("tile"); tileNode != NULL; tileNode = tileNode.next_sibling("tile")) {
                mapLayer->tiles.push_back(tileNode.attribute("gid").as_int());
            }

            //add the layer to the map
            mapData.layers.push_back(mapLayer);
        }

        // L08 TODO 3: Create colliders
        // L08 TODO 7: Assign collider type
        // Later you can create a function here to load and create the colliders from the map
        for (pugi::xml_node objectGroupNode = mapFileXML.child("map").child("objectgroup"); objectGroupNode != NULL; objectGroupNode = objectGroupNode.next_sibling("objectgroup")) {

            ObjectGroup* objectGroup = new ObjectGroup();
            objectGroup->id = objectGroupNode.attribute("id").as_int();
            objectGroup->name = objectGroupNode.attribute("name").as_string();
            for (pugi::xml_node objectNode = objectGroupNode.child("object"); objectNode != NULL; objectNode = objectNode.next_sibling("object")) {
                Object* object = new Object();
                object->id = objectNode.attribute("id").as_int();
                object->x = objectNode.attribute("x").as_int();
                object->y = objectNode.attribute("y").as_int();
                object->width = objectNode.attribute("width").as_int();
                object->height = objectNode.attribute("height").as_int();
                objectGroup->object.push_back(object);
            }
            mapData.objectsGroups.push_back(objectGroup);
        }

        for (ObjectGroup* objectGroup : mapData.objectsGroups)
        {
            if (objectGroup->name == "Collisions")
            {
                for (Object* object : objectGroup->object)
                {
                    PhysBody* c = Engine::GetInstance().physics.get()->CreateRectangle(object->x + object->width / 2, object->y + object->height / 2, object->width, object->height, STATIC);
                    c->ctype = ColliderType::PLATFORM;
                }
            }
            if (objectGroup->name == "Spikes")
            {
                for (Object* object : objectGroup->object)
                {
                    PhysBody* c = Engine::GetInstance().physics.get()->CreateRectangle(object->x + object->width / 2, object->y + object->height / 2, object->width, object->height, STATIC);
                    c->ctype = ColliderType::SPYKE;
                }
            }
            if (objectGroup->name == "Abyss")
            {
                for (Object* object : objectGroup->object)
                {
                    PhysBody* c = Engine::GetInstance().physics.get()->CreateRectangle(object->x + object->width / 2, object->y + object->height / 2, object->width, object->height, STATIC);
                    c->ctype = ColliderType::ABYSS;
                }
            }
            if (objectGroup->name == "MapLimits")
            {
                for (Object* object : objectGroup->object)
                {
                    PhysBody* c = Engine::GetInstance().physics.get()->CreateRectangle(object->x + object->width / 2, object->y + object->height / 2, object->width, object->height, STATIC);
                    c->ctype = ColliderType::MAPLIMITS;
                }
            }
            if (objectGroup->name == "Ladder") {
                for (Object* object : objectGroup->object)
                {
                    PhysBody* c = Engine::GetInstance().physics.get()->CreateRectangleSensor(object->x + object->width / 2, object->y + object->height / 2, object->width, object->height, STATIC);

                    c->ctype = ColliderType::LADDER;
                }
            }
            if (objectGroup->name == "Checkpoints") {
                for (Object* object : objectGroup->object) {

                    PhysBody* c = Engine::GetInstance().physics.get()->CreateRectangleSensor(object->x + object->width / 2, object->y + object->height / 2, object->width, object->height, STATIC);

                    c->ctype = ColliderType::CHECKPOINT;
                }
            }
            if (objectGroup->name == "Items") {
                for (Object* object : objectGroup->object) {

                    PhysBody* c = Engine::GetInstance().physics.get()->CreateRectangleSensor(object->x + object->width / 2, object->y + object->height / 2, object->width, object->height, STATIC);

                    c->ctype =ColliderType::PUMPKIN;
                }
            }
            if (objectGroup->name == "LevelChanger") {
                for (Object* object : objectGroup->object) {

                    PhysBody* c = Engine::GetInstance().physics.get()->CreateRectangleSensor(object->x + object->width / 2, object->y + object->height / 2, object->width, object->height, STATIC);

                    c->ctype = ColliderType::LEVEL_CHANGER;
                }
            }
            if (objectGroup->name == "BossStarter") {
                for (Object* object : objectGroup->object) {

                    PhysBody* c = Engine::GetInstance().physics.get()->CreateRectangleSensor(object->x + object->width / 2, object->y + object->height / 2, object->width, object->height, STATIC);

                    c->ctype = ColliderType::BOSS_STARTER;
                }
            }
            if (objectGroup->name == "BossFightColliders") {
                for (Object* object : objectGroup->object) {

                    PhysBody* c = Engine::GetInstance().physics.get()->CreateRectangle(object->x + object->width / 2, object->y + object->height / 2, object->width, object->height, STATIC);

                    c->ctype = ColliderType::MAPLIMITS;
                    bossFightColliders.push_back(c);
                }
            }
        }



        ret = true;

        // L06: TODO 5: LOG all the data loaded iterate all tilesetsand LOG everything
        if (ret == true)
        {
            LOG("Successfully parsed map XML file :%s", fileName.c_str());
            LOG("width : %d height : %d", mapData.width, mapData.height);
            LOG("tile_width : %d tile_height : %d", mapData.tileWidth, mapData.tileHeight);

            LOG("Tilesets----");

            //iterate the tilesets
            for (const auto& tileset : mapData.tilesets) {
                LOG("name : %s firstgid : %d", tileset->name.c_str(), tileset->firstGid);
                LOG("tile width : %d tile height : %d", tileset->tileWidth, tileset->tileHeight);
                LOG("spacing : %d margin : %d", tileset->spacing, tileset->margin);
            }

            LOG("Layers----");

            for (const auto& layer : mapData.layers) {
                LOG("id : %d name : %s", layer->id, layer->name.c_str());
                LOG("Layer width : %d Layer height : %d", layer->width, layer->height);
            }
        }
        else {
            LOG("Error while parsing map file: %s", mapPathName.c_str());
        }

        if (mapFileXML) mapFileXML.reset();

    }

    mapLoaded = ret;
    return ret;
}

bool Map::LoadParalax(pugi::xml_node paralaxLayerNode)
{
    bool AllLoaded = true;
    for (pugi::xml_node layer = paralaxLayerNode.child("layer"); layer != NULL; layer = layer.next_sibling("layer")) {
        Paralax* paralax = new Paralax();
        paralax->loaded = true;
        SDL_Texture* texture = Engine::GetInstance().textures.get()->Load(layer.attribute("path").as_string());

        if (texture == nullptr)
        {
            LOG("Could not load paralax texture from path %s", layer.attribute("path").as_string());
            paralax->loaded = false;
            AllLoaded = false;
        }
        else
        {
            paralax->texture = texture;
            paralax->name = layer.attribute("name").as_string();

            paralax->marginX = layer.attribute("marginX").as_int();
            paralax->marginY = layer.attribute("marginY").as_int();
            paralax->spacing = layer.attribute("spacing").as_int();
            paralax->width = layer.attribute("width").as_int();
            paralax->height = layer.attribute("height").as_int();
            paralax->slowX = layer.attribute("slowX").as_int();
            paralax->slowY = layer.attribute("slowY").as_int();
            paralax->repeatNum = layer.attribute("repeatNum").as_int();
            mapData.paralaxs.push_front(paralax);
        }
    }

    return AllLoaded;
}

//void LoadParalaxLayers(std::list<Paralax*> paralaxLayers) {
//    for (Paralax* l : paralaxLayers) {
//        /*LoadParalax(mapParameters.child("parallax").child("layer").attribute("path").as_string(), ParalaxType : (mapParameters.child("parallax").child("layer").attribute("type").as_string));*/
//    }
//}

// L07: TODO 8: Create a method that translates x,y coordinates from map positions to world positions
Vector2D Map::MapToWorld(int x, int y) const
{
    Vector2D ret;

    ret.setX(x * mapData.tileWidth);
    ret.setY(y * mapData.tileHeight);

    return ret;
}

Vector2D Map::MapToWorldCentered(int x, int y)
{
    Vector2D ret;

    ret.setX(x * mapData.tileWidth + mapData.tileWidth/2);
    ret.setY(y * mapData.tileHeight + mapData.tileWidth / 2);

    return ret;
}

Vector2D Map::WorldToWorldCenteredOnTile(int x, int y)
{
    Vector2D ret;

    int tileX = x / mapData.tileWidth;
    int tileY = y / mapData.tileHeight;

    ret.setX(tileX * mapData.tileWidth + mapData.tileWidth / 2);
    ret.setY(tileY * mapData.tileHeight + mapData.tileHeight / 2);

    return ret;
}



// L10: TODO 5: Add method WorldToMap to obtain  map coordinates from screen coordinates 
Vector2D Map::WorldToMap(int x, int y) {

    Vector2D ret(0, 0);

    if (mapData.orientation == MapOrientation::ORTOGRAPHIC) {
        ret.setX(x / mapData.tileWidth);
        ret.setY(y / mapData.tileHeight);
    }

    if (mapData.orientation == MapOrientation::ISOMETRIC) {
        float half_width = mapData.tileWidth / 2;
        float half_height = mapData.tileHeight / 2;
        ret.setX(int((x / half_width + y / half_height) / 2));
        ret.setY(int((y / half_height - (x / half_width)) / 2));
    }

    return ret;
}

// L09: TODO 6: Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.propertyList.push_back(p);
    }

    return ret;
}

MapLayer* Map::GetNavigationLayer() {
    for (const auto& layer : mapData.layers) {
        if (layer->properties.GetProperty("Navigation") != NULL &&
            layer->properties.GetProperty("Navigation")->value) {
            return layer;

        }
    }
    return nullptr;
}

// L09: TODO 7: Implement a method to get the value of a custom property
Properties::Property* Properties::GetProperty(const char* name)
{
    for (const auto& property : propertyList) {
        if (property->name == name) {
            return property;
        }
    }

    return nullptr;
}
