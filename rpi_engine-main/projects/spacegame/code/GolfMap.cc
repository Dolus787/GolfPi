#include "config.h"
#include "spacegameapp.h"
#include <cstring>
#include "imgui.h"
#include "render/renderdevice.h"
#include "render/shaderresource.h"
#include <vector>
#include "render/textureresource.h"
#include "render/model.h"
#include "render/cameramanager.h"
#include "render/lightserver.h"
#include "render/debugrender.h"
#include "core/random.h"
#include "render/input/inputserver.h"
#include "render/input/gamepad.h"
#include "core/cvar.h"
#include "render/physics.h"
#include <chrono>
#include "Golf.h"
#include <string>
#include "GolfMap.h"

size_t MapTile::SetTileType(char tileChar)
{
	for (int i = 0; i < tileStringTypes.size(); i++)
	{
		if (tileChar == tileStringTypes[i])
		{
			return i;
		}
		/*else
		{
			std::cout
			return "[NO MODEL FOUND FOR TILE]"
		}*/
	}
}
float MapTile::SetRotation(GolfMap map, int tileLoc)
{
	if (!manualRot)
	{
		int aboveTile = tileLoc - map.width, belowTile = tileLoc + map.width, rightTile = tileLoc + 1, leftTile;
	}
	return 0;
}

GolfMap::GolfMap(std::string newMap, glm::vec3 mapSpawn, glm::vec3 mapGoal, int mapWidth, std::string manualRotation) :
map(newMap), spawnPos(mapSpawn), goalPos(mapGoal), width(mapWidth)
{
	goalPosInt = mapGoal.x + (mapGoal.y * mapWidth);
	std::cout << "spawn: " << spawnPos.length() << " goal: " << goalPosInt << std::endl;
	if (manualRotation != "")
	{

	}
}

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
				goalFlag.model = models[flag];
				glm::vec3 translation = glm::vec3
				(
					(int)(i / maps[mapCount].width) + mapOffset * mapCount,
					1,
					(i % maps[mapCount].width) + 1
				);
				glm::vec3 rotationAxis = normalize(translation);
				float rotation = 0;
				glm::mat4 transform = glm::translate(translation) * glm::rotate(rotation, rotationAxis);
				goalFlag.collider = Physics::CreateCollider(colliderMeshes[flag], transform);
				goalFlag.transform = transform;
				tiles.push_back(goalFlag);
			}
            if (maps[mapCount].map[i] != '0')
            {
				MapTile tile = MapTile();
				size_t resourceIndex = tile.SetTileType(maps[mapCount].map[i]);
				tile.model = models[resourceIndex];
				//float span = 20.0f;
				glm::vec3 translation = glm::vec3(
					// iterate the map's width
					(int)(i / maps[mapCount].width) + mapOffset * mapCount,
					0, 
					// iterate every step reset at the map's width
					(i % maps[mapCount].width)
				);
				glm::vec3 rotationAxis = glm::vec3(0,1,0);
				tile.rotation = tile.SetRotation(maps[mapCount], i);
				glm::mat4 transform = glm::translate(translation) * glm::rotate(tile.rotation, rotationAxis);
				tile.collider = Physics::CreateCollider(colliderMeshes[resourceIndex], transform);
				tile.transform = transform;
				tiles.push_back(tile);
            }
		}
    }
}
void MapManager::LoadMap()
{

}