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
     windmill,
     nothing
 };
std::string tileStringTypes = "CH123OW0";
struct MapTile
{
    //std::tuple<Render::ModelId, Physics::ColliderId, glm::mat4> tile;
    //TileType type;
    Render::ModelId model;
    Physics::ColliderId collider;
    glm::mat4 transform;
    //bool manualRot;
    MapTile()
    {

    }
    size_t SetTileType(char tileChar)
    {
        for (int i = 0; i < tileStringTypes.size(); i++)
        {
            if (tileChar == tileStringTypes[i])
            {
                return i;
            }
        }
    }
};
struct MapManager
{
    std::vector<GolfMap> maps;
    int selectedMap = 0;
    ModelId models[9] =
    {
        LoadModel("assets/golf/GLB/corner.glb"),
        LoadModel("assets/golf/GLB/castle.glb"),
        LoadModel("assets/golf/GLB/side.glb"),
        LoadModel("assets/golf/GLB/straight.glb"),
        LoadModel("assets/golf/GLB/end.glb"),
        LoadModel("assets/golf/GLB/open.glb"),
        LoadModel("assets/golf/GLB/windmill.glb"),
        LoadModel("assets/golf/GLB/club-red.glb"),
        LoadModel("assets/golf/GLB/flag-red.glb")
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
		Physics::LoadColliderMesh("assets/golf/GLB/club-red.glb"),
		Physics::LoadColliderMesh("assets/golf/GLB/flag-red.glb"),
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
* Usage: GolfMap()
*/
struct GolfMap
{
	std::string map = "CCCC";
    glm::vec3 spawnPos = {0,1,0};
    glm::vec3 goalPos = {2,1,2};
	int goalPosInt = 3;
	int width = 2;
	GolfMap() = default;
	GolfMap(std::string newMap, glm::vec3 mapSpawn, glm::vec3 mapGoal, int mapWidth, std::string manualRotation = "") :
		map(newMap), spawnPos(mapSpawn), goalPos(mapGoal), width(mapWidth)
    {
        goalPosInt = mapGoal.x + (mapGoal.y * mapWidth);
        std::cout << "spawn: " << spawnPos.length() << " goal: " << goalPosInt << std::endl;
        if (manualRotation != "")
        {

        }
    }
};
//Distance between the maps in the x coordinate
const int mapOffset = 80;
void MapManager::SpawnMaps()
{
    for (int mapCount = 0; mapCount < maps.size(); mapCount++)
    {
        maps[mapCount].goalPos.x += mapOffset * mapCount;
        maps[mapCount].spawnPos.x += mapOffset * mapCount;
		for (int i = 0; i < maps[mapCount].map.size(); i++)
		{
			if (i == maps[mapCount].goalPosInt)
			{
				MapTile goalFlag = MapTile();
			}
			MapTile tile = MapTile();
			size_t resourceIndex = tile.SetTileType(maps[mapCount].map[i]);
			tile.model = models[resourceIndex];
			float span = 20.0f;
			glm::vec3 translation = glm::vec3(
				// iterate the map's width
				(int)(i / maps[mapCount].width) + mapOffset * mapCount,
				0, 
				// iterate every step reset at the map's width
				(i % maps[mapCount].width) + 1
			);
			glm::vec3 rotationAxis = normalize(translation);
			float rotation = 0;
			glm::mat4 transform = glm::rotate(rotation, rotationAxis) * glm::translate(translation);
			tile.collider = Physics::CreateCollider(colliderMeshes[resourceIndex], glm::translate(glm::vec3(0,0,0)));
			tile.transform = transform;
			tiles.push_back(tile);
		}
    }
}
void MapManager::LoadMap()
{

}

