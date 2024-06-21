#pragma once
//------------------------------------------------------------------------------
/**
	Space game application

	(C) 20222 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/app.h"
#include "render/window.h"
#include "Golf.h"

struct MapManager;
namespace Game
{
	struct Highscore {
		char name[3];
		unsigned short hits;
	};

class SpaceGameApp : public Core::App
{
public:
	/// constructor
	SpaceGameApp();
	/// destructor
	~SpaceGameApp();

	/// open app
	bool Open();
	/// run app
	void Run();
	/// exit app
	void Exit();
	/// restart map
	void ResetMap();
	/// switch map
	void SwitchMap(MapManager* manager);
	/// select name
	void SelectName();
	/// save balls score
	void SaveScore();
	/// save balls score to file
	void ReadScore();
	/// insert balls score to highscores
	void InsertScore();

private:

	/// render UI with nanovg
	void RenderUI(NVGcontext* vg);
	Game::GolfBall ball;
	//MapManager* mapManager = new MapManager();
	//3 maps, 5 highscores éach
	Highscore highscores[3][5];

	const char* fileName = "HighScores.txt";
	//Name of player
	char name[3] = { 'A' , 'A', 'A' };
	unsigned short charIndex = 0;
	unsigned short mapIndex;

	Input::Gamepad Gamepad;
	Input::Keyboard *kbd;

	Display::Window* window;
};
} // namespace Game