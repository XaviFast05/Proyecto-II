#pragma once

#include "Module.h"
#include <list>
#include <vector>
#include "Physics.h"

// L10: TODO 2: Define a property to store the Map Orientation and Load it from the map
enum MapOrientation
{
    ORTOGRAPHIC = 0,
    ISOMETRIC
};


// L09: TODO 5: Add attributes to the property structure
struct Properties
{
    struct Property
    {
        std::string name;
        bool value; //We assume that we are going to work only with bool for the moment
    };

    std::list<Property*> propertyList;

    ~Properties()
    {
        for (const auto& property : propertyList)
        {
            delete property;
        }

        propertyList.clear();
    }

    // L09: DONE 7: Method to ask for the value of a custom property
    Property* GetProperty(const char* name);

};


struct MapLayer
{
    // L07: TODO 1: Add the info to the MapLayer Struct
    int id;
    std::string name;
    int width;
    int height;
    std::vector<int> tiles;
    Properties properties;

    // L07: TODO 6: Short function to get the gid value of i,j
    int Get(int i, int j) const
    {
        return tiles[(j * width) + i];
    }
};

struct Object
{
    int id;
    int x;
    int y;
    int width;
    int height;
    std::string name;
};

struct ObjectGroup
{
    int id;
    std::string name;
    std::vector<Object*> object;
};



struct Paralax
{
    std::string name;
    int spacing;
    int marginX;
    int marginY;
    int width;
    int height;
    int slowX;
    int slowY;
    int repeatNum;
    bool loaded;
    SDL_Texture* texture;
};

struct TileSet
{
    int firstGid;
    std::string name;
    int tileWidth;
    int tileHeight;
    int spacing;
    int margin;
    int tileCount;
    int columns;
    SDL_Texture* texture;

    // L07: TODO 7: Implement the method that receives the gid and returns a Rect
    SDL_Rect GetRect(unsigned int gid) {
        SDL_Rect rect = { 0 };

        int relativeIndex = gid - firstGid;
        rect.w = tileWidth;
        rect.h = tileHeight;
        rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
        rect.y = margin + (tileHeight + spacing) * (relativeIndex / columns);

        return rect;
    }

};

// L06: TODO 1: Create a struct needed to hold the information to Map node
struct MapData
{
	int width;
	int height;
	int tileWidth;
	int tileHeight;
    std::list<TileSet*> tilesets;
    std::list<Paralax*> paralaxs;

    MapOrientation orientation;

    // L07: TODO 2: Add the info to the MapLayer Struct
    std::list<MapLayer*> layers;
    std::list<ObjectGroup*> objectsGroups;
};

class Map : public Module
{
public:

    Map(bool startEnabled);

    // Destructor
    virtual ~Map();

    // Called before render is available
    bool Awake();

    // Called before the first frame
    bool Start();

    // Called each loop iteration
    bool Update(float dt);
    
    bool PostUpdate();

    // Called before quitting
    bool CleanUp();

    // Load new map
    bool Load(std::string path, std::string mapFileName);

    // L07: TODO 8: Create a method that translates x,y coordinates from map positions to world positions
    Vector2D MapToWorld(int x, int y) const;

    Vector2D MapToWorldCentered(int x, int y);
    
    Vector2D WorldToWorldCenteredOnTile(int x, int y);

    

    // L10: TODO 5: Add method WorldToMap to obtain  map coordinates from screen coordinates 
    Vector2D WorldToMap(int x, int y);

    // L09: TODO 2: Implement function to the Tileset based on a tile id
    TileSet* GetTilesetFromTileId(int gid) const;

    // L09: TODO 6: Load a group of properties 
    bool LoadProperties(pugi::xml_node& node, Properties& properties);

    int GetWidth() {
        return mapData.width;
    }

    int GetHeight() {
        return mapData.height;
    }

    int GetTileWidth() {
        return mapData.tileWidth;
    }

    int GetTileHeight() {
        return mapData.tileHeight;
    }

    MapLayer* GetNavigationLayer();

    bool LoadParalax(pugi::xml_node node);
    /*void LoadParalaxLayers(std::list<Paralax*> paralaxs);*/
    void SetParameters(pugi::xml_node parameters) {
        this->mapParameters = parameters;
    }

    void ActivateBossFightCollider(bool activate)
    {
        for (PhysBody* b : bossFightColliders)
        {
            b->body->SetEnabled(activate);
        }
    }



public: 
    std::string mapFileName;
    std::string mapPath;

private:
    bool mapLoaded;
    // L06: DONE 1: Declare a variable data of the struct MapData
    MapData mapData;
    /*Paralax* paralaxLayers;*/
    pugi::xml_node mapParameters;
    std::list<PhysBody*> bossFightColliders;
    
};