#pragma once
using namespace Render;

struct GolfMap;
// enum TileType
// {
    // corner, 
    // castle,
    // onewall,
    // straight,
    // threewalls,
    // open,
    // windmill,
    // nothing
// };
std::string tileStringTypes = "CH123OW0";
struct MapTile
{
    //std::tuple<Render::ModelId, Physics::ColliderId, glm::mat4> tile;
    //TileType type;
    Render::ModelId model;
    Physics::ColliderId collider;
    glm::mat4 transform;
    //bool manualRot;
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
};

//------------------------------------------------------------------------------
/**
* Struct containing all the necessities for the golf maps, the map is a string
* Usage: GolfMap()
*/
struct GolfMap
{
	std::string map = "CCCC";
	glm::vec2 spawnPoint = {0,0};
	glm::vec2 goal = {2,2};
	int width = 2;
	GolfMap() = default;
	GolfMap(std::string newMap, glm::vec2 mapSpawn, glm::vec2 mapGoal, int mapWidth, std::string manualRotation = "") :
		map(newMap), spawnPoint(mapSpawn), goal(mapGoal), width(mapWidth)
    {
        if (manualRotation != "")
        {

        }
    }
};
void LoadMap(MapManager* manager)
{
    for (int i = 0; i < manager->maps[manager->selectedMap].map.size(); i++)
    {
        
        MapTile tile;
        size_t resourceIndex = tile.SetTileType(manager->maps[manager->selectedMap].map[i]);
        tile.model = manager->models[resourceIndex];
        float span = 20.0f;
        glm::vec3 translation = glm::vec3(
            // iterate every five steps
            (int)(i / manager->maps[manager->selectedMap].width)+1,
            0+1, 
            // iterate every step reset at five
            (i % manager->maps[manager->selectedMap].width) + 1
        );
        glm::vec3 rotationAxis = normalize(translation);
        float rotation = 0;
        glm::mat4 transform = glm::rotate(rotation, rotationAxis) * glm::translate(translation);
        tile.collider = Physics::CreateCollider(manager->colliderMeshes[resourceIndex], glm::translate(glm::vec3(0,0,0)));
        tile.transform = transform;
        manager->tiles.push_back(tile);
    }
}

