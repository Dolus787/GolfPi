#pragma once
struct MapTile
{
	//ModelId tileModel;
};
enum TileType
{
    C,
    castle,
    onewall,
    straight,
    threewalls,
    open,
    windmill,
    nothing
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
	glm::vec2 goal = {0,0};
	int width = 2;
	GolfMap() = default;
	GolfMap(std::string newMap, glm::vec2 mapSpawn, glm::vec2 mapGoal, int mapWidth) :
		map(newMap), spawnPoint(mapSpawn), goal(mapGoal), width(mapWidth){}
};