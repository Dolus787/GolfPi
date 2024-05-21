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

MapTile::MapTile(char tileChar) : tileType(tileChar)
{
	if (tileChar == 'O')
	{
		return;
	}
	else if (tileChar == '1')
	{
		nrOfWalls = 1;
	}
	else if (tileChar == 'C' || tileChar == '2' || tileChar == 'H')
	{
		nrOfWalls = 2;
	}
	else if (tileChar == '3' || tileChar == 'G')
	{
		nrOfWalls = 3;
	}
}

size_t MapTile::SetTileType()
{
	for (int i = 0; i < tileStringTypes.size(); i++)
	{
		if (tileType == tileStringTypes[i])
		{
			return i;
		}
	}
	std::cout << "[NO MODEL FOUND FOR TILE]" << std::endl;
	//9 is out of range for the models list
	return 9;
}
void MapTile::SetAdjacents(GolfMap map, int tileLoc)
{
		int aboveTile = tileLoc - map.width, belowTile = tileLoc + map.width, rightTile = tileLoc + 1, leftTile = tileLoc - 1;
		adjacentTilesLoc = { aboveTile, belowTile, rightTile, leftTile };
}
float MapTile::SetRotation()
{
	if (!manualRot)
	{
		if (tileType == 'O')
		{
			return 0;
		}
		else
		{
			int emptyTilesFound = 0;
			for (int i = 0; i < adjacentTilesLoc.size(); i++)
			{
				/*if (adjacentTilesLoc[i] == '0' && emptyTilesFound < nrOfWalls)
				{
					emptyTilesFound++;
				}*/
			}
		}
	}
	return 0;
}

GolfMap::GolfMap(std::string newMap, glm::vec3 mapSpawn, int mapWidth, std::string manualRotation) :
map(newMap), spawnPos(mapSpawn), width(mapWidth)
{
	goalPosInt = map.find('G');
	goalPos = glm::vec3(goalPosInt % width, goalPosInt / width, 0);

	std::cout << "spawn: " << spawnPos.x << " " << spawnPos.y << " goal: " << goalPos.x << " " << goalPos.y << std::endl;
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
			if (maps[mapCount].map[i] == 'G')
			{
				MapTile goalFlag = MapTile('F');
				goalFlag.model = models[flag];
				glm::vec3 translation = glm::vec3
				(
					(int)(i / maps[mapCount].width) + mapOffset * mapCount,
					0,
					(i % maps[mapCount].width) + 1 
				);
				glm::vec3 rotationAxis = normalize(translation);
				glm::mat4 transform = glm::translate(translation) * glm::rotate(goalFlag.rotation, rotationAxis);
				goalFlag.collider = Physics::CreateCollider(colliderMeshes[flag], transform);
				goalFlag.transform = transform;
				tiles.push_back(goalFlag);
			}
            if (maps[mapCount].map[i] != '0')
            {
				MapTile tile = MapTile(maps[mapCount].map[i]);
				size_t resourceIndex = tile.SetTileType();
				tile.model = models[resourceIndex];
				//float span = 20.0f;
				glm::vec3 translation = glm::vec3(
					// iterate the map's width
					(int)(i / maps[mapCount].width) + mapOffset * mapCount,
					0, 
					// iterate every step reset at the map's width
					(i % maps[mapCount].width) +1
				);
				glm::vec3 rotationAxis = glm::vec3(0,1,0);
				tile.rotation = tile.SetRotation();
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