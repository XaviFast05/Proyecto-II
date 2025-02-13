#pragma once
#include <list>
#include <queue>
#include <functional> // for std::greater
#include "Vector2D.h"
#include "SDL2/SDL.h"
#include "Map.h"

enum ASTAR_HEURISTICS {
    MANHATTAN = 0,
    EUCLIDEAN,
    SQUARED
};

class Pathfinding
{

public:

    enum PATH_MOVEMENT_TYPE
    {
        WALK = 0,
        FLY
    };
    Pathfinding();

    ~Pathfinding();

    // L11: BFS Pathfinding methods
    void ResetPath(Vector2D pos);
    void DrawPath();
    bool IsFlyable(int x, int y);
    bool IsWalkable(int x, int y);
    bool IsJumpable(int x, int y);

    void PropagateBFS();

    // L12: Methods for BFS + Pathfinding and cost function for Dijkstra
    int MovementCost(int x, int y);
    void ComputePath(int x, int y);
    void PropagateDijkstra();
    bool CleanUp();

    // L13: A* Pathfinding methods
    void PropagateAStar(ASTAR_HEURISTICS heuristic, Vector2D destination, PATH_MOVEMENT_TYPE movementType = Pathfinding::FLY);

private:
    int Find(std::vector<Vector2D> vector, Vector2D elem);

public:

    // L11: BFS Pathfinding variables
    Map* map;
    MapLayer* layerNav;
    std::queue<Vector2D> frontier;
    std::vector<Vector2D> visited;
    SDL_Texture* pathTex = nullptr;
    Vector2D destination;

    // L12: Dijkstra Pathfinding variables
    std::priority_queue<std::pair<int, Vector2D>, std::vector<std::pair<int, Vector2D>>, std::greater<std::pair<int, Vector2D>> > frontierDijkstra;
    std::vector<Vector2D> breadcrumbs; //list of tiles that form the path
    std::vector<std::vector<int>> costSoFar; //matrix that stores the accumulated cost in the propagation of the Dijkstra algorithm
    std::list<Vector2D> pathTiles; //list of tiles that form the path
    SDL_Texture* tileX = nullptr; //texture used to show the path 

    // L13: A* Pathfinding variables
    std::priority_queue<std::pair<int, Vector2D>, std::vector<std::pair<int, Vector2D>>, std::greater<std::pair<int, Vector2D>> > frontierAStar;
    
    float lowestDistance;
    Vector2D nearestTile;

    int maxIterations = 20;
    int iterations;

    
    int blockedGid = 296; //Gid of the tiles that block the path - Important adjust this value to your map
    int walkableGid = 297; //Gid of the tiles that block the path - Important adjust this value to your map
    int jumpableGid = 298; //Gid of the tiles that block the path - Important adjust this value to your map
    int highCostGid = 50; //Gid of the tiles that have high cost - Important adjust this value to your map
    int checkPointGid = 295;
};

