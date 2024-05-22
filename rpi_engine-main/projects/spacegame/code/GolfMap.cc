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

MapTile::MapTile(char tileChar) : tileChar(tileChar)
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
		if (tileChar == tileStringTypes[i])
		{
			return i;
		}
	}
	std::cout << "[NO MODEL FOUND FOR TILE]" << std::endl;
	//9 is out of range for the models list
	return 9;
}
float MapTile::SetRotation(GolfMap map, int tileLoc)
{
	
	if (!manualRot)
	{
		if (tileChar == 'O')
		{
			return 0;
		}
		else
		{
			//order: above, below, left, right
			std::vector<bool> emptyTilesFoundList = {false, false, false, false};
			int aboveTile = tileLoc - map.width, belowTile = tileLoc + map.width, leftTile = tileLoc - 1, rightTile = tileLoc + 1;
			if (aboveTile < 0 || map.map[aboveTile] == '0')
			{
				// std::cout << "above: " << aboveTile << std::endl;
				emptyTilesFoundList[0] = true;
			}
			if (belowTile >= map.map.length() || map.map[belowTile] == '0')
			{
				// std::cout << "below: " << belowTile << std::endl;
				emptyTilesFoundList[1] = true;
			}
			if (leftTile / map.width != tileLoc / map.width || leftTile < 0 || map.map[leftTile] == '0')
			{
				 // std::cout << "left: " << leftTile << std::endl;
				emptyTilesFoundList[2] = true;
			}
			if (rightTile / map.width != tileLoc / map.width || rightTile > map.map.length() || map.map[rightTile] == '0')
			{
				// std::cout << "right: " << rightTile << std::endl;
				emptyTilesFoundList[3] = true;
			}

			//Commence magic number rotation

			if (tileChar == '2' || tileChar == 'H')
			{
				if (emptyTilesFoundList[0] || emptyTilesFoundList[1] || map.map[leftTile] == 'G' || map.map[rightTile] == 'G')
				{
					return 0;
				}
				else if (emptyTilesFoundList[2] || emptyTilesFoundList[3] || map.map[aboveTile] == 'G' || map.map[belowTile] == 'G')
				{
					rotation += 90;
				}
			}
			else if (tileChar == '1')
			{
				if (emptyTilesFoundList[0] == true)
				{
					rotation += 180;
				}
				else if (emptyTilesFoundList[1] == true)
				{
					rotation += 0;
				}
				else if (emptyTilesFoundList[2] == true)
				{
					rotation += 90;
				}
				else if (emptyTilesFoundList[3] == true)
				{
					rotation += 270;
				}
			}
			else if (tileChar == 'C' || tileChar == 'c')
			{
				std::cout << tileLoc << " start of setRot 'C' list: " << emptyTilesFoundList[0] << emptyTilesFoundList[1] << emptyTilesFoundList[2] << emptyTilesFoundList[3] << std::endl;
				if (emptyTilesFoundList[0] == true && emptyTilesFoundList[3] == true)
				{
					// std::cout << "here1" << std::endl;
					rotation += 270;
				}
				else if (emptyTilesFoundList[0] == true && emptyTilesFoundList[2] == true)
				{
					// std::cout << "here2" << std::endl;
					rotation += 180;
				}
				else if (emptyTilesFoundList[1] == true && emptyTilesFoundList[3] == true)
				{
					// std::cout << "here3" << std::endl;
					rotation += 0;
				}
				else if (emptyTilesFoundList[1] == true && emptyTilesFoundList[2] == true)
				{
					// std::cout << "here4" << std::endl;
					rotation += 90;
				}
			}
			else if (tileChar == '3' || tileChar == 'G')
			{
				if (emptyTilesFoundList[2] == false)
				{
					rotation += 0;
				}
				else if (emptyTilesFoundList[0] == false)
				{
					rotation += 90;
				}
				else if (emptyTilesFoundList[1] == false)
				{
					rotation += 270;
				}
				else if (emptyTilesFoundList[3] == false)
				{
					rotation += 180;
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
const int mapOffset = 40;
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
				//tile.SetAdjacents(maps[mapCount], i);
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