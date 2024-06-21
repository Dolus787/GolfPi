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

MapTile::MapTile(char tileChar) : tileChar(tileChar){}

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
	if (tileChar == 'O')
	{
		return 0;
	}
	else
	{
		//order: below, right, above, left
		std::vector<bool> emptyTilesFoundList = {false, false, false, false};
		int aboveTile = tileLoc - map.width, belowTile = tileLoc + map.width, leftTile = tileLoc - 1, rightTile = tileLoc + 1;
		if (belowTile >= map.map.length() || map.map[belowTile] == '0')
		{
			// std::cout << "below: " << belowTile << std::endl;
			emptyTilesFoundList[0] = true;
		}
		if (leftTile / map.width != tileLoc / map.width || leftTile < 0 || map.map[leftTile] == '0')
		{
			 // std::cout << "left: " << leftTile << std::endl;
			emptyTilesFoundList[1] = true;
		}
		if (aboveTile < 0 || map.map[aboveTile] == '0')
		{
			// std::cout << "above: " << aboveTile << std::endl;
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
			if (emptyTilesFoundList[0] || emptyTilesFoundList[2] || map.map[leftTile] == 'G' || map.map[rightTile] == 'G' || map.map[leftTile] == '2' || map.map[rightTile] == '2')
			{
				return 0;
			}
			else
			{
				rotation += 90;
			}
		}
		else if (tileChar == '1'|| tileChar == 'W' || tileChar == 'l' || tileChar == 'r')
		{
			for (int i = 0; i < emptyTilesFoundList.size(); i++)
			{
				if (emptyTilesFoundList[i])
				{
					rotation += 90 * i;
					break;
				}
			}
		}
		else if (tileChar == 'C' || tileChar == 'c' || tileChar == 'I')
		{
			if (emptyTilesFoundList[2] == true && emptyTilesFoundList[3] == true)
			{
				// std::cout << "here1" << std::endl;
				rotation += 270;
			}
			else if (emptyTilesFoundList[2] == true && emptyTilesFoundList[1] == true)
			{
				// std::cout << "here2" << std::endl;
				rotation += 180;
			}
			else if (emptyTilesFoundList[0] == true && emptyTilesFoundList[3] == true)
			{
				// std::cout << "here3" << std::endl;
				rotation += 0;
			}
			else if (emptyTilesFoundList[0] == true && emptyTilesFoundList[1] == true)
			{
				// std::cout << "here4" << std::endl;
				rotation += 90;
			}
		}
		else if (tileChar == '3' || tileChar == 'G')
		{
			for (int i = 0; i < emptyTilesFoundList.size(); i++)
			{
				if (!emptyTilesFoundList[i] && i != 1 && i != 3)
				{
					rotation -= 90 * (i + 1);
					break;
				}
				else if (!emptyTilesFoundList[i])
				{
					rotation += 90 * (i - 1);
					break;
				}
			}
		}
		return ( rotation * PI / 180);
	}
	return 0.0f;
}

GolfMap::GolfMap(std::string newMap, glm::vec3 mapSpawn, int mapWidth) :
map(newMap), spawnPos(mapSpawn), width(mapWidth)
{
	spawnPosInt = spawnPos.y * width + spawnPos.z;
	//the maps are offset by one in z because the model at world origin wouldn't load
	spawnPos.z += 1;
	std::cout << spawnPosInt << std::endl;
	goalPos = glm::vec3(map.find('G') % width, map.find('G') / width, 0);

	std::cout << "spawn: " << spawnPos.x << " " << spawnPos.y << " goal: " << goalPos.x << " " << goalPos.y << std::endl;
}

//Distance between the maps in the x coordinate
const int mapOffset = 40;
//const float tileOffset = 0.95;
void MapManager::SpawnMaps()
{
	// spawn all maps in the world
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
					(float)(i / maps[mapCount].width) + mapOffset * mapCount,
					0,
					(i % maps[mapCount].width)+ 1 
				);

				// World space goal position
				maps[mapCount].goalPos = { translation.x, translation.z };

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
				glm::vec3 translation = glm::vec3(
					// iterate the map's width and add map offset
					(float)(i / maps[mapCount].width) + mapOffset * mapCount,
					0, 
					// iterate every step reset at the map's width
					(i % maps[mapCount].width) + 1
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
int MapManager::NextMap()
{
	return selectedMap = (selectedMap + 1) % maps.size();
}