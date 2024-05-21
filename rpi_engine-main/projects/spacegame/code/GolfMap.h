#pragma once
using namespace Render;

struct GolfMap;
 enum TileType
 {
     corner, 
     castle,
     onewall,
     straight,
     threewalls,
     open,
     goal,
     flag
 };
 //the different types of tiles that exist in a map string
const std::string tileStringTypes = "CH123OGF";
struct MapTile
{
    //std::tuple<Render::ModelId, Physics::ColliderId, glm::mat4> tile;
    //TileType type;
    Render::ModelId model = Render::ModelId();
    Physics::ColliderId collider = Physics::ColliderId();
    glm::mat4 transform = glm::mat4();
	std::vector<int> adjacentTilesLoc;
    //The amount of walls the tile has
    int nrOfWalls = 0;
    float rotation = 0;
    bool manualRot = false;
    //the char of the tile in the map string
    char tileType ='0';
    MapTile() = default;
    MapTile(char tileChar);
    //Set what model the tile sohuld have
    size_t SetTileType();
    //Set adjacent tiles to this tile
    void SetAdjacents(GolfMap map, int tile);
    //Set rotation depending on surrounding tiles or manual set rotation
    float SetRotation(std::string map);
};
struct MapManager
{
    //List of all maps
    std::vector<GolfMap> maps;
    int selectedMap = 0;
    Render::ModelId models[9] =
    {
        Render::LoadModel("assets/golf/GLB/corner.glb"),
        Render::LoadModel("assets/golf/GLB/castle.glb"),
        Render::LoadModel("assets/golf/GLB/side.glb"),
        Render::LoadModel("assets/golf/GLB/straight.glb"),
        Render::LoadModel("assets/golf/GLB/end.glb"),
        Render::LoadModel("assets/golf/GLB/open.glb"),
        Render::LoadModel("assets/golf/GLB/hole-square.glb"),
        Render::LoadModel("assets/golf/GLB/flag-red.glb"),
        Render::LoadModel("assets/golf/GLB/club-red.glb")
    };
    Physics::ColliderMeshId colliderMeshes[9] = 
    {
		Physics::LoadColliderMesh("assets/golf/GLB/corner.glb"),
		Physics::LoadColliderMesh("assets/golf/GLB/castle.glb"),
		Physics::LoadColliderMesh("assets/golf/GLB/side.glb"),
		Physics::LoadColliderMesh("assets/golf/GLB/straight.glb"),
		Physics::LoadColliderMesh("assets/golf/GLB/end.glb"),
		Physics::LoadColliderMesh("assets/golf/GLB/open.glb"),
		Physics::LoadColliderMesh("assets/golf/GLB/hole-square.glb"),
		Physics::LoadColliderMesh("assets/golf/GLB/flag-red.glb"),
		Physics::LoadColliderMesh("assets/golf/GLB/club-red.glb")
    };
    std::vector<MapTile> tiles;
    MapManager() = default;
    //Spawn all maps into the world
    void SpawnMaps();
    //Load a map (really just moving the ball to the position of the next selected map)
    void LoadMap();
};

//------------------------------------------------------------------------------
/**
* Struct containing all the necessities for the golf maps, the map is a string
* Usage: GolfMap(string map, vec3 spawn point, vec3 goal location, int map width)
*/
struct GolfMap
{
	std::string map = "CCCC";
    glm::vec3 spawnPos = {0,1,0};
    glm::vec3 goalPos = {2,2,0};
	int goalPosInt = 3;
	int width = 2;
	GolfMap() = default;
    GolfMap(std::string newMap, glm::vec3 mapSpawn, int mapWidth, std::string manualRotation = "");
};
