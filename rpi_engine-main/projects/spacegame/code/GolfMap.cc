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

#define PI 3.1415926535f

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
	//THESE ARE WRONG GOTTA MAKE SURE ITS ALWAYS ON THE SAME LINE AND NOT CHECKING THE NEXT LINE ON THE MAP
	//THESE ARE WRONG GOTTA MAKE SURE ITS ALWAYS ON THE SAME LINE AND NOT CHECKING THE NEXT LINE ON THE MAP
	//THESE ARE WRONG GOTTA MAKE SURE ITS ALWAYS ON THE SAME LINE AND NOT CHECKING THE NEXT LINE ON THE MAP
	//THESE ARE WRONG GOTTA MAKE SURE ITS ALWAYS ON THE SAME LINE AND NOT CHECKING THE NEXT LINE ON THE MAP
	//THESE ARE WRONG GOTTA MAKE SURE ITS ALWAYS ON THE SAME LINE AND NOT CHECKING THE NEXT LINE ON THE MAP
	//THESE ARE WRONG GOTTA MAKE SURE ITS ALWAYS ON THE SAME LINE AND NOT CHECKING THE NEXT LINE ON THE MAP
		int aboveTile = tileLoc - map.width, belowTile = tileLoc + map.width, rightTile = tileLoc + 1, leftTile = tileLoc - 1;
		adjacentTilesLoc = { aboveTile, belowTile, rightTile, leftTile };
}
float MapTile::SetRotation(std::string map)
{
	if (!manualRot)
	{
		if (tileType == 'O')
		{
			return 0;
		}
		else
		{
			std::cout << "start of setRot" << std::endl;
			//above, below, right, left
			std::vector<bool> emptyTilesFoundList = {false, false, false, false};
			for (int i = 0; i < adjacentTilesLoc.size(); i++)
			{
				if (adjacentTilesLoc[i] >= 0 && adjacentTilesLoc[i] < map.length())
				{
					if (map[adjacentTilesLoc[i]] == '0')
					{
						emptyTilesFoundList[i] = true;
					}
					continue;
				}
				emptyTilesFoundList[i] = true;
			}
			if (tileType == 'C')
			{
				if (emptyTilesFoundList[0] == true && emptyTilesFoundList[2] == true)
				{
					std::cout << "here" << std::endl;
					rotation += 180;
				}
				else if (emptyTilesFoundList[0] == true && emptyTilesFoundList[3] == true)
				{
					std::cout << "here" << std::endl;
					rotation += 270;
				}
				else if (emptyTilesFoundList[1] == true && emptyTilesFoundList[2] == true)
				{
					std::cout << "here" << std::endl;
					rotation += 90;
				}
				else if (emptyTilesFoundList[1] == true && emptyTilesFoundList[3] == true)
				{
					std::cout << "here" << std::endl;
					rotation += 0;
				}
			}
			if (tileType == '2')
			{
				if (emptyTilesFoundList[0] || emptyTilesFoundList[1])
				{
					return 0;
				}
				else
				{
					rotation += 90;
				}
			}
			return ( rotation * PI / 180);
		}
	}
	return 0.0f;
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
				tile.SetAdjacents(maps[mapCount], i);
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
				tile.rotation = tile.SetRotation(maps[mapCount].map);
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