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

namespace Game
{
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
	/// switch map
	void SwitchMap();
	/// select name
	void SelectName();
	/// save balls score
	void SaveScore();

private:

	/// render some nanovg stuff
	void RenderUI(NVGcontext* vg);
	Game::GolfBall ball;

	const char* fileName = "HighScores.txt";
	//Name of player
	char name[3] = { 'A' , 'A', 'A' };
	unsigned int charIndex = 0;
	unsigned int mapIndex;

	Input::Gamepad Gamepad;
	Input::Keyboard *kbd;

	Display::Window* window;
};
} // namespace Game