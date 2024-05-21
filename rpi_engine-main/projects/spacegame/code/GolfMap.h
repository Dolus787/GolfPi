#pragma once

struct GolfMap;
 enum TileType
 {
     corner, 
     castle,
     onewall,
     straight,
     threewalls,
     open,
     windmill,
     goal,
     flag
 };
const std::string tileStringTypes = "CH123OWG";
struct MapTile
{
    //std::tuple<Render::ModelId, Physics::ColliderId, glm::mat4> tile;
    //TileType type;
    Render::ModelId model = Render::ModelId();
    Physics::ColliderId collider = Physics::ColliderId();
    glm::mat4 transform = glm::mat4();
    float rotation = 0;
    bool manualRot = false;
    MapTile() = default;
    //Set what model the tile sohuld have
    size_t SetTileType(char tileChar);
    //Set rotation depending on surrounding tiles or manual set rotation
    float SetRotation(GolfMap map, int tile);
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
        Render::LoadModel("assets/golf/GLB/windmill.glb"),
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
		Physics::LoadColliderMesh("assets/golf/GLB/windmill.glb"),
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
    glm::vec3 goalPos = {2,2,1};
	int goalPosInt = 3;
	int width = 2;
	GolfMap() = default;
    GolfMap(std::string newMap, glm::vec3 mapSpawn, glm::vec3 mapGoal, int mapWidth, std::string manualRotation = "");
};
